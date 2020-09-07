#include "ostream.h"
#define GOC_PRINTERROR
#include "log.h"

#define CHECK_NULL(x) \
	if ( x == NULL ) \
	{ \
		GOC_ERROR("Parameter " #x " is NULL"); \
		return -2; \
	}

#define CHECK_RESULT(y) \
	{ \
		int res = y; \
		if ( res < 0 ) \
			return res; \
	}
		

int goc_osWriteByte(GOC_OStream *os, unsigned char b)
{
	CHECK_NULL(os);
	CHECK_NULL(os->write);
	os->write(os, b);
	return 0;
}

int goc_osWrite(GOC_OStream *os, const void *pArray, unsigned int nArray)
{
	CHECK_NULL(os);
	CHECK_NULL(os->write);

	if ( pArray )
	{
		const unsigned char *pTable = pArray;
		while ( nArray-- )
		{
			os->write(os, *pTable);
			pTable++;
		}
	}
	return 0;
}

int goc_osClose(GOC_OStream *os)
{
	CHECK_NULL(os);
	CHECK_NULL(os->close);
	CHECK_NULL(os->flush);
	CHECK_RESULT(os->flush(os));
	return os->close(os);
}

int goc_osFlush(GOC_OStream *os)
{
	CHECK_NULL(os);
	CHECK_NULL(os->flush);
	return os->flush(os);
}
