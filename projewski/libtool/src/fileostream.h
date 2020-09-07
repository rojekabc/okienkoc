#ifndef _GOC_FILEOSTREAM_H_
#define _GOC_FILEOSTREAM_H_

#	include <stdio.h>

#	include "ostream.h"

#	define GOC_STRUCT_FILEOSTREAM \
		GOC_STRUCT_OSTREAM; \
		FILE *handler;

typedef struct GOC_FileOStream
{
	GOC_STRUCT_FILEOSTREAM;
} GOC_FileOStream;

GOC_OStream *goc_fileOStreamOpen(const char *filename);

#endif // ifndef _GOC_FILEOSTREAM_H_
