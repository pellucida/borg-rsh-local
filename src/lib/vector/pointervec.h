//
//  @(#) pointervec.h
//
# if	!defined(POINTERVEC_H)

# define	POINTERVEC_H	1
# include	<stdint.h>
# include	"errors/errors.h"

typedef struct pointer_t pointer_t;

typedef	struct	pointervec_t	pointervec_t;

pointer_t*	pointervec_default (const pointervec_t* vp);
size_t	pointervec_used (const pointervec_t* vp);
size_t	pointervec_size (const pointervec_t* vp);
pointer_t**	pointervec_vec (const pointervec_t* vp);
unsigned	pointervec_objflag (const pointervec_t* vp);
unsigned	pointervec_storeflag (const pointervec_t* vp);

result_t	pointervec_grow (pointervec_t* vp, size_t reqsize);
result_t	pointervec_Create_default (pointervec_t** vpp, size_t size, pointer_t* default_value);
result_t	pointervec_Create (pointervec_t** vpp, size_t size);
result_t	pointervec_copy (pointervec_t* dst, pointervec_t* src);
result_t	pointervec_Copy (pointervec_t** vpp, pointervec_t* src);
result_t	pointervec_copy_from (pointervec_t* vp, pointer_t** src, size_t n);
result_t	pointervec_Create_from_default (pointervec_t** vpp, pointer_t** src, size_t n, pointer_t* default_value);
result_t	pointervec_Create_from (pointervec_t** vpp, pointer_t** src, size_t n) ;
result_t	pointervec_Delete (pointervec_t** vpp);
result_t	pointervec_append_n (pointervec_t* vp, pointer_t** src, size_t n);
result_t	pointervec_get (pointervec_t* vp, size_t i, pointer_t** xp);
result_t	pointervec_replace (pointervec_t* vp, size_t i, pointer_t* x);
result_t	pointervec_insert (pointervec_t* vp, size_t i, pointer_t* x);
result_t	pointervec_remove  (pointervec_t* vp, size_t i);
result_t	pointervec_clear  (pointervec_t* vp, size_t i);
result_t	pointervec_append  (pointervec_t* vp, pointer_t* x);
ssize_t		pointervec_find (pointervec_t* vp, pointer_t* x);
ssize_t		pointervec_find_last (pointervec_t* vp, pointer_t* x);
int		pointervec_compare_offset_n (pointervec_t* ref, size_t roff, pointervec_t* seg, size_t soff, size_t len);
int		pointervec_compare_n (pointervec_t* ref, pointervec_t* seg, size_t len);
int		pointervec_compare (pointervec_t* ref, pointervec_t* seg);
ssize_t		pointervec_find_vec_n (pointervec_t* vp, pointervec_t* seg, size_t len);
ssize_t		pointervec_find_vec (pointervec_t* vp, pointervec_t* seg);
result_t	pointervec_remove_last  (pointervec_t* vp);
result_t	pointervec_concatenate  (pointervec_t* dst, pointervec_t* src);
result_t	pointervec_slice  (pointervec_t* dst, pointervec_t* src, size_t start, size_t len);

# endif
