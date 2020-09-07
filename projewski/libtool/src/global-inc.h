#ifndef _GOC_GLOBAL_INC_H_
#	define _GOC_GLOBAL_INC_H_

#if defined(__STDC__)
# define C89
# if defined(__STDC_VERSION__)
#  define C90
#  if (__STDC_VERSION__ >= 199409L)
#   define C94
#  endif
#  if (__STDC_VERSION__ >= 199901L)
#   define C99
#  endif
#  if (__STDC_VERSION__ >= 201112L)
#   define C11
#  endif
# endif
#endif

#	ifdef _DEBUG
#		define MEMWATCH
#		define MW_STDIO
//#		include <mpatrol.h>
//#	include <malloc.h>
//#		include <mcheck.h>
//#		include <malloc.h>
//#		include "memwatch.h"
#		include <malloc.h>
#	else
#       ifdef C99
#           include <stdlib.h>
#       else
#		    include <malloc.h>
#       endif
#	endif
#endif
