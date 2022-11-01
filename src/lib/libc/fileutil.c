

result_t	str_basename (str_t* path, str_t* base) {
	result_t	result	= err;
	size_t	len	= str_length (path);
	ssize_t	pos	= str_findrchar (path, '/');
	if (pos >= 0 && pos+1 <= len) {
		result	= str_substr (base, path, pos+1, len-pos); 
	}
	else	{
		result	= str_copy (base, path);
	}
	return	result;
}
result_t	str_dirname (str_t* path, str_t* dir) {
	result_t	result	= err;
	size_t	len	= str_length (path);
	ssize_t	pos	= str_findrchar (path, '/');
	if (pos >= 0) {
		result	= str_substr (base, path, 0, pos-1); 
	}
	return	result;
}
	
	
