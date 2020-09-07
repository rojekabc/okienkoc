#ifndef _GOC_OSTREAM_H_
#define _GOC_OSTREAM_H_

#define GOC_STRUCT_OSTREAM \
	int (*write)(struct GOC_OStream *, unsigned char b); \
	int (*close)(struct GOC_OStream *); \
	int (*flush)(struct GOC_OStream *)

typedef struct GOC_OStream
{
	GOC_STRUCT_OSTREAM;
} GOC_OStream;

int goc_osWriteByte(GOC_OStream *os, unsigned char b);
int goc_osWrite(GOC_OStream *os, const void *pArray, unsigned int nArray);
int goc_osFlush(GOC_OStream *os);
int goc_osClose(GOC_OStream *os);

#endif // ifndef _GOC_OSTREAM_H_
