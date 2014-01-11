#include "global-inc.h"
#include <stdio.h>
#ifndef _DEBUG
#	include <string.h>
#endif

#include "memistream.h"
#define GOC_PRINTERROR
#include "log.h"

#define GOC_MALLOC(var, type) type* var = (type*)malloc(sizeof(type));

// GOC_MemIStream inner functions
static int memIStreamClose(GOC_IStream *is)
{
	GOC_MemIStream *bis = (GOC_MemIStream *)is;
	if ( bis->pBuffer )
		free(bis->pBuffer);
	bis->pBuffer = NULL;
	bis->pEnd = NULL;
	bis->pPos = NULL;
	free( bis );
	return 0;
}

static int memIStreamReadChar(GOC_IStream *is)
{
	GOC_MemIStream *bis = (GOC_MemIStream *)is;
	if ( bis->pPos != bis->pEnd )
	{
		unsigned char v = *bis->pPos;
		bis->pPos++;
		return v;
	}
	else
		return -1;
}
/*
static int memIStreamReadArray(GOC_IStream *is, unsigned char *pBuffer, unsigned int nBuffer)
{
	GOC_MemIStream *bis = (GOC_MemIStream *)is;
	if ( bis->pPos != bis->pEnd )
	{
		if ( bis->pPos + nBuffer < bis->pEnd )
		{
			memcpy(pBuffer, bis->pPos, nBuffer);
			bis->pPos += nBuffer;
			return nBuffer;
		}
		else
		{
			unsigned int tc = (unsigned int)(bis->pEnd - bis->pPos);
			memcpy(pBuffer, bis->pPos, tc);
			return tc;
		}
	}
	else
		return -1;
}
*/
static int memIStreamAvailable(GOC_IStream *is)
{
	GOC_MemIStream *bis = (GOC_MemIStream *)is;
	return bis->pEnd-bis->pPos;
}

static int memIStreamSeek(GOC_IStream *is, long offset, int whence)
{
	GOC_MemIStream *bis = (GOC_MemIStream *)is;
	if ( whence == SEEK_SET )
	{
		if ( offset > 0 )
		{
			if ( bis->pBuffer + offset < bis->pEnd )
				bis->pPos = bis->pBuffer + offset;
			else
				bis->pPos = bis->pEnd;
		}
		else
			bis->pPos = bis->pBuffer;
	}
	else if ( whence == SEEK_CUR )
	{
		if ( offset >= 0 )
		{
			if ( bis->pPos + offset < bis->pEnd )
				bis->pPos += offset;
			else
				bis->pPos = bis->pEnd;
		}
		else
		{
			if ( bis->pPos - offset > bis->pBuffer )
				bis->pPos -= offset;
			else
				bis->pPos = bis->pBuffer;
		}
	}
	else if ( whence == SEEK_END )
	{
		if ( offset > 0 )
		{
			if ( bis->pEnd - offset > bis->pBuffer )
				bis->pPos = bis->pEnd - offset;
			else
				bis->pPos = bis->pBuffer;
		}
		else
			bis->pPos = bis->pEnd;

	}
	else
	{
		GOC_ERROR("Unknown whence value");
		return -2;
	}
	return 0;
}

// Function make its own copy of buffer !
GOC_IStream *goc_memIStreamOpen(const unsigned char *pBuffer, unsigned int nBuffer)
{
	GOC_MALLOC(bis, GOC_MemIStream);
	bis->readChar = memIStreamReadChar;
//	bis->readArray = memIStreamReadArray;
	bis->close = memIStreamClose;
	bis->available = memIStreamAvailable;
	bis->seek = memIStreamSeek;
	bis->pBuffer = malloc(nBuffer);
	memcpy(bis->pBuffer, pBuffer, nBuffer);
	bis->pEnd = bis->pBuffer + nBuffer;
	bis->pPos = bis->pBuffer;
	return (GOC_IStream *)bis;
}

// Function set buffer from pointer ! Close it and free after all !
GOC_IStream *goc_memIStreamAttach(unsigned char *pBuffer, unsigned int nBuffer)
{
	GOC_MALLOC(bis, GOC_MemIStream);
	bis->readChar = memIStreamReadChar;
//	bis->readArray = memIStreamReadArray;
	bis->close = memIStreamClose;
	bis->available = memIStreamAvailable;
	bis->seek = memIStreamSeek;
	bis->pBuffer = pBuffer;
	bis->pEnd = bis->pBuffer + nBuffer;
	bis->pPos = bis->pBuffer;
	return (GOC_IStream *)bis;
}
