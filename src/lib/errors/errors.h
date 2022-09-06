/*
// @(#) 
*/
# if	!defined( ERRORS_H)
# define	ERRORS_H

// # include	<errno.h>
# include	<stdint.h>
# include	<stdlib.h>
# include	<errno.h>

# define	MASK_CLRBITS(n)	((~(uint64_t)(0))<<(n))	
# define	MASK_BITS(n)	(~(MASK_CLRBITS(n)))	
# define	MASK_FLD(n,off)	(((uint64_t)(MASK_BITS(n)))<<(off))
# define	MASK_CLRFLD(n,off)	(~MASK_FLD(n,off))
# define	BITS_SETFLD(x,n,off)	(((uint64_t)(x) & MASK_BITS(n))<<(off))
# define	BITS_CLRFLD(x,n,off)	((uint64_t)(x) & MASK_CLRFLD(n,off))

# if	defined( DEBUGGING)
extern	void	str_debug_(char*,char*, int, void*);
# define	str_debug(s)	\
	str_debug_((char*)__FILE__,(char*)__FUNCTION__,__LINE__, value_to_ptr(s))
# else
# define	str_debug(s)	

# endif

enum	{ // err:1, severity:3. reserved:28, facility:16, errno:16
	bits_errno	= 16,
	off_errno	= 0,
	mask_errno	= MASK_BITS (bits_errno),

	bits_facility	= 16,
	off_facility	= off_errno+bits_errno,

	bits_reserved	= 28,
	off_reserved	= off_facility+bits_facility,

	bits_severity	= 3,
	off_severity	= off_reserved+bits_reserved,

	bits_err	= 1,
	off_err		= off_severity+bits_severity,
};

typedef	int64_t	result_t;

enum	{
	ok	= 0,
	err	= -1,
};
enum	{
	ERROR_STD_SEVERITY	= -1,
	ERROR_STD_FACILITY	= -1,
	ERROR_STD_ERRNO		= -1,
};

# define	ERRORS_STD_ERRNO(erno)	((MASK_CLRBITS(bits_errno))|((erno) & MASK_BITS(bits_errno)))
		
# define	ERRORS_STD_ENOMEM	ERRORS_STD_ERRNO(ENOMEM)	
# define	ERRORS_STD_ENOENT	ERRORS_STD_ERRNO(ENOENT)	
# define	ERRORS_STD_ERANGE	ERRORS_STD_ERRNO(ERANGE)	
# define	ERRORS_STD_EDOM		ERRORS_STD_ERRNO(EDOM)	

# define	ERRORS_STD_EPERM	ERRORS_STD_ERRNO(EPERM)
# define	ERRORS_STD_ESRCH	ERRORS_STD_ERRNO(ESRCH)
# define	ERRORS_STD_EINTR	ERRORS_STD_ERRNO(EINTR)
# define	ERRORS_STD_EIO		ERRORS_STD_ERRNO(EIO)
# define	ERRORS_STD_ENXIO	ERRORS_STD_ERRNO(ENXIO)
# define	ERRORS_STD_E2BIG	ERRORS_STD_ERRNO(E2BIG)
# define	ERRORS_STD_ENOEXEC	ERRORS_STD_ERRNO(ENOEXEC)
# define	ERRORS_STD_EBADF	ERRORS_STD_ERRNO(EBADF)
# define	ERRORS_STD_ECHILD	ERRORS_STD_ERRNO(ECHILD)
# define	ERRORS_STD_EAGAIN	ERRORS_STD_ERRNO(EAGAIN)
# define	ERRORS_STD_EACCES	ERRORS_STD_ERRNO(EACCES)
# define	ERRORS_STD_EFAULT	ERRORS_STD_ERRNO(EFAULT)
# define	ERRORS_STD_ENOTBLK	ERRORS_STD_ERRNO(ENOTBLK)
# define	ERRORS_STD_EBUSY	ERRORS_STD_ERRNO(EBUSY)
# define	ERRORS_STD_EEXIST	ERRORS_STD_ERRNO(EEXIST)
# define	ERRORS_STD_EXDEV	ERRORS_STD_ERRNO(EXDEV)
# define	ERRORS_STD_ENODEV	ERRORS_STD_ERRNO(ENODEV)
# define	ERRORS_STD_ENOTDIR	ERRORS_STD_ERRNO(ENOTDIR)
# define	ERRORS_STD_EISDIR	ERRORS_STD_ERRNO(EISDIR)
# define	ERRORS_STD_EINVAL	ERRORS_STD_ERRNO(EINVAL)
# define	ERRORS_STD_ENFILE	ERRORS_STD_ERRNO(ENFILE)
# define	ERRORS_STD_EMFILE	ERRORS_STD_ERRNO(EMFILE)
# define	ERRORS_STD_ENOTTY	ERRORS_STD_ERRNO(ENOTTY)
# define	ERRORS_STD_ETXTBSY	ERRORS_STD_ERRNO(ETXTBSY)
# define	ERRORS_STD_EFBIG	ERRORS_STD_ERRNO(EFBIG)
# define	ERRORS_STD_ENOSPC	ERRORS_STD_ERRNO(ENOSPC)
# define	ERRORS_STD_ESPIPE	ERRORS_STD_ERRNO(ESPIPE)
# define	ERRORS_STD_EROFS	ERRORS_STD_ERRNO(EROFS)
# define	ERRORS_STD_EMLINK	ERRORS_STD_ERRNO(EMLINK)
# define	ERRORS_STD_EPIPE	ERRORS_STD_ERRNO(EPIPE)

