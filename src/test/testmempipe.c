#include <okienkoc/mempipestream.h>
#include <string.h>
#include <stdio.h>

int main()
{
	const char* str = "Testowy ciag znakow.";
	const char* str2 = " Znakow pisanych";
	char pBuf[6];
	int c = 0;
	GOC_MemPipeStream* pipe = goc_memPipeOpen();

	goc_memPipeWrite(pipe, str, strlen(str));
	goc_memPipeWrite(pipe, str2, strlen(str2));
	while ( (c = goc_memPipeReadChar( pipe )) > 0 )
	{
		putchar(c);
		if ( c == 'a' )
			goc_memPipeWrite(pipe, "+", 1);
	}

	putchar('\n');

	goc_memPipeWrite(pipe, str, strlen(str));
	goc_memPipeWrite(pipe, str2, strlen(str2));
	memset(pBuf, 0, 6);
	while ( (c = goc_memPipeReadArray( pipe, pBuf, 5 )) > 0 )
	{
		pBuf[c] = 0;
		fputs(pBuf, stdout);
		if ( strchr(pBuf, 'a') )
			goc_memPipeWrite(pipe, "+", 1);
	}

	putchar('\n');
	fflush(stdout);

	return 0;
}
