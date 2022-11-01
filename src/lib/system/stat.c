/*
// @(#) stat.c
*/
# include	<sys/types.h>
# include	<sys/stat.h>
# include	<unistd.h>
# include	<errno.h>
# include	<stdlib.h>

# include	"errors/errors.h"
# include	"string/str.h"
# include	"system/stat.h"

result_t	str_stat (str_t* path, struct stat* sb){
	result_t	result	= ok;
	char*	cpath	= str_cstring (path);
	if (stat (cpath, sb)!=0) 
		result	= ERRORS_STD_ERRNO (errno);
	return	result;
}
result_t	str_lstat (str_t* path, struct stat* sb){
	result_t	result	= ok;
	char*	cpath	= str_cstring (path);
	if (lstat (cpath, sb)!=0)
		result	= ERRORS_STD_ERRNO (errno);
	return	result;
}
