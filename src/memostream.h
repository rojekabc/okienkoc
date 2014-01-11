#ifndef _GOC_MEMOSTREAM_H_
#define _GOC_MEMOSTREAM_H_

#	include "ostream.h"

#	define GOC_STRUCT_MEMOSTREAM \
		GOC_STRUCT_OSTREAM; \
		unsigned char *pBuffer; \
		unsigned int nBuffer;

typedef struct GOC_MemOStream
{
	GOC_STRUCT_MEMOSTREAM;
} GOC_MemOStream;

GOC_OStream *goc_memOStreamOpen();
unsigned char *goc_memOStreamGet(GOC_OStream *os);
unsigned int goc_memOStreamSize(GOC_OStream *os);

#endif // ifndef _GOC_MEMOSTREAM_H_
