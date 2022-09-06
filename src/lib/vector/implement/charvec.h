//
//  @(#) implement/charvec.h
//
# if	!defined(IMPLEMENT_VEC_H)
# define	IMPLEMENT_VEC_H

enum    {
	CHARVEC_SIZE_MIN_SHIFT	=	7,
	CHARVEC_SIZE_MIN		= (size_t)(1)<<CHARVEC_SIZE_MIN_SHIFT,
	CHARVEC_SIZE_STEP_SHIFT	= 8,
	CHARVEC_SIZE_MAX_SHIFT	= 63,
	CHARVEC_SIZE_MAX		= (size_t)(1)<<CHARVEC_SIZE_MAX_SHIFT,
};

enum	{ CHARVEC_DEFVALUE = 0, };

# define	charvec_storeflag_(vp)	((vp)->flags.sflag)
# define	charvec_objflag_(vp)	((vp)->flags.oflag)
# define	charvec_used_(vp)	((vp)->used)
# define	charvec_size_(vp)	((vp)->size)
# define	charvec_vec_(vp)	((vp)->vec)
# define	charvec_default_(vp)	((vp)->default_value)

# endif
