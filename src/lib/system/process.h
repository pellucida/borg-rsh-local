/*
// #(#) process.h
*/
# if	!defined(SYSTEM_PROCESS_H)
# define	SYSTEM_PROCESS_H	1

# include	"errors/errors.h"
# include	"string/str.h"
# include	"string/strvec.h"

result_t	str_execve (str_t* path, strvec_t* args, strvec_t* env);
result_t	str_execv (str_t* path, strvec_t* args);

# endif
