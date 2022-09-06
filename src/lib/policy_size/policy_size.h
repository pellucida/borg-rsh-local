/*
// @(#) policy_size.h
*/
# if	!defined (POLICY_SIZE_H)
# define	POLICY_SIZE_H

static	inline	size_t	POLICY_SIZE_STEP (size_t rq, size_t min, size_t max, size_t step_shift){
	size_t	result	= rq;
	if (rq <= min) {
		result	= min;
	}
	else if (rq <= max) {
		size_t	step_mask	= (~(size_t)(0))<<step_shift;
		/*Note: ~step_mask == step_size - 1 */
		result	= (rq + ~step_mask) & step_mask;
	}
	return	result;
}

static	inline	size_t	POLICY_SIZE_DOUBLE (size_t rq,size_t min,size_t max) {
	size_t	result	= rq;						
	if (rq <= min) {						
		result	= min;						
	}								
	else if (rq <= max) {						
		result	= 2*rq;					
	}								
	return	result;
}

static	inline	size_t	POLICY_SIZE_NULL( size_t rq){
	return	rq;
}

# endif
