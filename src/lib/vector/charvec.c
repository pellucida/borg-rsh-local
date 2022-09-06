//
// @(#) charvec.c
//

# include	<stdint.h>
# include	<stdlib.h>
# include	<stdbool.h>
# include	<string.h>

# include	"errors/errors.h"
# include	"memory/memory.h"
# include	"container/container.h"
# include	"policy_size/policy_size.h"


# include	"vector/charvec.h"
# include	"vector/implement/charvec.h"



static  inline size_t   policy_size(size_t request) {
        return  POLICY_SIZE_STEP(request,
                CHARVEC_SIZE_MIN, CHARVEC_SIZE_MAX,  CHARVEC_SIZE_STEP_SHIFT);
}

struct	charvec_t {
	cont_flag_t	flags;
	char	default_value;
	size_t		size;
	size_t		used;
	char*	vec;
};

char	charvec_default (const charvec_t* vp) {
	return	charvec_default_(vp);
}
size_t	charvec_used (const charvec_t* vp) {
	return  charvec_used_(vp);
}
size_t	charvec_size (const charvec_t* vp) {
	return  charvec_size_(vp);
}
char*	charvec_vec (const charvec_t* vp) {
	return  charvec_vec_(vp);
}
unsigned	charvec_objflag (const charvec_t* vp) {
	return	charvec_objflag_(vp);
}
unsigned	charvec_storeflag (const charvec_t* vp) {
	return	charvec_storeflag_(vp);
}

static  inline  void charvec_copyin (charvec_t* dst, size_t offset, char const* src, size_t n) {
        char*	vec	= &dst->vec[offset];
	size_t		i	= 0;
	for (; i < n; ++i){
		vec [i]	= src[i];
	}
}

result_t	charvec_grow (charvec_t* vp, size_t reqsize){
	result_t	result	= ok;
	size_t		size	= policy_size (reqsize);
	if (size > charvec_size_(vp)){
		char*	new	= vp->vec; //realloc (vp->vec, sizeof(vp->vec[0])*size);
		result	=	mem_reallocate_vec (&new, size,  sizeof(vp->vec[0]));
		if (result==ok) {
			vp->vec		= new;
			vp->size	= size;
		}
	}
	return	result;
}

result_t	charvec_Create_default (charvec_t** vpp, size_t size, char default_value){
	charvec_t*	vp	= 0;	// calloc (sizeof(*vp), 1);
	result_t	result	= mem_allocate (&vp, sizeof (*vp));
	
	if (result==ok) {
		vp->used	= 0;
		vp->size	= 0;
		vp->vec		= 0;
		vp->default_value	= default_value;
		result	= charvec_grow (vp, size);
		if (result==ok) {
			*vpp	= vp;
		}
		else	{
			mem_free (&vp);
		}
	}
	return	result;
}
result_t	charvec_Create (charvec_t** vpp, size_t size){
	return	charvec_Create_default (vpp, size, CHARVEC_DEFVALUE);
}

result_t	charvec_copy (charvec_t* dst, charvec_t* src){
	result_t	result	= ok;
	size_t		used	= charvec_used_(src);
	if (used > charvec_size_(dst)) {
		result	= charvec_grow (dst, used);
	}
	if (result == ok) {
		charvec_copyin (dst, 0, src->vec, used);
		dst->used	= used;
	} 
	return	result;
}
result_t	charvec_Copy (charvec_t** vpp, charvec_t* src){
	result_t	result	= ok;
	charvec_t*	vp	= *vpp;
	if (vp==0) {
		result	= charvec_Create_default (&vp, charvec_used_(src), charvec_default_(src));
	}
	if (result==ok) {
		result	= charvec_copy (vp, src);
		if (result==ok) {
			*vpp	= vp;
		}
	}
	return	result;
}
result_t	charvec_copy_from (charvec_t* vp, char* src, size_t n){
	result_t	result	= ok;
	if (n > charvec_size_(vp)) {
		result	= charvec_grow (vp, n);
	}
	if (result==ok) {
		charvec_copyin (vp, 0, src, n);
		vp->used	= n;
	}
	return	result;
}
result_t	charvec_Create_from_default (charvec_t** vpp, char* src, size_t n, char default_value){
	charvec_t*	vp	= 0;
	result_t	result	= charvec_Create_default (&vp, n, default_value);
	if (result==ok){
		// copy_from(): size(vp) >= n so will succeed
		result	= charvec_copy_from (vp, src, n);
		*vpp	= vp;
	}
	return	result;
}
result_t	charvec_Create_from (charvec_t** vpp, char* src, size_t n) {
	return	charvec_Create_from_default (vpp, src, n, CHARVEC_DEFVALUE); 
}
result_t	charvec_Delete (charvec_t** vpp){
	result_t	result	= ok;
	if (vpp) {
		charvec_t*	vp	= 0;
		if (vp!=0 && (charvec_storeflag_ (vp) == ST_HEAP)) {
			mem_free (&vp->vec);
			if (charvec_objflag_ (vp) == ST_HEAP) {
				vp->vec	= 0;
				mem_free (&vp);
				*vpp	= 0;
			}
		}
	}
	return	result;
}


