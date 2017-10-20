#include "global-inc.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fileistream.h"
#define GOC_PRINTERROR
#include "log.h"

#define GOC_MALLOC(var, type) type* var = (type*)malloc(sizeof(type));

// GOC_FileIStream inner functions
static int fileIStreamClose(GOC_IStream *is)
{
	GOC_FileIStream *fis = (GOC_FileIStream *)is;
	fclose( fis->handler );
	free( fis );
	return 0;
}

static int fileIStreamReadChar(GOC_IStream *is)
{
	GOC_FileIStream *fis = (GOC_FileIStream *)is;
	return fgetc( fis->handler );
}
/*
static int fileIStreamReadArray(GOC_IStream *is, unsigned char *pBuffer, unsigned int nBuffer)
{
	GOC_FileIStream *fis = (GOC_FileIStream *)is;
	return fread(pBuffer, 1, nBuffer, fis->handler);
}
*/
static int fileIStreamAvailable(GOC_IStream *is)
{
	GOC_FileIStream *fis = (GOC_FileIStream *)is;
	int filedes;
	long pos;
	struct stat st;
	if ( fis == NULL )
	{
		GOC_ERROR("NULL I Stream");
		return -2;
	}
	filedes = fileno(fis->handler);
	pos = ftell(fis->handler);
	fstat(filedes, &st);
	return st.st_size-pos;
}

static int fileIStreamSeek(GOC_IStream *is, long offset, int whence)
{
	GOC_FileIStream *fis = (GOC_FileIStream *)is;
	if ( fseek(fis->handler, offset, whence) < 0 )
		return -2;
	return 0;
}

GOC_IStream *goc_fileIStreamOpen(const char *filename)
{
	GOC_MALLOC(fis, GOC_FileIStream);
	fis->readChar = fileIStreamReadChar;
//	fis->readArray = fileIStreamReadArray;
	fis->close = fileIStreamClose;
	fis->available = fileIStreamAvailable;
	fis->seek = fileIStreamSeek;
	fis->handler = fopen(filename, "rb");
	if ( fis->handler == NULL )
	{
		free( fis );
		GOC_ERROR("Error while opening file");
		fis = NULL;
	}
	return (GOC_IStream *)fis;
}

