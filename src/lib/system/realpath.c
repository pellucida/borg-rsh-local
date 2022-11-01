/*
// @(#) realpath.c
*/
# include	<errno.h>
# include	<stdlib.h>

# include	"errors/errors.h"
# include	"string/str.h"
# include	"system/realpath.h"

result_t	str_realpath (str_t* path, str_t* target) {
	result_t	result	= ok;
	char*   cpath   = str_cstring (path);
 	char*	tgt	= realpath (cpath, 0);
	if (tgt) {
		result	= str_copy_cstring (target, tgt);
		free (tgt);
	}
	else	result  = ERRORS_STD_ERRNO (errno);
	return	result;
}
