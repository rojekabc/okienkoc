#ifndef _GOC_MALLOC_H_
#	define _GOC_MALLOC_H_
#	include <malloc.h>
#	include <string.h>

#	define ALLOC( _type_, _name_ ) \
		_type_* _name_ = malloc(sizeof(_type_));\
		memset( _name_, 0, sizeof(_type_))

#endif