//      _get(v,i,&s)    => s == v[i]
//      _insert(v,i,s)  => s == v[i], v[j>i] == v'[j-1] ! insert
//      _replace(v,i,s) => s == v[i], v[j>i] == v'[j]   ! replace
//      _append(v,s)    => s == v[used'], used == used'+1
//      _clear(v,i)     => v[i] == default
//      _remove(v,i)    =  v[j>i] == v'[j+1], used == used'-1


result_t	charvec_append_n (charvec_t* vp, char* src, size_t n){
	result_t	result	= ok;
	size_t		used	= charvec_used_(vp);
	if (used+n < charvec_size_(vp)) {
		result	= charvec_grow (vp, used+n);
	}
	if (result==ok) {
		charvec_copyin (vp, used, src, n);
		vp->used	+= n;
	}
	return	result;
}

result_t	charvec_get (charvec_t* vp, size_t i, char* xp){
	result_t	result	= ok;
	char	x	= charvec_default_(vp);
	if (i < charvec_used_(vp)) {
		x	= vp->vec [i];
	}
	else	{
		result	= ERRORS_STD_EDOM;
	}
	*xp	= x;
	return	result;
}
result_t	charvec_replace (charvec_t* vp, size_t i, char x){
	result_t	result	= ok;
	if (i >= charvec_size_(vp)) {
		result	= charvec_grow (vp, i+1);
	}
	if (result == ok) {
		size_t	used	= charvec_used_(vp);
		if (i >= used) {
			size_t	j	= used;
			for (; j < i; ++j){
				vp->vec [j]	= charvec_default_(vp);
			}
			vp->used	= i+1;
		}
		vp->vec [i]	= x;
	}
	return	result;
}
result_t	charvec_insert (charvec_t* vp, size_t i, char x){
	result_t	result	= ok;
	size_t		used	= charvec_used_(vp);
	size_t		size	= charvec_size_(vp);
	if (i >= size) {
		result	= charvec_grow (vp, i+1);
	}
	if (result == ok) {
		if (i >= used) {
			result	= charvec_replace (vp, i, x);
		}
		else	{
			 if (used == size) {
				result	= charvec_grow (vp, size+1);
			}
			if (result == ok){
				size_t	j	= used-1;
				for (; j >= i; --j) {
					vp->vec [j+1]	= vp->vec [j];
				}
				vp->vec [i]	= x;
				vp->used	= used+1;
			}
		}
	}
	return	result;
}
result_t	charvec_remove  (charvec_t* vp, size_t i){
	result_t	result	= ERRORS_STD_EDOM;
	size_t		used	= charvec_used_(vp);
	if (i < used) {
		size_t	j	= i;
		for(; j < used-1; ++j) {
			vp->vec [j]	= vp->vec[j+1];
		}
		vp->vec [used-1]	= charvec_default_(vp);
		vp->used		= used-1;
		result	= ok;
	}
	return	result;
}
result_t	charvec_clear  (charvec_t* vp, size_t i){
	result_t	result	= ERRORS_STD_EDOM;
	if (i < charvec_used_(vp)) {
		vp->vec [i]	= charvec_default_(vp);
		result		= ok;
	}
	return	result;
}
result_t	charvec_append  (charvec_t* vp, char x){
	result_t	result	= ok;
	size_t		used	= charvec_used_(vp);
	size_t		size	= charvec_size_(vp);
	if (used == size) {
		result	= charvec_grow (vp, used+1);
	}
	if (result==ok) {
		vp->vec[used]	= x;
		vp->used	= used+1;
	}
	return	result;
}
result_t	charvec_remove_last (charvec_t* vp){
	result_t	result	= ERRORS_STD_EDOM;
	size_t	last	= charvec_used_(vp);
	if (last > 0){
		vp->vec [last-1]	= charvec_default_(vp);
		vp->used	= last - 1;
		result	= ok;
	}
	return	result;
}
result_t	charvec_concatenate (charvec_t* dst, charvec_t* src){
	result_t	result	= charvec_append_n (dst, charvec_vec_(src), charvec_used_(src));
	return	result;
}
result_t	charvec_slice (charvec_t* dst, charvec_t* src, size_t start, size_t len){
	result_t	result	= ERRORS_STD_ERANGE;
	size_t	src_used	= charvec_used_(src);
	if (start < src_used) {
		if (start+len > src_used){
			len	= src_used - start;
		}
		result	= charvec_copy_from (dst, charvec_vec_(src)+start, len);
	}
	return	result;
} 

