/*
//
*/
# if	!defined(STR_STR_H)
# define	STR_STR_H
# include	"errors/errors.h"


typedef	struct	str_t	str_t;
char* const	str_cstring (const str_t* str);
size_t	str_length (str_t* str);
size_t	str_size (str_t* str);
result_t	str_Create (str_t** spp, size_t size);
result_t	str_Create_cstring (str_t** spp, const char* cstr);
result_t	str_Copy (str_t** spp, const str_t* src);
result_t	str_copy (str_t* sp, const str_t* src);
result_t	str_copy_cstring (str_t* sp, const char* cstr);
result_t	str_Delete (str_t** spp);
result_t	str_append (str_t* dst, str_t* src);
result_t	str_append_char (str_t* dst, int ch);
result_t	str_substr (str_t* dst, str_t* src, size_t start, size_t len);
ssize_t	str_findchar (str_t* src, int ch);
ssize_t	str_findrchar (str_t* src, int ch);
ssize_t	str_findstr (str_t* ref, str_t* pat);
int	str_compare (str_t* ref, str_t* pat);
# endif
