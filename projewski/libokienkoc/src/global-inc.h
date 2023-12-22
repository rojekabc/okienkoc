#ifndef _GOC_GLOBAL_INC_H_
#	define _GOC_GLOBAL_INC_H_
#	ifdef _DEBUG
#		ifdef __MACH__
#			include <stdlib.h>
#		else
#			include <malloc.h>
#		endif
#	else
#		ifdef __MACH__
#			include <stdlib.h>
#		else
#			include <malloc.h>
#		endif
#	endif
#endif
