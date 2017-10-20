#include <stdio.h>
#include <okienkoc/fileistream.h>
#include <okienkoc/memistream.h>
#include <okienkoc/memostream.h>
#define GOC_PRINTINFO
#define GOC_PRINTERROR
#include <okienkoc/log.h>
#include <okienkoc/mystr.h>


int main(int argc, char **argv)
{
	GOC_IStream *is = NULL;
	GOC_OStream *os = NULL;
	int c;
//	unsigned char *pBuff;
//	unsigned int nBuff;
	char *line;

	if ( argc < 2 )
	{
		GOC_ERROR("No input file name");
		return -1;
	}

	os = goc_memOStreamOpen();

	GOC_INFO("--- GOC_FileIStream test");
	is = goc_fileIStreamOpen(argv[1]);
//	nBuff = goc_isAvailable(is);
//	pBuff = malloc(nBuff);
	GOC_BINFO("Available: %d\n", goc_isAvailable(is));
//	nBuff = 0;
	while ( (c = goc_isReadChar(is)) > 0 )
	{
		goc_osWriteByte(os, (unsigned char)c);
//		pBuff[nBuff++] = c;
		putchar(c);
	}
	goc_isClose(is);

	GOC_INFO("--- GOC_MemIStream test\n");
	is = goc_memIStreamAttach(goc_memOStreamGet(os), goc_memOStreamSize(os));
//	nBuff = goc_isAvailable(is);
	GOC_BINFO("Available: %d\n", goc_isAvailable(is));
	while ( (c = goc_isReadChar(is)) > 0 )
		putchar(c);
	goc_isClose(is);

	GOC_INFO("--- GOC_FileIStream readLine test");
	is = goc_fileIStreamOpen(argv[1]);
	while ( (line = goc_isReadLine(is)) )
	{
		printf("%s\n", line);
	}
	goc_isClose(is);
	
	return 0;
}
