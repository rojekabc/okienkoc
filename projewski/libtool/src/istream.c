#include <stdio.h>
#include <string.h>
#include "istream.h"
#define GOC_PRINTERROR
#include "log.h"
#include "mystr.h"

// GOC_IStream functions

// >=0 odczytany znak
// -1 EOF
// -2 GOC_ERROR
int goc_isReadChar(GOC_IStream *is)
{
	if ( is == NULL )
	{
		GOC_ERROR("NULL Input Stream");
		return -2;
	}
	if ( is->readChar == NULL )
	{
		GOC_ERROR("readChar function not implemented");
		return -2;
	}
	return is->readChar(is);
}

// >= 0 liczba odczytanych znakow
// -1 EOF
// -2 GOC_ERROR
int goc_isReadArray(GOC_IStream *is, unsigned char *pBuffer, unsigned int nBuffer)
{
	int cnt = 0;
	if ( is == NULL )
	{
		GOC_ERROR("NULL Input Stream");
		return -2;
	}
	if ( pBuffer == NULL )
	{
		GOC_ERROR("NULL array");
		return -2;
	}
	if ( is->readChar == NULL )
	{
		GOC_ERROR("readChar function not implemented");
		return -2;
	}
	while ( nBuffer-- )
	{
		int c = is->readChar(is);
		if ( c < 0 )
			break;
		*pBuffer = (unsigned char)c;
		pBuffer++;
		cnt++;
	}
	if ( cnt > 0 )
		return cnt;
	else
		return -1;
}

char *goc_isReadLine(GOC_IStream *is)
{
	int c;
	char buf[128];
	char *tmp = NULL;
	int i;

	if ( is == NULL )
	{
		GOC_ERROR("NULL input stream");
		return NULL;
	}

	if ( is->readChar == NULL )
	{
		GOC_ERROR("readChar function not implemented");
		return NULL;
	}

	i = 0;
	memset(buf, 0, 128);

	while ( (c = is->readChar(is)) >= 0 )
	{
		// linux end line
		if ( c == 0x0A )
			break;
		buf[i++] = (char)c;
		if ( i == 127 )
		{
			i = 0;
			tmp = goc_stringAdd(tmp, buf);
			memset(buf, 0, 128);
		}
	}
	if (( tmp == NULL ) && ( buf[0] == 0 ) && ( c < 0 ))
		return NULL;
	return goc_stringAdd(tmp, buf);
}

// == 0 OK
// -2 GOC_ERROR
int goc_isClose(GOC_IStream *is)
{
	if ( is == NULL )
	{
		GOC_ERROR("NULL Input Stream");
		return -2;
	}
	if ( is->close == NULL )
	{
		GOC_ERROR("close function not implemented");
		return -2;
	}
	return is->close(is);
}

// >=0 available bytes
// -2 GOC_ERROR
int goc_isAvailable(GOC_IStream *is)
{
	if ( is == NULL )
	{
		GOC_ERROR("NULL Input Stream");
		return -2;
	}
	if ( is->available == NULL )
	{
		GOC_ERROR("available function not implemented");
		return -2;
	}
	return is->available(is);
}

int goc_isSetPos(GOC_IStream *is, long pos)
{
	if ( is == NULL )
	{
		GOC_ERROR("NULL Input Stream");
		return -2;
	}
	if ( is->seek == NULL )
	{
		GOC_ERROR("reset function not implemented");
		return -2;
	}
	return is->seek(is, pos, SEEK_SET);
}

int goc_isSeek(GOC_IStream *is, long offset, int whence)
{
	if ( is == NULL )
	{
		GOC_ERROR("NULL Input Stream");
		return -2;
	}
	if ( is->seek == NULL )
	{
		GOC_ERROR("reset function not implemented");
		return -2;
	}
	return is->seek(is, offset, whence);
}

