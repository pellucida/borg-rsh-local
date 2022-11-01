# include       <unistd.h>
# include       <errno.h>
# include       <stdlib.h>
# include       <stdio.h>
# include       <string.h>

# include       "errors/errors.h"
# include       "string/str.h"
# include       "string/strvec.h"
# include	"system/stat.h"
# include	"system/readlink.h"
# include	"system/procfs_pid.h"

result_t	str_getprocfs_pid(str_t* path, pid_t pid, str_t* file) {
	result_t	result	= err;
	char	pidpath [sizeof("/proc/") + sizeof(pid)*(8*300/1000)+2];

	int	size	= snprintf (pidpath, sizeof(pidpath), "/proc/%u/", pid);
	if (size < sizeof(pidpath)) {
		result	= str_copy_cstring (path, pidpath);
		if (result==ok) {
			result	= str_append (path, file);
		}
	}
	return	result;
}
	
result_t	str_getargv_pid (strvec_t* argv, pid_t pid) {
	str_t*	cmd	= 0;
	str_t*	file	= 0;
	result_t	result	= str_Create (&cmd, 0);
	if (result==ok) {
		result	= str_Create_cstring (&file, "cmdline");
		if (result==ok) {
			result	= str_getprocfs_pid (cmd, pid, file);
			if (result==ok) {
				FILE*	cf	= fopen (str_cstring (cmd), "rb");
				if (cf) {
					size_t	size	= 0;
					char*	arg	= 0;
					while (getdelim (&arg, &size, '\0', cf) != -1) {
						str_t*	copy	= 0;
						str_Create_cstring (&copy, arg);
						strvec_append (argv, copy);
					}
					free (arg);
					fclose (cf);
				}
				else	result = ERRORS_STD_ERRNO (errno);
			}
			str_Delete (&file);
		}
		str_Delete (&cmd);
	}
	return	result;
}

result_t	str_getexe_pid (str_t* prog, pid_t pid) {
	str_t*	exefile	= 0;
	result_t	result	= str_Create (&exefile, 0);
	if (result==ok) {
		str_t*	file	= 0;
		result	= str_Create_cstring (&file, "exe");
		if (result==ok) {
			result	= str_getprocfs_pid (exefile, pid, file);
			str_Delete (&file);
		}
		result	= str_readlink (exefile, prog);
		str_Delete (&exefile);
	}
	return	result;
}

# if 0
main(int argc, char* argv[]){
	int	ac;
	int	i	= 0;
	if (argc <= 1) {
		exit (0);
	}
	pid_t	pid	= atoi (argv[1]);
	strvec_t*	av	= 0;
	str_t*	exe	= 0;
	strvec_Create (&av, 0);
	str_Create (&exe, 0);
	str_getargv_pid (av, pid);
	str_getexe_pid (exe, pid);
	
	printf ("EXE %s\n", str_cstring (exe));
	for (i=0; i < strvec_used (av); ++i) {
		str_t*	arg	= 0;
		strvec_get (av, i, &arg);
		printf ("%s\n", str_cstring (arg));
	}
}
# endif
