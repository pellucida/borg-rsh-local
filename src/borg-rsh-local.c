/*
// @(#) borg-rsh-local.c
//
// Emulate ssh to localhost for root dumps to repos owned
// by a non-root user typically on an NFS volume.
//
// # mount -o rw,nodev,noexec store:/rembck/borg /borg
// # ls -ld /borg/example 
// drwxrws--- . 4 root dumpuser ... /borg/example
//
// # borg init -e authenticated --rsh "/sbin/opt/borg-rsh-local -r /borg" \
//               dumpuser@localhost:/borg/example
//
// # borg create --rsh "/sbin/opt/borg-rsh-local -r /borg" \
//		 dumpuser@localhost:/borg/example::example /example
// 
*/
# include	<unistd.h>
# include	<errno.h>
# include	<stdio.h>
# include	<stdlib.h>
# include	<string.h>
# include	<limits.h>

# include	"errors/errors.h"

# include	"string/str.h"
# include	"string/strvec.h"

# include	"system/passwd.h"
# include	"system/process.h"
# include	"system/procfs_pid.h"


// Check for invalid returns and bail
static	void inline	_ZERO_ (result_t retval, char* file, int line, char* msg) {
	if (retval != 0) {
		char*	errmsg	= "general error(-1)";
		if (retval!=err) {
			int	erno	= error_errno (retval);
			errmsg	= strerror (erno);
		}
		fprintf (stderr, "FATAL: %s(%d) %s : %s\n", file, line, msg, errmsg);
		exit (EXIT_FAILURE);
	}
}
# define	Fail_(v)	_ZERO_((v),__FILE__, __LINE__, #v )
# define	Fail2_(v,m)	_ZERO_((v),__FILE__, __LINE__, #v ": "  m )
# define	False_(v)	_ZERO_((v)?ok:err,__FILE__, __LINE__, #v )
# define	False2_(v, m)	_ZERO_((v)?ok:err,__FILE__, __LINE__, #v ": " m)

//-----------------------------------------------------------------


struct	remote_t {
	str_t*	user;
	str_t*	host;
	str_passwd_t*	pw;
	strvec_t*	argv;
	strvec_t*	nargv;
	strvec_t*	environ;
	
	str_t*	borg_path;
	str_t*	ssh_original_command;
	char*	restrict_path;
};
typedef	struct  remote_t	remote_t;

// The argv in remote_t have been processed by process_argv()
//
// argv[0] = ssh, argv[1] = user@host, argv [2/BORGOFF] = "borg" etc
enum	{ BORGOFF = 2, };

static	result_t	make_ssh_original_command (remote_t* r) {
	size_t	i	= 0;
	strvec_t*	argv	= r->argv;
	size_t	argc	= strvec_used (argv);

	str_t*	arg	= 0;
	str_t*	original	= 0;

	result_t	result	= strvec_get (argv, BORGOFF, &arg);
	if (result==ok) {
		result	= str_Copy (&original, arg); 
		for (i=BORGOFF+1; result==ok && i < argc; ++i) {
			result	= str_append_char (original, ' ');
			if (result==ok) 
				result	= strvec_get (argv, i, &arg);
			if (result==ok) 
				result	= str_append (original, arg);
		}
	}
	if (result==ok) 
		r->ssh_original_command	= original;
	return	result;
}
// user@remote 
static	result_t	remote_parse (remote_t* r) {
	str_t*	user_host	= 0;
	result_t	result	= strvec_get (r->argv, 1, &user_host);
	if (result==ok) {
		ssize_t	position	= str_findchar (user_host, '@');
		if (position >= 0) {
			str_t*	user	= 0;
			str_t*	host	= 0;
			size_t	len	= str_length (user_host);
			result	= str_Create (&user, position);
			if (result==ok) {
				result	= str_substr (user, user_host, 0, position);
			}
			if (result==ok) {
				result	= str_Create (&host, len - position);
			}
			if (result==ok) {
				result	= str_substr (host, user_host, position+1, len - (position+1));
			}
			if (result==ok) {
				r->user	= user;
				r->host	= host;
			}
			else	{
				str_Delete (&user);
				str_Delete (&host);
			}
		}
		else	result	= ERRORS_STD_ENOENT;
	}
	return	result;
}
// lookup user from user@
static	result_t	lookup_user (remote_t* r) {
	str_passwd_t*	pw	= 0;
	result_t	result	= str_getpwnam_r (&pw, r->user);
	if (result==ok) {
		r->pw	= pw;
	}
	else	{
		result	= ERRORS_STD_ENOENT;
	}
	return	result;
}
// check host from user@host is local
static	int	host_is_local (remote_t* r){
	char*	rhost	= str_cstring (r->host);
	int	result	= strcmp (rhost, "localhost")==0
		|| strcmp (rhost, "127.0.0.1")==0
		|| strcmp (rhost, "0.0.0.0")==0
	;
	return	result;
}

