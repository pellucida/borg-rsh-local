/*
// #(#) stat.h
*/
# if	!defined(SYSTEM_READLINK_H)
# define	SYSTEM_READLINK_H	1

# include       <sys/types.h>
# include       <sys/stat.h>

# include	"errors/errors.h"
# include	"string/str.h"

result_t	str_readlink (str_t* path, str_t* target);

# endif
