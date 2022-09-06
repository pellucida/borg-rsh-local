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
# include	<pwd.h>
# include	<grp.h>


# if	!defined(_PATH_BORG)
# 	define	_PATH_BORG	"/bin/borg"
# endif

// Simplified from "errors/errors.h"
enum	{
	ok	= 0,
	err	= -1,
};
# define	ERRORS_STD_ENCODE(x)	(-(x))
# define	ERRORS_STD_DECODE(x)	(-(x))
# define	ERRORS_STD_ENOMEM	ERRORS_STD_ENCODE(ENOMEM)
# define	ERRORS_STD_ERANGE	ERRORS_STD_ENCODE(ERANGE)
# define	ERRORS_STD_ENOENT	ERRORS_STD_ENCODE(ENOENT)
# define	ERRORS_STD_EPERM	ERRORS_STD_ENCODE(EPERM)
# define	ERRORS_STD_E2BIG	ERRORS_STD_ENCODE(E2BIG)
# define	ERRORS_STD_EINVAL	ERRORS_STD_ENCODE(EINVAL)
# define	ERRORS_STD_ENAMETOOLONG	ERRORS_STD_ENCODE(ENAMETOOLONG)

typedef	int	result_t;

// Check for invalid returns and bail
static	void inline	_ZERO_ (result_t retval, char* file, int line, char* msg) {
	if (retval != 0) {
		char*	errmsg	= "";
		if (retval!=err)
			errmsg	= strerror (ERRORS_STD_DECODE(retval));
		fprintf (stderr, "FATAL: %s(%d) %s : %s\n", file, line, msg, errmsg);
		exit (EXIT_FAILURE);
	}
}
# define	Fail_(v)	_ZERO_((v),__FILE__, __LINE__, #v )
# define	False_(v)	_ZERO_((v)?ok:err,__FILE__, __LINE__, #v )

//-----------------------------------------------------------------
// Simplified from strcont/strvec.h
// Encapulate a simple argc, argv ADT
struct	strvec_t {
	size_t	size;
	size_t	count;
	char**	vec;
};
typedef	struct	strvec_t strvec_t;

// Simplified from  "policy_size/policy_size.h"

static  inline  size_t  POLICY_SIZE_STEP (size_t rq, size_t min, size_t unused_max, size_t step_shift){
	size_t  result  = (rq < min) ? min : rq;
	size_t  step_mask       = (~(size_t)(0))<<step_shift;
	result  = (rq + ~step_mask) & step_mask;
	return	result;
}
enum	{ // Note enums are int, size_t is long (uint64_t) 
	REQ_STEP_SHIFT = 4,
	REQ_MIN   = 1u<<REQ_STEP_SHIFT,
	REQ_STEP  = REQ_MIN,
};
static	inline	size_t policy_size (size_t req){
	return	POLICY_SIZE_STEP (req, REQ_MIN, 0, REQ_STEP_SHIFT);
}
//---
static	inline	size_t	strvec_count (strvec_t* sv){
	return	sv->count;
}
static	inline	char*	strvec_at (strvec_t* sv, size_t i) {
	char*	result	= 0;
	if (i < strvec_count (sv)){
		result	= sv->vec [i];
	}
	return	result;
}
static	result_t	strvec_grow (strvec_t* sv, size_t request) {
	result_t	result	= ok;
	size_t	size	= policy_size (request+1); // +1 for final NULL
	if (size > sv->size) {
		typeof(sv->vec)	vec	= realloc (sv->vec, size*sizeof(sv->vec[0]));
		if (vec) {
			size_t	i	= strvec_count (sv);
			for (; i < size; ++i){
				vec [i]	= 0;
			}
			sv->size	= size;
			sv->vec		= vec;
		}
		else	result	= ERRORS_STD_ENOMEM;
	}
	return	result;
}
# define	STRVEC_INIT	(strvec_t){ .size = 0, .count = 0, .vec = 0, }

static	result_t	strvec_append (strvec_t* sv, char* s){
	result_t	result	= ok;
	if (sv->count >= sv->size)
		result	= strvec_grow (sv, sv->count); 
	if (result==ok)
		sv->vec [sv->count++]	= s;
	return	ok;
}
static	inline	char**	strvec_vec (strvec_t* sv){
	return	sv->vec;
}
//-----------------------------------------------------------------


struct	remote_t {
	char	user [LOGIN_NAME_MAX];
	char	host [HOST_NAME_MAX+1];
	uid_t	uid;
	gid_t	gid;
	char	dir [PATH_MAX+1];

	strvec_t	argv;
	strvec_t	nargv;
	strvec_t	environ;
	
	char*	ssh_original_command;
	char*	restrict_path;
};
typedef	struct  remote_t	remote_t;


// The argc/argv in remote_t have been processed by process_argv()
//
// argv[0] = ssh, argv[1] = user@host, argv [2/BORGOFF] = "borg" etc
enum	{ BORGOFF = 2, };

