/*
// #(#) stat.h
*/
# if	!defined(SYSTEM_STAT_H)
# define	SYSTEM_STAT_H	1

# include       <sys/types.h>
# include       <sys/stat.h>

# include	"errors/errors.h"
# include	"string/str.h"

result_t	str_stat (str_t* path, struct stat* sb);
result_t	str_lstat (str_t* path, struct stat* sb);

# endif
