#ifndef _GOC_MEMPIPESTREAM_H_
#	define _GOC_MEMPIPESTREAM_H_

#	include "ostream.h"
#	include "istream.h"

#	define GOC_STRUCT_MEMPIPESTREAM \
		GOC_STRUCT_OSTREAM; \
		GOC_STRUCT_ISTREAM;

typedef struct GOC_MemPipeStream
{
	// strumien danych do odczytu
	GOC_IStream* istream;
	// strumien danych do wypelniania
	GOC_OStream* ostream;
} GOC_MemPipeStream;

GOC_MemPipeStream *goc_memPipeOpen();
int goc_memPipeReadChar(GOC_MemPipeStream* pipe);
int goc_memPipeWriteByte(GOC_MemPipeStream* pipe, unsigned char b);
int goc_memPipeWrite(GOC_MemPipeStream* pipe, const void* pBuf, unsigned int nBuf);
int goc_memPipeReadArray(GOC_MemPipeStream *pipe, unsigned char *pBuffer, unsigned int nBuffer);
int goc_memPipeSize(GOC_MemPipeStream* pipe);

#endif // define _GOC_MEMPIPESTREAM_H_
