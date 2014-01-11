#ifndef _GOC_MEMISTREAM_H_
#define _GOC_MEMISTREAM_H_

#	include "istream.h"

#	define GOC_STRUCT_MEMISTREAM \
		GOC_STRUCT_ISTREAM; \
		unsigned char *pBuffer; \
		unsigned char *pEnd; \
		unsigned char *pPos;

typedef struct GOC_MemIStream
{
	GOC_STRUCT_MEMISTREAM;
} GOC_MemIStream;

GOC_IStream *goc_memIStreamOpen(const unsigned char *pBuffer, unsigned int nBuffer);

/**
 * Function points on external memory buffer.
 * After all a memory is free by structure.
 */
GOC_IStream *goc_memIStreamAttach(unsigned char *pBuffer, unsigned int nBuffer);

#endif // ifndef _GOC_MEMISTREAM_H_
