/*
// @(#) process.c
*/
# include	<unistd.h>
# include	<errno.h>
# include	<stdlib.h>

# if	!defined( _GNU_SOURCE)
extern	char** environ;
# endif
# include	"errors/errors.h"
# include	"string/str.h"
# include	"string/strvec.h"
# include	"system/process.h"

result_t	str_execve (str_t* path, strvec_t* args, strvec_t* env){
	result_t	result	= ok;
	char*	cpath	= 0;
	char**	argv	= 0;
	char**	envp	= 0;

	if (path==0 || args==0) {
		result	= ERRORS_STD_EINVAL;
	}
	if (result==ok) {
		cpath	= str_cstring (path);
		result	= strvec_argc_argv (args, 0, &argv);
	}
	if (result==ok) {
		if (env != 0) {
			result	= strvec_argc_argv (env, 0, &envp);
		}
		if (result==ok) {
			execve (cpath, argv, envp);
			result	= ERRORS_STD_ERRNO (errno);
		}
	}
	return	result;
}
	
result_t	str_execv (str_t* path, strvec_t* args){
	result_t	result	= ok;
	char*	cpath	= 0;
	char**	argv	= 0;

	if (path==0 || args==0) {
		result	= ERRORS_STD_EINVAL;
	}
	if (result==ok) {
		cpath	= str_cstring (path);
		result	= strvec_argc_argv (args, 0, &argv);
	}
	if (result==ok) {
		execve (cpath, argv, environ);
		result	= ERRORS_STD_ERRNO (errno);
	}
	return	result;
}
