/*
// @(#) memory.c - wrappers around malloc/free
*/

# include	<stdlib.h>
# include	<malloc.h>
# include	<string.h>

# include	"errors/errors.h"
# include	"memory/memory.h"

// Nasty casts. T* ptr, passing pointer to ptr (T**)
// conforms to void* in function call.
// T*, &T* [T**] call 
//	-> void* [T**] callee
//		 -> void** [T**], *void** [T*] = new 

// -O<n> n>0 doesn't generate any code for these.
static inline	const void**	const_ref_ref_void (const void* p){return (const void**)(p);}
static inline	void**	ref_ref_void (const void* p){return (void**)(p);}

result_t	mem_allocate (void* mp, size_t request){
	result_t	result	= ERRORS_STD_EINVAL;
	if (mp) {
		void*	m	= calloc (1, request);
		if (m) {
			*const_ref_ref_void(mp)	= m;
			result	= ok;
		}
		else	result	= ERRORS_STD_ENOMEM;
	}
	return	result;
}
result_t	mem_allocate_vec (void* vp, size_t nelem, size_t eltsize){
	result_t	result	= ERRORS_STD_EINVAL;
	if (vp) {
		size_t	request	= nelem * eltsize;
		void*	m	= 0;
		result	= mem_allocate (&m, request);
		if (result==ok) {
			*const_ref_ref_void(vp)	= m;
		}
	}
	return	result;
}
	
result_t	mem_reallocate (void* mp, size_t request){
	result_t	result	= ERRORS_STD_EINVAL;
	if (mp) {
		void*	m	= *ref_ref_void(mp);
# if	defined( HAVE_MALLOC_USABLE_SIZE)
		size_t	oldsize	= malloc_usable_size (m);
# endif
		void*	new	= realloc (m, request);
		if (new){
# if	defined( HAVE_MALLOC_USABLE_SIZE)
			size_t	newsize	= malloc_usable_size (new);
			if (newsize > oldsize)
				memset (new+oldsize, 0, newsize-oldsize);
# endif
			*const_ref_ref_void(mp)	= new;
			result	= ok;
		}
		else	result	= ERRORS_STD_ERRNO(errno);
	}
	return	result;
}
result_t	mem_reallocate_vec (void* vp, size_t nelem, size_t eltsize){
	result_t	result	= ERRORS_STD_EINVAL;
	if (vp) {
		size_t	request	= nelem * eltsize;
		void*	m	= *ref_ref_void(vp);
		result	= mem_reallocate (&m, request);
		if (result==ok) { // *vp now invalid 
			*const_ref_ref_void(vp)	= m;
		}
	}
	return	result;
}
// mem_free (0) and void* p=0, mem_free (&p) aren't errors
result_t	mem_free (void* mp){
	result_t	result	= ok;
	if (mp) {
		void*	m	= *ref_ref_void(mp);
		if (m) {
			free (m);
			*const_ref_ref_void(mp)	= 0;
		}
	}
	return	result;
}
