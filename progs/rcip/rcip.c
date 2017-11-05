#include <stdlib.h>
#include <stdio.h>
#include <tools/mystr.h>

// TODO : Inner random number generator - to save from differents operating
// system
//
int printHelp(const char *prgName, const char *errorMsg)
{
	fprintf(stderr, "%s\n", errorMsg);
	fprintf(stderr, "%s <parameters> <inputfile>\n", prgName);
	fprintf(stderr, "Parameters\n");
	fprintf(stderr, "-c xxxxxxxx  Cipher code\n");
	fprintf(stderr, "-o filename  Output filename [Default: stdout]\n");
	if ( errorMsg == NULL )
		return 0;
	else
		return -1;
}

int a = 397204094;
int m = 1073741823; // 2^30-1
int c = 0;

int nextRand(int x)
{
	x *= a;
	x &= m;
	return x;
}

int main(int argc, char **argv)
{
	int i;
	unsigned int r;
	unsigned int code = 0;
	FILE *inStream = NULL;
	FILE *outStream = NULL;

	// informacje wejsciowe
	fprintf(stderr, "int size = %ld\n", sizeof(int));

	// rozpoznanie argumentow wejsciowych
	for (i=1; i<argc; i++)
	{
		if ( goc_stringEquals(argv[i], "-c") )
		{
			i++;
			if ( i >= argc )
				return printHelp(argv[0], "Wrong arguments");
			sscanf(argv[i], "%x", &code);
		}
		else if ( goc_stringEquals(argv[i], "-o") )
		{
			i++;
			if ( i >= argc )
				return printHelp(argv[0], "Wrong arguments");
			outStream = fopen( argv[i], "wb" );
			if ( outStream == NULL )
				return fprintf(stderr, "Cannot open a file %s\n", argv[i]);
		}
		else
		{
			// podano nazwę pliku
			inStream = fopen( argv[i], "rb" );
			if ( inStream == NULL )
				return fprintf(stderr, "Cannot open a file %s\n", argv[i]);
		}
	}

	// uzupelnianie brakujacych danych
	if ( code == 0 )
	{
		fprintf(stderr, "Give me a code: ");
		fscanf(stdin, "%x", &code);
	}
	if ( inStream == NULL )
		inStream = stdin;
	if ( outStream == NULL )
		outStream = stdout;

//	// zainicjowanie
//	srand(code);

	// proces szyfrowania
	int rcnt;
	int size = 0;
	int x = code;
	while ( ( rcnt = fread( &r, 1, sizeof(int), inStream ) ) )
	{
		x = nextRand( x );
		r ^= x; // rand();
		fwrite( &r, 1, rcnt, stdout );
		size += rcnt;
	}
	fprintf(stderr, "Przetworzono %d B\n", size);
	if ( inStream != stdin )
		fclose(inStream);
	return 0;
}