// Add "key=value" to r->environ

static	result_t	environ_append (strvec_t* env, char* key, str_t* value) {
	
	str_t*	keyvalue	= 0;
	result_t	result	= str_Create_cstring (&keyvalue, key);
	if (result==ok)
		result	= str_append_char (keyvalue, '=');
	if (result==ok)
		result	= str_append (keyvalue, value);
	if (result==ok)
		result	= strvec_append (env, keyvalue);
	return	result;
}
static	result_t	environ_append_cstring (strvec_t* env, char* key, char* value) {
	str_t*	val	= 0;
	result_t	result	= str_Create_cstring (&val, value);
	if (result==ok) {
		result	= environ_append (env, key, val);
		str_Delete (&val);
	}
	return	result;
}
// Construct restricted environ[] in r->environ
static	result_t	setup_environ (remote_t* r) {
	strvec_t*	env	= 0;
	result_t	result	= strvec_Create (&env, 0);
	if (result==ok) {
		result	= environ_append (env, "USER", r->user);
		if (result==ok)
			result	= environ_append (env, "LOGNAME",	r->user);
		if (result==ok)
			result	= environ_append (env, "HOME",	r->pw->pw_dir);
		if (result==ok) 
			result	= environ_append_cstring (env, "SHELL", "/bin/sh");
		if (result==ok) 
			result	= environ_append_cstring (env, "LANG", "C");
		if (result==ok)
			result	= environ_append (env, "SSH_ORIGINAL_COMMAND", r->ssh_original_command);
		if (result==ok)
			r->environ	= env;
	}
	return	result;
}
//
//	If root(uid==0) drop privilege to become "remote" user
//	otherwise if running as the same uid as the remote user
//	do nothing (continue) otherwise an error.
//
static	result_t	become_user (remote_t* r) {
	result_t	result	= ERRORS_STD_EPERM;
	uid_t	caller	= getuid ();
	uid_t	ruid	= r->pw->pw_uid;
	gid_t	rgid	= r->pw->pw_gid;
	if (caller==0) {
		if (ruid != 0) {
			setgid (rgid);
			initgroups (str_cstring (r->user), rgid);
			if (setuid (ruid)==ok) 
				result	= ok;
			else
				result	= ERRORS_STD_ERRNO (errno);
		}
	}
	else if (ruid == caller) {
		result	= ok;
	}
	return	result;
}

//
// Construct argv[] for execve()
//
// Construct "forced command" - borg serv --restrict-to-path /repo --umask 077
// Borg will consult $SSH_ORIGINAL_COMMAND to augment.

static	result_t	setup_nargv (remote_t* r) {
	strvec_t*	nargv	= 0;
	result_t	result	= strvec_Create (&nargv, 0);
	if (result==ok)
		result	= strvec_append_cstring (nargv, "borg");
	if (result==ok) 
		result	= strvec_append_cstring (nargv, "serve");
	if (result==ok) 
		result	= strvec_append_cstring (nargv, "--umask");
	if (result==ok) 
		result	= strvec_append_cstring (nargv, "077");
	
	if (result==ok && r->restrict_path) {
		result	= strvec_append_cstring (nargv, "--restrict-to-path");
		if (result==ok)
			result	= strvec_append_cstring (nargv, r->restrict_path);
	}
	if (result==ok)
		r->nargv	= nargv;
	return	result;
}
static	result_t	process_argv (remote_t* r, int argc, char* argv[]){
	result_t	result	= ok;
	int	opt	= EOF;

	// '+' don't permute argv[]
	while ((opt = getopt (argc, argv, "+r:"))!=EOF &&result==ok) {
		switch (opt) {
		case	'r': 
			r->restrict_path = optarg;
		break;
		case	'?':
		default:
			result	= ERRORS_STD_EINVAL;
		break;
		}
	}
	if (result==ok) {
		strvec_t*	rargv	= 0;
		result	= strvec_Create (&rargv, argc);
		if (result==ok) {
			size_t	i	= 0;
			result	= strvec_append_cstring (rargv, argv [0]);
			for (i=optind; result==ok && i < argc; ++i) {
				result	= strvec_append_cstring (rargv, argv [i]);
			}
		}
		if (result==ok){
			r->argv	= rargv;
		}
	}
	return	result;
}