//	compare ref[roff..roff+len-1] with slice[soff..soff+len-1]

int	charvec_compare_offset_n (charvec_t* ref, size_t roff, charvec_t* seg, size_t soff, size_t len){
	int	result	= 0; //equal
	size_t		rused	= charvec_used_(ref);
	size_t		sused	= charvec_used_(seg);
	char*	rvec	= charvec_vec_(ref);
	char*	svec	= charvec_vec_(seg);
	if (roff+len <= rused && soff+len <= sused) {
		size_t	i	= 0;
		size_t	j	= len;
		while (i!=j) {
// C			if (rvec [roff+i] != svec [soff+i]) { 
			result	= ((rvec [roff+i]) - (svec [soff+i]));
			if (result != 0) {
				j	= i;
			}
			else	{
				++i;
			}
		}
	}
	else if (roff > rused) {
			result	= -1;
	}
	else if (soff > sused){
			result	= 1;
	}
	else	{
		if (roff+len > rused) {
			len	= rused-roff;
		}
		if (soff+len > sused) {
			len	= sused-soff;
		}
		result	= charvec_compare_offset_n (ref, roff, seg, soff, len);
		if (result==0) {
// C			result	= rvec[roff+len] - svec[soff+len];
			result	= ((rvec[roff+len]) - (svec[soff+len]));
		}
	}
	return	result;
}
int	charvec_compare_n (charvec_t* ref, charvec_t* seg, size_t len){
	return	charvec_compare_offset_n (ref, 0, seg, 0, len);
}
	
int	charvec_compare (charvec_t* ref, charvec_t* seg) {
	return	charvec_compare_n (ref, seg, (size_t)(-1));
}
	
ssize_t	charvec_find_vec_n (charvec_t* ref, charvec_t* seg, size_t len){
	ssize_t	result	= -1;
	size_t		rused	= charvec_used_(ref);
	size_t		sused	= charvec_used_(seg);
	if (len > sused) {
		len	= sused;
	}
	if (len <= rused) {
		char*	rvec	= charvec_vec_(ref);
		char*	svec	= charvec_vec_(seg);
		size_t	i	= 0;
		size_t	j	= rused - len;
		char	x	= svec [0];
		while (i!=j) {
			if (x == rvec [i] && charvec_compare_offset_n (ref, i+1, seg, 1, len-1)==0) {
				j	= i;
				result	= i;
			}
			else	{
				++i;
			}
		}
	}
	return	result;
}
ssize_t	charvec_find_vec (charvec_t* ref, charvec_t* seg){
	ssize_t	result	= charvec_find_vec_n (ref, seg, charvec_used_(seg));
	return	result;
}
ssize_t	charvec_find (charvec_t* vp, char x){
	ssize_t	result	= -1;
	char*	vec	= charvec_vec_(vp);
	ssize_t	i	= 0;
	ssize_t	j	= charvec_used_(vp);
	while (i!=j) {
// C		if (vec [i] == x) {
		if ((vec [i])==(x)) {
			j	= i;
			result	= i;
		}
		else	{
			++i;
		}
	}
	return	result;
}
ssize_t	charvec_find_last (charvec_t* vp, char x){
	ssize_t	result	= -1;
	char*	vec	= charvec_vec_(vp);
	ssize_t	i	= charvec_used_(vp);
	ssize_t	j	= 0;
	while (i!=j) {
 // C		if (vec [i-1] == x){
		if ((vec [i-1])==(x)) {
			j	= i;
			result	= i-1;
		}
		else	{
			--i;
		}
	}
	return	result;
}