static	result_t	make_ssh_original_command (remote_t* r) {
	result_t	result	= ERRORS_STD_ENOMEM;
	size_t	i	= 0;
	size_t	nchars	= 0;

	strvec_t*	argv	= &r->argv;
	size_t	argc	= strvec_count (argv);
	char*	original	= 0;
	for (i=BORGOFF; i < argc; ++i) {
		nchars	+= strlen (strvec_at (argv, i))+1;
	}
	original	= calloc (1, nchars);
	if (original) {
		size_t	next	= 0;
		for (i=BORGOFF; i < argc; ++i) {
			char*	argn	= strvec_at (argv,i);
			size_t	len	= strlen (argn);

			memcpy (&original[next],argn, len);
			next	+= len;
			original [next++]	= ' ';
		}
		original [next-1]	= '\0';
		r->ssh_original_command	= original;
		result	= ok;
	}
	return	result;
}
		
		
// user@remote 
static	result_t	remote_parse (remote_t* r) {
	result_t	result	= ERRORS_STD_ENAMETOOLONG;
	char*	user_host	= strvec_at (&r->argv, 1);
	char*	at	= strchr (user_host, '@');
	if (at) {
		size_t	userlen	= at - user_host;
		size_t	hostlen	= strlen (user_host) - (userlen+1);
		if (userlen < sizeof (r->user)) {
			memcpy (r->user, user_host, userlen);
			r->user [userlen]	= '\0';
			result	= ok;
		}
		if (result==ok) {
			if (hostlen < sizeof (r->host)) {
				memcpy (r->host, &user_host[userlen+1],hostlen);
				r->host [hostlen]	= '\0';
			}
			else	result	= ERRORS_STD_ENAMETOOLONG;
		}
	}
	return	result;
}
// lookup user from user@
static	result_t	lookup_user (remote_t* r) {
	result_t	result	= ok;
	struct	passwd*	pwd	= getpwnam (r->user);
	if (pwd) {
		size_t	len	= strlen (pwd->pw_dir);
		r->uid	= pwd->pw_uid;
		r->gid	= pwd->pw_gid;
		if (len+1 <= sizeof (r->dir)){
			memcpy (r->dir, pwd->pw_dir, len+1);
		}
		else	{
			result	= ERRORS_STD_ENAMETOOLONG;
		}
	}
	else	{
		result	= ERRORS_STD_ENOENT;
	}
	return	result;
}
// check host from user@host is local
static	int	host_is_local (remote_t* r){
	char*	rhost	= r->host;
	int	result	= strcmp (rhost, "localhost")==0
		|| strcmp (rhost, "127.0.0.1")==0
		|| strcmp (rhost, "0.0.0.0")==0
	;
	return	result;
}

// Add "key=value" to r->environ

static	result_t	environ_append (remote_t* r, char* key, char* value) {
	result_t	result	= ERRORS_STD_ENOMEM;
	size_t	klen	= strlen (key);
	size_t	vlen	= strlen (value);
	size_t	buflen	= klen+vlen+2;
	char*	buf	= calloc (buflen, 1);
	if (buf) {
		if (buflen >= snprintf (buf, buflen, "%s=%s", key, value))
			result	= strvec_append (&r->environ, buf);
		else	
			result	= ERRORS_STD_E2BIG;
	}
	return	result;
}
// Construct restricted environ[] in r->environ
static	result_t	setup_environ (remote_t* r) {
	result_t	result	= environ_append (r, "USER", 	r->user);
	if (result==ok)
		result	= environ_append (r, "LOGNAME",	r->user);
	if (result==ok)
		result	= environ_append (r, "HOME",	r->dir);
	if (result==ok)
		result	= environ_append (r, "SHELL",	"/bin/sh");
	if (result==ok)
		result	= environ_append (r, "LANG",	"C");
	if (result==ok)
		result	= environ_append (r, "SSH_ORIGINAL_COMMAND", r->ssh_original_command);
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
	uid_t	ruid	= r->uid;
	gid_t	rgid	= r->gid;
	if (caller==0) {
		if (ruid != 0) {
			setgid (rgid);
			initgroups(r->user, rgid);
			if (setuid (ruid)==ok) 
				result	= ok;
			else
				result	= ERRORS_STD_ENCODE(errno);
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
	strvec_t*	nargv	= &r->nargv;
	result_t	result	= strvec_append (nargv, "borg");
	if (result==ok)
		result	= strvec_append (nargv, "serve");
	if (result==ok)
		result	= strvec_append (nargv, "--umask");
	if (result==ok)
		result	= strvec_append (nargv, "077");

	if (result==ok && r->restrict_path) {
		result	= strvec_append (nargv, "--restrict-to-path");
		if (result==ok)
			result	= strvec_append (nargv, r->restrict_path);
	}
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
		size_t	i	= 0;
		strvec_t*	rargv	= &r->argv;
		result	= strvec_append (rargv, argv [0]);
		for (i=optind; result==ok && i < argc; ++i) {
			result	= strvec_append (rargv, argv [i]);
		}
	}
	return	result;
}
int	main (int argc, char* argv[]) {
	
	remote_t r	= { 
		.argv		= STRVEC_INIT, 
		.environ	= STRVEC_INIT, 
		.nargv		= STRVEC_INIT, 
		.restrict_path	= 0,
	};
	Fail_ (
		process_argv (&r, argc, argv)
	);

//[0]ssh [1]user@host [2]borg [3]serve [4]--umask=077
	if (strvec_count (&r.argv) == 5) {
		Fail_ (
			remote_parse (&r)
		);
		False_ (
			host_is_local (&r)
		); 
		Fail_ (
			make_ssh_original_command (&r)
		);
		Fail_ (
			lookup_user (&r)
		);
		Fail_ (
			become_user (&r)
		);
		Fail_ (
			setup_environ (&r)
		);
		Fail_ (
			setup_nargv (&r)
		);
		// Final check we aren't going to exec as root (uid==0)
		False_ (
			getuid()!=0
		);
		execve (_PATH_BORG, strvec_vec (&r.nargv), strvec_vec (&r.environ));
	}		
	exit (EXIT_FAILURE);
}
