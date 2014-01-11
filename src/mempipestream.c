#include "mempipestream.h"
#include "conflog.h"
#include <malloc.h>
#include <string.h>

// Kiedy nastepuje odczyt strumien danych jest tworzony ze strumienia wypelniania
#define CHECK_NULL(x) \
	if ( x == NULL ) \
	{ \
		GOC_ERROR("Parameter " #x " is NULL"); \
		return -2; \
	}

GOC_MemPipeStream *goc_memPipeOpen()
{
	GOC_MemPipeStream* pipe = malloc(sizeof(GOC_MemPipeStream));
	memset(pipe, 0, sizeof(GOC_MemPipeStream));
	return pipe;
}

int goc_memPipeReadChar(GOC_MemPipeStream* pipe)
{
	int c = -1;
	CHECK_NULL(pipe);
	// sprawdzenie, czy mamy strumien danych wejsciowych
	if ( pipe->istream )
	{
		c = goc_isReadChar(pipe->istream);
		// strumien danych wejsciowych jeszcze cos ma
		if ( c != -1 )
			return c;
		// strumien wejsciowy skonczyl sie - zamknij go
		goc_isClose(pipe->istream);
		pipe->istream = NULL;
	}
	// czy mamy nastepne dane do odczytu
	if ( !pipe->ostream )
		return -1;
	// zalozenie nowego strumienia wejsciowego z danych wyjsciowych
	pipe->istream = goc_memIStreamOpen(
		goc_memOStreamGet(pipe->ostream),
		goc_memOStreamSize(pipe->ostream));
	// zamknij strumien do zapisu
	goc_osClose(pipe->ostream);
	pipe->ostream = NULL;
	// wykonaj ponowny odczyt z pipe
	return goc_memPipeReadChar(pipe);
}

int goc_memPipeWriteByte(GOC_MemPipeStream* pipe, unsigned char b)
{
	CHECK_NULL(pipe);
	if ( !pipe->ostream )
		pipe->ostream = goc_memOStreamOpen();
	return goc_osWriteByte(pipe->ostream, b);
}

int goc_memPipeWrite(GOC_MemPipeStream* pipe, const void* pBuf, unsigned int nBuf)
{
	CHECK_NULL(pipe);
	if ( !pipe->ostream )
		pipe->ostream = goc_memOStreamOpen();
	return goc_osWrite(pipe->ostream, pBuf, nBuf);
}

int goc_memPipeReadArray(GOC_MemPipeStream *pipe, unsigned char *pBuffer, unsigned int nBuffer)
{
	int c;
	int cnt = -1;
	CHECK_NULL(pipe);
	CHECK_NULL(pBuffer);
	// sprawdzenie, czy mamy strumien danych wejsciowych
	if ( pipe->istream )
	{
		cnt = goc_isReadArray(pipe->istream, pBuffer, nBuffer);
		// strumien danych wejsciowych jeszcze cos ma
		if (( cnt != -1 ) && ( cnt == nBuffer ))
			return cnt;
		// strumien wejsciowy skonczyl sie - zamknij go
		goc_isClose(pipe->istream);
		pipe->istream = NULL;
	}
	// czy mamy nastepne dane do odczytu
	if ( !pipe->ostream )
		return cnt;
	if ( cnt == -1 )
		cnt ^= cnt;
	// zalozenie nowego strumienia wejsciowego z danych wyjsciowych
	pipe->istream = goc_memIStreamOpen(
		goc_memOStreamGet(pipe->ostream),
		goc_memOStreamSize(pipe->ostream));
	// zamknij strumien do zapisu
	goc_osClose(pipe->ostream);
	pipe->ostream = NULL;
	// wykonaj dalszy odczyt danych
	c = goc_isReadArray(pipe->istream, pBuffer + cnt, nBuffer);
	if ( c > 0 )
		cnt += c;
	if ( cnt < nBuffer )
	{
		goc_isClose(pipe->istream);
		pipe->istream = NULL;
	}
	return cnt;
}

int goc_memPipeSize(GOC_MemPipeStream* pipe)
{
	int size = 0;
	CHECK_NULL(pipe);
	if ( pipe->istream != NULL )

	return size;
}
