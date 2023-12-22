#ifndef _GOC_GLOBAL_INC_H_
#	define _GOC_GLOBAL_INC_H_
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
#		ifdef __MACH__
#			include <stdlib.h>
#		else
#			include <malloc.h>
#		endif
#	endif
#endif
