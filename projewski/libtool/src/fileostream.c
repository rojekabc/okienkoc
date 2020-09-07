#include "global-inc.h"
#include <stdio.h>

#include "fileostream.h"
#define GOC_PRINTERROR
#include "log.h"

#define GOC_MALLOC(var, type) type* var = (type*)malloc(sizeof(type));

// GOC_FileOStream inner functions
static int fileOStreamClose(GOC_OStream *os)
{
	GOC_FileOStream *fos = (GOC_FileOStream *)os;
	fclose( fos->handler );
	free( fos );
	return 0;
}

static int fileOStreamFlush(struct GOC_OStream* os) {
    fflush(((GOC_FileOStream *)os)->handler);
    return 0;
}

static int fileOStreamWrite(struct GOC_OStream *os, unsigned char b) {
    fputc(b, ((GOC_FileOStream *)os)->handler);
    return 0;
}

GOC_OStream *goc_fileOStreamOpen(const char *filename)
{
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        GOC_ERROR("Error while opening file");
        return NULL;
    }
	GOC_MALLOC(fos, struct GOC_FileOStream);
	fos->close = fileOStreamClose;
	fos->flush = fileOStreamFlush;
	fos->write = fileOStreamWrite;
	fos->handler = file;
	return (GOC_OStream *)fos;
}

