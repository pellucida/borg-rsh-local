//
//  @(#) implement/pointervec.h
//
# if	!defined(IMPLEMENT_VEC_H)
# define	IMPLEMENT_VEC_H

enum    {
	POINTERVEC_SIZE_MIN_SHIFT	=	7,
	POINTERVEC_SIZE_MIN		= (size_t)(1)<<POINTERVEC_SIZE_MIN_SHIFT,
	POINTERVEC_SIZE_STEP_SHIFT	= 8,
	POINTERVEC_SIZE_MAX_SHIFT	= 63,
	POINTERVEC_SIZE_MAX		= (size_t)(1)<<POINTERVEC_SIZE_MAX_SHIFT,
};

enum	{ POINTERVEC_DEFVALUE = 0, };

# define	pointervec_storeflag_(vp)	((vp)->flags.sflag)
# define	pointervec_objflag_(vp)	((vp)->flags.oflag)
# define	pointervec_used_(vp)	((vp)->used)
# define	pointervec_size_(vp)	((vp)->size)
# define	pointervec_vec_(vp)	((vp)->vec)
# define	pointervec_default_(vp)	((vp)->default_value)

# endif