static	inline	result_t	error_field_set (result_t r, int x, int width, int offset) {
	result_t	result	= BITS_CLRFLD(r,width,offset) | BITS_SETFLD(x,width,offset);
	return	result;
}
static	inline	result_t	error_field_get (result_t r, int width, int offset) {
	result_t	result	= (r >> offset)&MASK_BITS(width);
	return	result;
}

static	inline	result_t	error_facility_set (result_t r, int fac) {
	return	error_field_set (r, fac, bits_facility, off_facility);
}
static	inline	result_t	error_severity_set (result_t r, int sev) {
	return	error_field_set (r, sev, bits_severity, off_severity);
}
static	inline	result_t	error_reserved_set (result_t r, int rs) {
	return	error_field_set (r, rs, bits_reserved, off_reserved);
}
static	inline	result_t	error_err_set (result_t r, int t) {
	return	error_field_set (r, t, bits_err, off_err);
}
static	inline	result_t	error_errno_set (result_t r, int t) {
	return	error_field_set (r, t, bits_errno, off_errno);
}
static	inline result_t error_encode (int severity, int facility, int errno_){
	uint64_t	result	= 
		 error_err_set (0, 1)
		| error_severity_set (0, severity)
		| error_reserved_set (0, -1)
		| error_facility_set (0, facility)
		| error_errno_set (0, errno_);

	return	result;
}
static	inline result_t	error_std_errno (int errno_) {
	return	error_encode (ERROR_STD_SEVERITY, ERROR_STD_FACILITY, errno_);
}
static	const result_t	error_std_err	= err;
static	const result_t	error_std_ok	= ok;

static	inline	int	error_severity (result_t r){
	return	error_field_get (r, bits_severity, off_severity);
}
static	inline	int	error_facility (result_t r){
	return	error_field_get (r, bits_facility, off_facility);
}
static	inline	int	error_errno (result_t r){
	return	error_field_get (r, bits_errno, off_errno);
}

typedef	ssize_t	(*error_strerror_r_t) (result_t r, char* buf, size_t bufsize);
typedef	char*	(*error_strerror_t) (result_t r);

struct	error_messages {
	char*			facility_name;
	error_strerror_r_t	strerror_r;
	error_strerror_t	strerror;
};

typedef	struct	error_messages	error_messages_t;

int	error_messages_register (error_messages_t em);

char*	error_strerror (result_t result);
ssize_t	error_strerror_r (result_t result, char* msg, size_t msgsize);
char*	error_facility_name (result_t result);
int	error_lookup_facility (char* name);

# endif
