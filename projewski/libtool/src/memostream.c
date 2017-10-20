#include <string.h>

#include "global-inc.h"
#include "memostream.h"

int memOStreamWrite(GOC_OStream *os, unsigned char b)
{
	GOC_MemOStream* mos = (GOC_MemOStream*)os;
	mos->nBuffer++;
	if ( mos->pBuffer )
		mos->pBuffer = realloc(mos->pBuffer, mos->nBuffer);
	else
		mos->pBuffer = malloc(mos->nBuffer);
	mos->pBuffer[mos->nBuffer-1] = b;
	return 0;
}

int memOStreamClose(GOC_OStream *os)
{
	GOC_MemOStream* mos = (GOC_MemOStream*)os;
	if ( mos->pBuffer )
		free(mos->pBuffer);
	free(mos);
	return 0;
}

int memOStreamFlush(GOC_OStream *os)
{
	return 0;
}

GOC_OStream *goc_memOStreamOpen()
{
	GOC_MemOStream* res = (GOC_MemOStream*)malloc(sizeof(GOC_MemOStream));
	memset(res, 0, sizeof(GOC_MemOStream));
	res->close = memOStreamClose;
	res->write = memOStreamWrite;
	res->flush = memOStreamFlush;
	return (GOC_OStream*)res;
}

unsigned char *goc_memOStreamGet(GOC_OStream *os)
{
	GOC_MemOStream* mos = (GOC_MemOStream*)os;
	return mos->pBuffer;
}

unsigned int goc_memOStreamSize(GOC_OStream *os)
{
	GOC_MemOStream* mos = (GOC_MemOStream*)os;
	return mos->nBuffer;
}
