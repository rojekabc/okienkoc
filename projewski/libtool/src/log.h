#ifndef _GOC_LOG_H_
#	define _GOC_LOG_H_

#	include <stdio.h>

#	define GOC_PRINT(stream, type, str)\
		fprintf(stream, "[%s] [%s:%d] %s\n",\
			type, __FILE__, __LINE__, str)

#	define GOC_FPRINT(stream, type, str) { \
		char *cstr = (char *)str; \
		GOC_PRINT(stream, type, cstr); \
		goc_stringFree(cstr); \
	}

#	ifndef GOC_BPRINT_SIZE
#		define GOC_BPRINT_SIZE 1024
#	endif

#	define GOC_BPRINT(stream, type, str, a...) { \
		char *cstr = (char*)malloc(GOC_BPRINT_SIZE); \
		sprintf(cstr, str, a); \
		GOC_PRINT(stream, type, cstr); \
		free(cstr); \
	}

#	ifdef GOC_PRINTDEBUG
#		ifndef GOC_DEBUGSTREAM
#			define GOC_DEBUGSTREAM stderr
#		endif
#		define GOC_DEBUG(str) GOC_PRINT(GOC_DEBUGSTREAM, "GOC_DEBUG", str)
#		define GOC_FDEBUG(str) GOC_FPRINT(GOC_DEBUGSTREAM, "GOC_DEBUG", str)
#		define GOC_BDEBUG(str, a...) GOC_BPRINT(GOC_DEBUGSTREAM, "GOC_DEBUG", str, a)
#	else
#		define GOC_DEBUG(str)
#		define GOC_FDEBUG(str)
#		define GOC_BDEBUG(str, a...)
#	endif

#	ifdef GOC_PRINTINFO
#		ifndef GOC_INFOSTREAM
#			define GOC_INFOSTREAM stderr
#		endif
#		define GOC_INFO(str) GOC_PRINT(GOC_INFOSTREAM, "GOC_INFO", str)
#		define GOC_FINFO(str) GOC_FPRINT(GOC_INFOSTREAM, "GOC_INFO", str)
#		define GOC_BINFO(str, a...) GOC_BPRINT(GOC_INFOSTREAM, "GOC_INFO", str, a)
#	else
#		define GOC_INFO(str)
#		define GOC_FINFO(str)
#		define GOC_BINFO(str, a...)
#	endif

#	ifdef GOC_PRINTERROR
#		ifndef GOC_ERRORSTREAM
#			define GOC_ERRORSTREAM stderr
#		endif
#		define GOC_ERROR(str) GOC_PRINT(GOC_ERRORSTREAM, "GOC_ERROR", str)
#		define GOC_FERROR(str) GOC_FPRINT(GOC_ERRORSTREAM, "GOC_ERROR", str)
#		define GOC_BERROR(str, a...) GOC_BPRINT(GOC_ERRORSTREAM, "GOC_ERROR", str, a)
#	else
#		define GOC_ERROR(str)
#		define GOC_FERROR(str)
#		define GOC_BERROR(str, a...)
#	endif

#	define GOC_CHECK_DEBUG(condition) if ( !(condition) ) {GOC_DEBUG( #condition" --- failed" );}
#	define GOC_CHECK(condition) if ( !(condition) ) {GOC_ERROR( #condition" --- failed" );}
#	define GOC_CHEOP_DEBUG(condition, operation) if ( !(condition) ) {GOC_DEBUG( #condition" --- failed" ); operation;}
#	define GOC_CHEOP(condition, operation) if ( !(condition) ) {GOC_ERROR( #condition" --- failed" ); operation;}
#	define GOC_MSG_CHEOP(message, condition, operation) if ( !(condition) ) {GOC_ERROR( message ); operation;}
#endif // ifndef _GOC_LOG_H_
