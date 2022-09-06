//
//
//
# if	!defined(VECTOR_STRVEC_H)

# define	VECTOR_STRVEC_H
# include	<stdint.h>
# include	"errors/errors.h"

typedef	struct	strvec_t	strvec_t;

size_t	strvec_used (const strvec_t* vp);
size_t	strvec_size (const strvec_t* vp);
str_t**	strvec_vec (const strvec_t* vp);
result_t	strvec_Create (strvec_t** vpp, size_t size);
result_t	strvec_copy (strvec_t* dst, strvec_t* src);
result_t	strvec_Copy (strvec_t** vpp, strvec_t* src);
result_t	strvec_Delete (strvec_t** vpp);
result_t	strvec_append_n (strvec_t* vp, str_t** src, size_t n);
result_t	strvec_get (strvec_t* vp, size_t i, str_t** xp);
result_t	strvec_replace (strvec_t* vp, size_t i, str_t* x);
result_t	strvec_insert (strvec_t* vp, size_t i, str_t* x);
result_t	strvec_remove  (strvec_t* vp, size_t i);
result_t	strvec_clear  (strvec_t* vp, size_t i);
result_t	strvec_append  (strvec_t* vp, str_t* x);
result_t	strvec_append_cstring  (strvec_t* vp, char* cstr);
result_t	strvec_remove_last (strvec_t* vp);
result_t	strvec_concatenate (strvec_t* dst, strvec_t* src);
result_t	strvec_slice (strvec_t* dst, strvec_t* src, size_t start, size_t len);
ssize_t	strvec_find (strvec_t* vp, str_t* x);
ssize_t	strvec_find_last (strvec_t* vp, str_t* x);
result_t	strvec_argc_argv (strvec_t* vp, int* argcp, char*** argvp);

# endif
