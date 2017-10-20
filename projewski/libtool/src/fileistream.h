#ifndef _GOC_FILEISTREAM_H_
#define _GOC_FILEISTREAM_H_

#	include <stdio.h>

#	include "istream.h"

#	define GOC_STRUCT_FILEISTREAM \
		GOC_STRUCT_ISTREAM; \
		FILE *handler;

typedef struct GOC_FileIStream
{
	GOC_STRUCT_FILEISTREAM;
} GOC_FileIStream;

GOC_IStream *goc_fileIStreamOpen(const char *filename);

#endif // ifndef _GOC_FILEISTREAM_H_
