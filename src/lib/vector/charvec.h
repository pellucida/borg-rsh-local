//
//  @(#) charvec.h
//
# if	!defined(CHARVEC_H)

# define	CHARVEC_H	1
# include	<stdint.h>
# include	"errors/errors.h"

typedef	struct	charvec_t	charvec_t;

char	charvec_default (const charvec_t* vp);
size_t	charvec_used (const charvec_t* vp);
size_t	charvec_size (const charvec_t* vp);
char*	charvec_vec (const charvec_t* vp);
unsigned	charvec_objflag (const charvec_t* vp);
unsigned	charvec_storeflag (const charvec_t* vp);

result_t	charvec_grow (charvec_t* vp, size_t reqsize);
result_t	charvec_Create_default (charvec_t** vpp, size_t size, char default_value);
result_t	charvec_Create (charvec_t** vpp, size_t size);
result_t	charvec_copy (charvec_t* dst, charvec_t* src);
result_t	charvec_Copy (charvec_t** vpp, charvec_t* src);
result_t	charvec_copy_from (charvec_t* vp, char* src, size_t n);
result_t	charvec_Create_from_default (charvec_t** vpp, char* src, size_t n, char default_value);
result_t	charvec_Create_from (charvec_t** vpp, char* src, size_t n) ;
result_t	charvec_Delete (charvec_t** vpp);
result_t	charvec_append_n (charvec_t* vp, char* src, size_t n);
result_t	charvec_get (charvec_t* vp, size_t i, char* xp);
result_t	charvec_replace (charvec_t* vp, size_t i, char x);
result_t	charvec_insert (charvec_t* vp, size_t i, char x);
result_t	charvec_remove  (charvec_t* vp, size_t i);
result_t	charvec_clear  (charvec_t* vp, size_t i);
result_t	charvec_append  (charvec_t* vp, char x);
ssize_t		charvec_find (charvec_t* vp, char x);
ssize_t		charvec_find_last (charvec_t* vp, char x);
int		charvec_compare_offset_n (charvec_t* ref, size_t roff, charvec_t* seg, size_t soff, size_t len);
int		charvec_compare_n (charvec_t* ref, charvec_t* seg, size_t len);
int		charvec_compare (charvec_t* ref, charvec_t* seg);
ssize_t		charvec_find_vec_n (charvec_t* vp, charvec_t* seg, size_t len);
ssize_t		charvec_find_vec (charvec_t* vp, charvec_t* seg);
result_t	charvec_remove_last  (charvec_t* vp);
result_t	charvec_concatenate  (charvec_t* dst, charvec_t* src);
result_t	charvec_slice  (charvec_t* dst, charvec_t* src, size_t start, size_t len);

# endif
