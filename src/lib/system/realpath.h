/*
// #(#) stat.h
*/
# if	!defined(SYSTEM_REALPATH_H)
# define	SYSTEM_REALPATH_H	1

# include	<stdlib.h>

# include	"errors/errors.h"
# include	"string/str.h"

result_t	str_realpath (str_t* path, str_t* target);

# endif
