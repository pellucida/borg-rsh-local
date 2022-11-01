/*
// @(#) stat.c
*/
# include	<sys/types.h>
# include	<sys/stat.h>
# include	<unistd.h>
# include	<errno.h>
# include	<limits.h>
# include	<stdlib.h>

# include	"errors/errors.h"
# include	"string/str.h"
# include	"system/stat.h"
# include	"system/readlink.h"

// Have to muck about as lstat of symlinks in /proc/<pid>/ return st_size==0
// So read into increasingly larger buffers until (size < request) 

result_t	str_readlink (str_t* path, str_t* target) {
	struct	stat	sb;
	result_t	result	= str_lstat (path, &sb);
	if (result == ok) {
		if (S_ISLNK (sb.st_mode)) {
			char*   cpath   = str_cstring (path);
			char*	tgt	= 0;
			ssize_t	nr	= 0;
			ssize_t	end	= -1;
			size_t	size	= sb.st_size;
			if (size==0) { // /proc ?
				size	= 64;
			}
			while (nr != end) {
				result	= mem_reallocate (&tgt, size+1);
				if (result==ok) {
					nr	= readlink (cpath, tgt, size+1);
					if (nr < 0 || nr > size+1) {
						nr	= end;
					}
					else if (nr < size+1) {
						tgt [nr]	= '\0';
						result	= str_copy_cstring (target, tgt);
						end	= nr;
					}
					else	{
						size	= 2*size;
					}
				}
				else	{
					nr	= end;
				}
			}
			mem_free (&tgt);
		}
		else	result	= ERRORS_STD_ERRNO (EINVAL);
	}
	else	result  = ERRORS_STD_ERRNO (errno);
	return	result;
}
# if	defined(SIMPLE_TEST)
# include	<stdio.h>
main (int argc, char* argv[]) {
	str_t*	p	= 0;
	str_t*	l	= 0;
	str_Create_cstring (&p, argv[1]);
	str_Create (&l, 0);
	if (ok==str_readlink (p, l)) {
		printf ("%s -> %s\n", str_cstring (p), str_cstring (l));
	}
}
# endif
