
# include	<string.h>
# include	"vector/charvec.h"

# include	"string/str.h"


// Basically casting str->u8vec and back
static	inline	charvec_t*	str_charvec (const str_t* s){ return (charvec_t*)(s);}
static	inline	str_t*		charvec_str (charvec_t* v){ return (str_t*)(v);}

typedef	struct	str_t	str_t;

char*	const str_cstring (const str_t* str) {
        char*	vec	= charvec_vec (str_charvec (str));
	return	(char* const)vec;
}
//
// We want to retain the terminating '\0' so we must
// allocate at least length+1
// Hence length == used - 1
//
size_t	str_length (str_t* str) {
        return	charvec_used (str_charvec(str))-1;
}
size_t	str_size (str_t* str) {
        return	charvec_size (str_charvec(str));
}
result_t	str_Create (str_t** spp, size_t size) {
	charvec_t*	vp	= 0;
	result_t	result	= charvec_Create (&vp, size+1);
	if (result==ok){
		charvec_append (vp, '\0');	// NUL terminate
		*spp	= charvec_str (vp);
	}
	return	result;
}
result_t	str_Create_cstring (str_t** spp, const char* cstr) {
	charvec_t*	vp	= 0;
	size_t		len	= strlen (cstr);
	result_t	result	= charvec_Create_from (&vp, (char*)cstr, len+1);
	if (result==ok) {
		*spp	= charvec_str (vp);
	}
	return	result;
} 
result_t	str_Copy (str_t** spp, const str_t* src) {
	charvec_t*	vp	= 0;
	result_t	result	= charvec_Copy (&vp, str_charvec (src));
	if (result==ok){
		*spp	= charvec_str (vp);
	}
	return	result;
}
result_t	str_copy (str_t* sp, const str_t* src) {
	result_t	result	= charvec_copy (str_charvec (sp), str_charvec (src));
	return	result;
}
result_t	str_copy_cstring (str_t* sp, const char* cstr) {
	size_t		len	= strlen (cstr);
	result_t	result	= charvec_copy_from (str_charvec (sp), (char*)cstr, len+1);
	return	result;
}

result_t	str_Delete (str_t** spp) {
	charvec_t**	vpp	= (charvec_t**)spp;
	result_t	result	= charvec_Delete (vpp);
	return	result;
}
result_t	str_append (str_t* dst, str_t* src){
	charvec_t*	ud	= str_charvec (dst);
	charvec_t*	us	= str_charvec (src);
	// remove terminal \0 from dst
	result_t	result	= charvec_remove_last (ud);
	if (result==ok){
		result	= charvec_concatenate (ud, us);
	}
	return	result;
}
result_t	str_append_char (str_t* sp, int ch){
	charvec_t*	cv	= str_charvec(sp);
	result_t	result	= charvec_remove_last (cv);
	if (result==ok)
		result	= charvec_append (cv, (char)(ch));
	if (result==ok)
		result	= charvec_append (cv, '\0');
	return	result;
}
result_t	str_substr (str_t* dst, str_t* src, size_t start, size_t len){
	charvec_t*	ud	= str_charvec (dst);
	charvec_t*	us	= str_charvec (src);
	result_t	result	= charvec_slice (ud, us, start, len);
	if (result == ok) {
		if ((start + len) < charvec_used (us)){
			result	= charvec_append (ud, '\0');
		}
	}
	return	result;
}
ssize_t	str_findchar (str_t* src, int ch){
	charvec_t*	us	= str_charvec (src);
	ssize_t	result	= charvec_find (us, (char)ch);
	return	result;
}
ssize_t	str_findrchar (str_t* src, int ch){
	charvec_t*	us	= str_charvec (src);
	ssize_t	result	= charvec_find_last (us, (char)ch);
	return	result;
}
ssize_t	str_findstr (str_t* ref, str_t* pat){
	charvec_t*	ur	= str_charvec (ref);
	charvec_t*	up	= str_charvec (pat);
	ssize_t	result	= charvec_find_vec_n (ur, up, str_length(pat));
	return	result;
}
int	str_compare (str_t* ref, str_t* pat){
	charvec_t*	ur	= str_charvec (ref);
	charvec_t*	up	= str_charvec (pat);
	int	result	= charvec_compare_n (ur, up, str_length(pat));
	return	result;
}
