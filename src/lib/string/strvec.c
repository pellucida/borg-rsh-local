//
// @(#) strvec.c
//

# include	<stdint.h>
# include	<stdlib.h>
# include	<stdbool.h>
# include	<string.h>

# include	"errors/errors.h"
# include	"memory/memory.h"

# include	"vector/pointervec.h"
# include	"string/str.h"
# include	"string/strvec.h"



typedef	struct	strvec_t strvec_t;

// Basically casting
//	strvec_t*	<-> pointervec_t*
//	str_t*		<-> pointer_t
//	str_t*[]	<-> pointer_t* # ref_
//
static	inline	pointervec_t*	strvec_pointervec (const strvec_t* s) {return (pointervec_t*)(s);}
static	inline	strvec_t*	pointervec_strvec (pointervec_t* u) {return (strvec_t*)(u);}
static	inline	pointer_t**	ref_str_pointer (str_t** s) {return (pointer_t**)(s);}
static	inline	pointer_t*	str_pointer (str_t* s) {return (pointer_t*)(s);}
static	inline	str_t**		ref_pointer_str (pointer_t** u) {return (str_t**)(u);}
static	inline	str_t*		pointer_str (pointer_t* u) {return (str_t*)(u);}

size_t	strvec_used (const strvec_t* vp) {
	return  pointervec_used (strvec_pointervec (vp));
}
size_t	strvec_size (const strvec_t* vp) {
	return  pointervec_size (strvec_pointervec (vp));
}
str_t**	strvec_vec (const strvec_t* vp) {
	return  ref_pointer_str (pointervec_vec (strvec_pointervec(vp)));
}

result_t	strvec_Create (strvec_t** vpp, size_t size){
	pointervec_t*	v	= 0;
	result_t	result	= pointervec_Create (&v, size);
	if (result==ok)
		*vpp	= pointervec_strvec(v);
	return	result;
}
result_t	strvec_copy (strvec_t* dst, strvec_t* src){
	result_t	result	= pointervec_copy (strvec_pointervec(dst), strvec_pointervec(src));
	return	result;
}
result_t	strvec_Copy (strvec_t** vpp, strvec_t* src){
	pointervec_t*	v	= 0;
	result_t	result	= pointervec_Copy (&v, strvec_pointervec (src));
	if (result==ok){
		*vpp	= pointervec_strvec(v);
	}
	return	result;
}
result_t	strvec_Delete (strvec_t** vpp){
	result_t	result	= ok;
	if (vpp) {
		pointervec_t*	vp	= strvec_pointervec (*vpp);
		result	= pointervec_Delete (&vp);
		if (result==ok)
			*vpp	= 0;
	}
	return	result;
}


//      _get(v,i,&s)    => s == v[i]
//      _insert(v,i,s)  => s == v[i], v[j>i] == v'[j-1] ! insert
//      _replace(v,i,s) => s == v[i], v[j>i] == v'[j]   ! replace
//      _append(v,s)    => s == v[used'], used == used'+1
//      _clear(v,i)     => v[i] == default
//      _remove(v,i)    =  v[j>i] == v'[j+1], used == used'-1
// ---
result_t	strvec_append_n (strvec_t* vp, str_t** src, size_t n){
	result_t	result	= pointervec_append_n (strvec_pointervec(vp), ref_str_pointer(src), n);
	return	result;
}

result_t	strvec_get (strvec_t* vp, size_t i, str_t** xp){
	result_t	result	= pointervec_get (strvec_pointervec(vp), i, ref_str_pointer(xp));
	return	result;
}
result_t	strvec_replace (strvec_t* vp, size_t i, str_t* x){
	result_t	result	= pointervec_replace (strvec_pointervec(vp), i, str_pointer (x)); 
	return	result;
}
result_t	strvec_insert (strvec_t* vp, size_t i, str_t* x){
	result_t	result	= pointervec_insert (strvec_pointervec(vp), i, str_pointer(x));
	return	result;
}
result_t	strvec_remove  (strvec_t* vp, size_t i){
	result_t	result	= pointervec_remove (strvec_pointervec(vp), i);
	return	result;
}
result_t	strvec_clear  (strvec_t* vp, size_t i){
	result_t	result	= pointervec_clear (strvec_pointervec(vp), i);
	return	result;
}
result_t	strvec_append  (strvec_t* vp, str_t* x){
	result_t	result	= pointervec_append (strvec_pointervec(vp), str_pointer(x));
	return	result;
}
result_t	strvec_append_cstring  (strvec_t* vp, char* cstr){
	str_t*	str	= 0;
	result_t	result	= str_Create_cstring (&str, cstr);
	if (result==ok){
		result	= strvec_append (vp, str);
		if (result != ok)
			str_Delete (&str);
	}
	return	result;
}
result_t	strvec_remove_last (strvec_t* vp){
	result_t	result	= pointervec_remove_last (strvec_pointervec(vp));
	return	result;
}
result_t	strvec_concatenate (strvec_t* dst, strvec_t* src){
	result_t	result	= pointervec_concatenate (strvec_pointervec (dst), strvec_pointervec (src));
	return	result;
}
result_t	strvec_slice (strvec_t* dst, strvec_t* src, size_t start, size_t len){
	result_t	result	= pointervec_slice (strvec_pointervec (dst), strvec_pointervec (src), start, len);
	return	result;
} 
ssize_t	strvec_find (strvec_t* vp, str_t* x){
	ssize_t	result	= -1;
	str_t**	vec	= strvec_vec (vp);
	ssize_t	i	= 0;
	ssize_t	j	= strvec_used (vp);
	while (i!=j) {
		if (str_compare(vec [i],x)==0) {
			j	= i;
			result	= i;
		}
		else	{
			++i;
		}
	}
	return	result;
}
ssize_t	strvec_find_last (strvec_t* vp, str_t* x){
	ssize_t	result	= -1;
	str_t**	vec	= strvec_vec (vp);
	ssize_t	i	= strvec_used (vp);
	ssize_t	j	= 0;
	while (i!=j) {
		if (str_compare(vec [i-1],x)==0) {
			j	= i;
			result	= i-1;
		}
		else	{
			--i;
		}
	}
	return	result;
}

result_t	strvec_argc_argv (strvec_t* vp, int* argcp, char*** argvp){
	size_t	used	= strvec_used (vp);
	char**	argv	= 0;
	result_t	result	= mem_allocate_vec (&argv, (used+1),sizeof (argv[0]));
	if (result==ok) {
		size_t	i	= 0;
		for (i=0, result=ok; result==ok && i < used; ++i) {
			str_t*	arg	= 0;
			result	= strvec_get (vp, i, &arg);
			if (result==ok){
				argv [i]	= str_cstring (arg);
			}
		}
		if (result==ok) {
			argv [used]	= 0;
			*argvp	= argv;
			if (argcp) {
				*argcp	= used;
			}
		}
		else	{
			mem_free (&argv);
		}
	}
	return	result;
}
