/*
// @(#) memory/memory.h
*/

# if	!defined(MEMORY_MEMORY_H)
// # define	MEMORY_MEMORY_H

# include	<stdlib.h>
# include	"errors/errors.h"

# define	HAVE_MALLOC_USABLE_SIZE

result_t	mem_allocate (void* mp, size_t request);
result_t	mem_allocate_vec (void* vp, size_t nelem, size_t eltsize);
result_t	mem_reallocate (void* mp, size_t request);
result_t	mem_reallocate_vec (void* vp, size_t nelem, size_t eltsize);
result_t	mem_free (void* mp);
# endif