//[0]ssh [1]user@host [2]borg [3]serve [4]--umask=077

# define	match_literal(s,lit)	strncmp ((s), (lit), (sizeof(lit)-1))
static	result_t	verify_argv (strvec_t* argv) {
	result_t	result	= ERRORS_STD_EINVAL;
	size_t	used	= strvec_used (argv);
	if (used == 5 || used == 4){
		str_t*	arg	= 0;
		char*	args	= 0;
		result	= strvec_get (argv, 2, &arg);
		if (result==ok) {
			args	= str_cstring (arg);
			if (strcmp (args, "borg")!=0) 
				result	= ERRORS_STD_EINVAL;
		}
		if (result==ok) { 
			result	= strvec_get (argv, 3, &arg);
			if (result==ok){ 
				args	= str_cstring (arg);
				if (strcmp (args, "serve")!=0) 
					result	= ERRORS_STD_EINVAL;
			}
		}
		if (result==ok && used == 5) {
			result	= strvec_get (argv, 4, &arg);
			if (result==ok){ 
				args	= str_cstring (arg);
				if (match_literal (args, "--umask=")!=0) 
					result	= ERRORS_STD_EINVAL;
			}
		}
	}
	return	result;
}
// Retrieve the borg command (python script) from
// parent's command line
static	result_t	getparent_borg (remote_t* r) {
	pid_t	parent	= getppid ();
	strvec_t*	argv	= 0;
	result_t	result	= strvec_Create (&argv, 0);
	if (result==ok) { 
		result	= str_getargv_pid (argv, parent);
		if (result==ok) {
			str_t*	borg_path	= 0;
			result	= str_Create (&borg_path, 0);
			if (result==ok) {
				result	= strvec_get (argv, 1, &borg_path);
				if (result==ok)
					r->borg_path	= borg_path;
			}
		}	
		strvec_Delete (&argv);
	}
	return	result;
}


int	main (int argc, char* argv[]) {
	remote_t r	= { 
		.argv		= 0,
		.environ	= 0, 
		.pw		= 0,
		.nargv		= 0, 
		.borg_path	= 0,
		
		.restrict_path	= 0,
		.ssh_original_command	= 0,
	};
	Fail2_ (
		process_argv (&r, argc, argv), "invalid option passed via ssh"
	);
	Fail2_ (
		verify_argv (r.argv), "ssh command line from borg not expected"
	);
	
	Fail2_ (
		remote_parse (&r),	"Remote not user@host"
	);
	False2_ (
		host_is_local (&r),	"Remote not user@localhost"
	); 
	Fail2_ (
		make_ssh_original_command (&r), "Couldn't construct SSH_ORIGINAL_COMMAND"
	);
	Fail2_ (
		lookup_user (&r), 	"Local user in user@localhost lookup failed"
	);
	Fail2_ (
		become_user (&r),	"Uid==0 -> uid==0 not permitted! Must be root -> user or user -> same(user)"
	);
	Fail2_ (
		setup_environ (&r),	"Couldn't construct environment (envp) for execve(2)"
	);
	Fail2_ (
		setup_nargv (&r),	"Couldn't construct argv for execve(2)"
	);

	// Final check we aren't going to exec as root (uid==0)
	False2_ (
			getuid()!=0,	"Naughty! Still running as root (uid==0)"
	);
	Fail2_ (
		getparent_borg (&r),	"Could not retrieve original borg command from parent"
	);
	str_execve (r.borg_path, r.nargv, r.environ);

	exit (EXIT_FAILURE);
}
