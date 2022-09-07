//
// @(#) pointervec.c
//

# include	<stdint.h>
# include	<stdlib.h>
# include	<stdbool.h>
# include	<string.h>

# include	"errors/errors.h"
# include	"memory/memory.h"
# include	"container/container.h"
# include	"policy_size/policy_size.h"


# include	"vector/pointervec.h"
# include	"vector/implement/pointervec.h"



static  inline size_t   policy_size(size_t request) {
        return  POLICY_SIZE_STEP(request,
                POINTERVEC_SIZE_MIN, POINTERVEC_SIZE_MAX,  POINTERVEC_SIZE_STEP_SHIFT);
}

struct	pointervec_t {
	cont_flag_t	flags;
	pointer_t*	default_value;
	size_t		size;
	size_t		used;
	pointer_t**	vec;
};

pointer_t*	pointervec_default (const pointervec_t* vp) {
	return	pointervec_default_(vp);
}
size_t	pointervec_used (const pointervec_t* vp) {
	return  pointervec_used_(vp);
}
size_t	pointervec_size (const pointervec_t* vp) {
	return  pointervec_size_(vp);
}
pointer_t**	pointervec_vec (const pointervec_t* vp) {
	return  pointervec_vec_(vp);
}
unsigned	pointervec_objflag (const pointervec_t* vp) {
	return	pointervec_objflag_(vp);
}
unsigned	pointervec_storeflag (const pointervec_t* vp) {
	return	pointervec_storeflag_(vp);
}

static  inline  void pointervec_copyin (pointervec_t* dst, size_t offset, pointer_t* const* src, size_t n) {
        pointer_t**	vec	= &dst->vec[offset];
	size_t		i	= 0;
	for (; i < n; ++i){
		vec [i]	= src[i];
	}
}

result_t	pointervec_grow (pointervec_t* vp, size_t reqsize){
	result_t	result	= ok;
	size_t		size	= policy_size (reqsize);
	if (size > pointervec_size_(vp)){
		pointer_t**	new	= vp->vec; //realloc (vp->vec, sizeof(vp->vec[0])*size);
		result	=	mem_reallocate_vec (&new, size,  sizeof(vp->vec[0]));
		if (result==ok) {
			vp->vec		= new;
			vp->size	= size;
		}
	}
	return	result;
}

result_t	pointervec_Create_default (pointervec_t** vpp, size_t size, pointer_t* default_value){
	pointervec_t*	vp	= 0;	// calloc (sizeof(*vp), 1);
	result_t	result	= mem_allocate (&vp, sizeof (*vp));
	
	if (result==ok) {
		vp->used	= 0;
		vp->size	= 0;
		vp->vec		= 0;
		vp->default_value	= default_value;
		result	= pointervec_grow (vp, size);
		if (result==ok) {
			*vpp	= vp;
		}
		else	{
			mem_free (&vp);
		}
	}
	return	result;
}
result_t	pointervec_Create (pointervec_t** vpp, size_t size){
	return	pointervec_Create_default (vpp, size, POINTERVEC_DEFVALUE);
}

