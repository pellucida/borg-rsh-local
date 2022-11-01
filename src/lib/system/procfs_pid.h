/*
//  @(#) procfs_pid.h
*/

# if	!defined( PROCFS_PID_H)
# define	PROCFS_PID_H	1

# include	<unistd.h>
# include	"errors/errors.h"
# include	"string/str.h"
# include	"string/strvec.h"


result_t	str_getprocfs_pid(str_t* path, pid_t pid, str_t* file);
result_t	str_getargv_pid (strvec_t* argv, pid_t pid);
result_t	str_getexe_pid (str_t* prog, pid_t pid);
# endif
