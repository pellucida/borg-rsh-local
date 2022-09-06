/*
// @(#) generic/value.h
*/

# if	!defined(CONTAINER_VALUE_H)
# define	CONTAINERGENERIC_VALUE_H

# include	<stdint.h>
# include	"errors/errors.h"

typedef	uint64_t	hash_t;

union	value_t {
	void*		ptr;
	uint64_t	word;
};
# define	value_to_ptr(x)	(x).ptr
# define	ptr_to_value(x)	(value_t){.ptr = (x),}	
# define	value_to_word(x)	(x).word
# define	word_to_value(x)	(value_t){.word = (x),}	

typedef	union	value_t	value_t;

typedef result_t        (*generic_copy_ctor_t) (value_t* copyp, value_t src);
typedef result_t        (*generic_dtor_t) (value_t* copyp);
typedef int             (*generic_compare_t)(value_t key1, value_t key2);
typedef hash_t          (*generic_hash_t) (value_t key);

# endif
