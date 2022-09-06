/*
//  @(#) container.h
*/

# if	!defined(CONTAINER_H)
# define	CONTAINER_H	

# include	"container/value.h"

// Container flags:
//	copy_in		- make a copy when inserting item
//	copy_out	- make a copy when extracting item
//	enumerate_default	- (vector) _apply() also passes default elt
//	map_values	- if (maptab) is a (map) then true else (table)
//	keep_hash	- store hash with item in hashtab
//	oflag		- allocation type for object descriptor viz heap, stack, static or const
//	sflag		- allocation type for object storage viz heap, stack, static or const
//
# include	<stdint.h>
typedef	struct	{
	uint64_t	copy_in:1, copy_out:1, enumerate_default:1, map_values:1, keep_hash:1, oflag:2, sflag:2;
}	cont_flag_t;

enum    store_type      {
        ST_HEAP = 0,
        ST_AUTO = 1,
        ST_CONST= 2,
        ST_STATIC = 3,
};


// Mask in Copy use src flags only
# define	CNTMSK_NONE	(cont_flag_t){0,}
# define	CNTMSK_COPYIN	(cont_flag_t){.copy_in = true,}
# define	CNTMSK_COPYOUT	(cont_flag_t){.copy_out = true,}
# define	CNTMSK_COPYBOTH	(cont_flag_t){.copy_in = true, .copy_out = true,}

struct	cont_op_t	{
	
        generic_hash_t          hash;
        generic_compare_t       compare;
        generic_copy_ctor_t     copy_ctor;
        generic_dtor_t          dtor;
	generic_copy_ctor_t	value_copy_ctor;
        generic_dtor_t          value_dtor;
};
typedef	struct	cont_op_t	cont_op_t;

result_t	cont_op_Create (cont_op_t* opp);

# endif