result_t	pointervec_copy (pointervec_t* dst, pointervec_t* src){
	result_t	result	= ok;
	size_t		used	= pointervec_used_(src);
	if (used > pointervec_size_(dst)) {
		result	= pointervec_grow (dst, used);
	}
	if (result == ok) {
		pointervec_copyin (dst, 0, src->vec, used);
		dst->used	= used;
	} 
	return	result;
}
result_t	pointervec_Copy (pointervec_t** vpp, pointervec_t* src){
	result_t	result	= ok;
	pointervec_t*	vp	= *vpp;
	if (vp==0) {
		result	= pointervec_Create_default (&vp, pointervec_used_(src), pointervec_default_(src));
	}
	if (result==ok) {
		result	= pointervec_copy (vp, src);
		if (result==ok) {
			*vpp	= vp;
		}
	}
	return	result;
}
result_t	pointervec_copy_from (pointervec_t* vp, pointer_t** src, size_t n){
	result_t	result	= ok;
	if (n > pointervec_size_(vp)) {
		result	= pointervec_grow (vp, n);
	}
	if (result==ok) {
		pointervec_copyin (vp, 0, src, n);
		vp->used	= n;
	}
	return	result;
}
result_t	pointervec_Create_from_default (pointervec_t** vpp, pointer_t** src, size_t n, pointer_t* default_value){
	pointervec_t*	vp	= 0;
	result_t	result	= pointervec_Create_default (&vp, n, default_value);
	if (result==ok){
		// copy_from(): size(vp) >= n so will succeed
		result	= pointervec_copy_from (vp, src, n);
		*vpp	= vp;
	}
	return	result;
}
result_t	pointervec_Create_from (pointervec_t** vpp, pointer_t** src, size_t n) {
	return	pointervec_Create_from_default (vpp, src, n, POINTERVEC_DEFVALUE); 
}
result_t	pointervec_Delete (pointervec_t** vpp){
	result_t	result	= ok;
	if (vpp) {
		pointervec_t*	vp	= 0;
		if (vp!=0 && (pointervec_storeflag_ (vp) == ST_HEAP)) {
			mem_free (&vp->vec);
			if (pointervec_objflag_ (vp) == ST_HEAP) {
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


result_t	pointervec_append_n (pointervec_t* vp, pointer_t** src, size_t n){
	result_t	result	= ok;
	size_t		used	= pointervec_used_(vp);
	if (used+n >= pointervec_size_(vp)) {
		result	= pointervec_grow (vp, used+n);
	}
	if (result==ok) {
		pointervec_copyin (vp, used, src, n);
		vp->used	+= n;
	}
	return	result;
}

result_t	pointervec_get (pointervec_t* vp, size_t i, pointer_t** xp){
	result_t	result	= ok;
	pointer_t*	x	= pointervec_default_(vp);
	if (i < pointervec_used_(vp)) {
		x	= vp->vec [i];
	}
	else	{
		result	= ERRORS_STD_EDOM;
	}
	*xp	= x;
	return	result;
}
result_t	pointervec_replace (pointervec_t* vp, size_t i, pointer_t* x){
	result_t	result	= ok;
	if (i >= pointervec_size_(vp)) {
		result	= pointervec_grow (vp, i+1);
	}
	if (result == ok) {
		size_t	used	= pointervec_used_(vp);
		if (i >= used) {
			size_t	j	= used;
			for (; j < i; ++j){
				vp->vec [j]	= pointervec_default_(vp);
			}
			vp->used	= i+1;
		}
		vp->vec [i]	= x;
	}
	return	result;
}
result_t	pointervec_insert (pointervec_t* vp, size_t i, pointer_t* x){
	result_t	result	= ok;
	size_t		used	= pointervec_used_(vp);
	size_t		size	= pointervec_size_(vp);
	if (i >= size) {
		result	= pointervec_grow (vp, i+1);
	}
	if (result == ok) {
		if (i >= used) {
			result	= pointervec_replace (vp, i, x);
		}
		else	{
			 if (used == size) {
				result	= pointervec_grow (vp, size+1);
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
result_t	pointervec_remove  (pointervec_t* vp, size_t i){
	result_t	result	= ERRORS_STD_EDOM;
	size_t		used	= pointervec_used_(vp);
	if (i < used) {
		size_t	j	= i;
		for(; j < used-1; ++j) {
			vp->vec [j]	= vp->vec[j+1];
		}
		vp->vec [used-1]	= pointervec_default_(vp);
		vp->used		= used-1;
		result	= ok;
	}
	return	result;
}
result_t	pointervec_clear  (pointervec_t* vp, size_t i){
	result_t	result	= ERRORS_STD_EDOM;
	if (i < pointervec_used_(vp)) {
		vp->vec [i]	= pointervec_default_(vp);
		result		= ok;
	}
	return	result;
}
result_t	pointervec_append  (pointervec_t* vp, pointer_t* x){
	result_t	result	= ok;
	size_t		used	= pointervec_used_(vp);
	size_t		size	= pointervec_size_(vp);
	if (used >= size) {
		result	= pointervec_grow (vp, used+1);
	}
	if (result==ok) {
		vp->vec[used]	= x;
		vp->used	= used+1;
	}
	return	result;
}
result_t	pointervec_remove_last (pointervec_t* vp){
	result_t	result	= ERRORS_STD_EDOM;
	size_t	last	= pointervec_used_(vp);
	if (last > 0){
		vp->vec [last-1]	= pointervec_default_(vp);
		vp->used	= last - 1;
		result	= ok;
	}
	return	result;
}
result_t	pointervec_concatenate (pointervec_t* dst, pointervec_t* src){
	result_t	result	= pointervec_append_n (dst, pointervec_vec_(src), pointervec_used_(src));
	return	result;
}
result_t	pointervec_slice (pointervec_t* dst, pointervec_t* src, size_t start, size_t len){
	result_t	result	= ERRORS_STD_ERANGE;
	size_t	src_used	= pointervec_used_(src);
	if (start < src_used) {
		if (start+len > src_used){
			len	= src_used - start;
		}
		result	= pointervec_copy_from (dst, pointervec_vec_(src)+start, len);
	}
	return	result;
} 

//	compare ref[roff..roff+len-1] with slice[soff..soff+len-1]

int	pointervec_compare_offset_n (pointervec_t* ref, size_t roff, pointervec_t* seg, size_t soff, size_t len){
	int	result	= 0; //equal
	size_t		rused	= pointervec_used_(ref);
	size_t		sused	= pointervec_used_(seg);
	pointer_t**	rvec	= pointervec_vec_(ref);
	pointer_t**	svec	= pointervec_vec_(seg);
	if (roff+len <= rused && soff+len <= sused) {
		size_t	i	= 0;
		size_t	j	= len;
		while (i!=j) {
// C			if (rvec [roff+i] != svec [soff+i]) { 
			result	= (char*)(rvec [roff+i])-(char*)(svec [soff+i]);
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
		result	= pointervec_compare_offset_n (ref, roff, seg, soff, len);
		if (result==0) {
// C			result	= rvec[roff+len] - svec[soff+len];
			result	= (char*)(rvec[roff+len])-(char*)(svec[soff+len]);
		}
	}
	return	result;
}
int	pointervec_compare_n (pointervec_t* ref, pointervec_t* seg, size_t len){
	return	pointervec_compare_offset_n (ref, 0, seg, 0, len);
}
	
int	pointervec_compare (pointervec_t* ref, pointervec_t* seg) {
	return	pointervec_compare_n (ref, seg, (size_t)(-1));
}
	
ssize_t	pointervec_find_vec_n (pointervec_t* ref, pointervec_t* seg, size_t len){
	ssize_t	result	= -1;
	size_t		rused	= pointervec_used_(ref);
	size_t		sused	= pointervec_used_(seg);
	if (len > sused) {
		len	= sused;
	}
	if (len <= rused) {
		pointer_t**	rvec	= pointervec_vec_(ref);
		pointer_t**	svec	= pointervec_vec_(seg);
		size_t	i	= 0;
		size_t	j	= rused - len;
		pointer_t*	x	= svec [0];
		while (i!=j) {
			if (x == rvec [i] && pointervec_compare_offset_n (ref, i+1, seg, 1, len-1)==0) {
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
ssize_t	pointervec_find_vec (pointervec_t* ref, pointervec_t* seg){
	ssize_t	result	= pointervec_find_vec_n (ref, seg, pointervec_used_(seg));
	return	result;
}
ssize_t	pointervec_find (pointervec_t* vp, pointer_t* x){
	ssize_t	result	= -1;
	pointer_t**	vec	= pointervec_vec_(vp);
	ssize_t	i	= 0;
	ssize_t	j	= pointervec_used_(vp);
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
ssize_t	pointervec_find_last (pointervec_t* vp, pointer_t* x){
	ssize_t	result	= -1;
	pointer_t**	vec	= pointervec_vec_(vp);
	ssize_t	i	= pointervec_used_(vp);
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
