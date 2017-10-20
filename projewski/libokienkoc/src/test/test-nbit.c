#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>

#include <okienkoc/nbitpola.h>
#include <okienkoc/random.h>
#include <okienkoc/mystr.h>
#include <okienkoc/tablica.h>
#include <okienkoc/log.h>

#define GOC_FLAGSTATS 1
#define GOC_FLAGDEBUG 2
#define GOC_IS_STATS (flag & GOC_FLAGSTATS)
#define GOC_IS_DEBUG (flag & GOC_FLAGDEBUG)
unsigned char flag = 0;

unsigned int timertick;
#define starttimer( _int_tickcounter_, _sigalarmfunction_ ) \
{ \
	struct itimerval tv; \
	struct sigaction sa; \
	memset( &sa, 0, sizeof( struct sigaction ) ); \
	sa.sa_handler=&_sigalarmfunction_; \
	sigaction(SIGALRM, &sa, 0); \
	memset( &tv, 0, sizeof( struct itimerval ) ); \
	tv.it_value.tv_usec=100; \
	tv.it_interval.tv_usec=10000; \
	_int_tickcounter_=0; \
	setitimer(ITIMER_REAL, &tv, 0); \
}

#define stoptimer() \
{ \
	setitimer(ITIMER_REAL, 0, 0); \
}
static void sigtimeralarm(int a)
{
	timertick++;
}

const char *vals="0123456789ABCDEF";

void printBytes(unsigned char *pBuf, int nBuf)
{
	int i;
	printf("len = %d [", nBuf);
	for (i=0; i<nBuf; i++)
		printf("%02x", (unsigned int)pBuf[i]);
	printf("]\n");
}

// Testy zamiany na String, odczytywania ze Stringa
void test_001()
{
	// test na pozyskiwanie NBitPola jako String. Liczba danych losowana,
	// dane losowane. Dane dwubitowe

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	char *str = NULL; // ciag generowany przez funkcje
	char *strGen = NULL; // ciag obliczany recznie
	int ld = 0; // liczba danych

	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);
	
	while (LICZBA_TESTOW--)
	{ 
		ld = 1+goc_random(255);
		pole = goc_nbitFieldCreate(1, 0);
		while (ld--)
		{
			int val = goc_random(2);
			goc_nbitFieldAdd(pole, val);
			strGen = goc_stringAddInt( strGen, val );
		}
		GOC_FDEBUG( goc_stringAdd( goc_stringAdd( goc_stringCopy(NULL, 
			"test_001 [GOC_DEBUG]: Ciag testowy: ["), strGen), "%s]\n") );
		str = goc_nbitFieldToString(pole);
		if ( !goc_stringEquals( str, strGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny string\n" );
			strFail = goc_stringAdd( strFail, "\tOczekiwano: [" );
			strFail = goc_stringAdd( strFail, strGen );
			strFail = goc_stringAdd( strFail, "]\n" );
			strFail = goc_stringAdd( strFail, "\tOtrzymano: [" );
			strFail = goc_stringAdd( strFail, str );
			strFail = goc_stringAdd( strFail, "]\n" );
			GOC_FERROR( strFail );
		}
		str = goc_stringFree( str );
		strGen = goc_stringFree( strGen );
		pole = goc_nbitFieldRemove( pole );
	}
	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_001 [STATS]: Time: %04d ticks\n", timertick);
	}
}

void test_002()
{
	// test na ustawianie NBitPola ze String. Liczba danych losowana,
	// dane losowane. Dane dwubitowe

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	char *str = NULL; // ciag generowany z pola
	char *strGen = NULL; // ciag ustawiany dla pola
	int ld = 0; // liczba danych

	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);
	
	while (LICZBA_TESTOW--)
	{ 
		ld = 1+goc_random(255);
		pole = goc_nbitFieldCreate(1, 0);
		while (ld--)
		{
			int val = goc_random(2);
			strGen = goc_stringAddInt( strGen, val );
		}
		if ( GOC_IS_DEBUG )
			printf("test_002 [GOC_DEBUG]: Ciag testowy: [%s]\n", strGen);
		goc_nbitFieldFromString(pole, strGen);
		str = goc_nbitFieldToString(pole);
		if ( !goc_stringEquals( str, strGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny string\n" );
			strFail = goc_stringAdd( strFail, "\tOczekiwano: [" );
			strFail = goc_stringAdd( strFail, strGen );
			strFail = goc_stringAdd( strFail, "]\n" );
			strFail = goc_stringAdd( strFail, "\tOtrzymano: [" );
			strFail = goc_stringAdd( strFail, str );
			strFail = goc_stringAdd( strFail, "]\n" );
			GOC_FERROR( strFail );
		}
		str = goc_stringFree( str );
		strGen = goc_stringFree( strGen );
		pole = goc_nbitFieldRemove( pole );
	}
	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_002 [STATS]: Time: %04d ticks\n", timertick);
	}
}

void test_003()
{
	// test na pozyskiwanie NBitPola jako String. Liczba danych losowana,
	// dane losowane. Ilosc bitow w danych losowana miedzy 1 a 4

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	char *str = NULL; // ciag generowany przez funkcje
	char *strGen = NULL; // ciag obliczany recznie
	int ld = 0; // liczba danych
	int lbnw = 0; // liczba bitow
	int stats[4];

	memset(stats, 0, sizeof(int)*4);
	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);
	
	while (LICZBA_TESTOW--)
	{ 
		ld = 1+goc_random(255);
		lbnw = 1+goc_random(4);
		pole = goc_nbitFieldCreate(lbnw, 0);
		stats[lbnw-1]++;
		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			goc_nbitFieldAdd(pole, val);
			strGen = goc_stringAddLen( strGen, vals + val, 1 );
		}
		if ( GOC_IS_DEBUG )
			printf("test_003 [GOC_DEBUG]: Ciag testowy: [%s]\n", strGen);
		str = goc_nbitFieldToString(pole);
		if ( !goc_stringEquals( str, strGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny string\n" );
			strFail = goc_stringAdd( strFail, "\tOczekiwano: [" );
			strFail = goc_stringAdd( strFail, strGen );
			strFail = goc_stringAdd( strFail, "]\n" );
			strFail = goc_stringAdd( strFail, "\tOtrzymano: [" );
			strFail = goc_stringAdd( strFail, str );
			strFail = goc_stringAdd( strFail, "]\n" );
			GOC_FERROR( strFail );
		}
		str = goc_stringFree( str );
		strGen = goc_stringFree( strGen );
		pole = goc_nbitFieldRemove( pole );
	}
	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_003 [STATS]: Time: %04d ticks\n", timertick);
	}
	if ( GOC_IS_STATS )
	{
		int i;
		for (i=0; i<4; i++)
			printf("test_003 [STATS]: Dla NBitPola o danych %d-bitowych przeprowadzono test %d razy\n", i+1, stats[i] );
	}
}

void test_004()
{
	// test na pozyskiwanie NBitPola jako String. Liczba danych losowana,
	// dane losowane. Ilosc bitow w danych losowana miedzy 1 a 4

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	char *str = NULL; // ciag generowany przez funkcje
	char *strGen = NULL; // ciag obliczany recznie
	int ld = 0; // liczba danych
	int lbnw = 0; // liczba bitow
	int stats[4];

	memset(stats, 0, sizeof(int)*4);
	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);
	
	while (LICZBA_TESTOW--)
	{ 
		ld = 1+goc_random(255);
		lbnw = 1+goc_random(4);
		pole = goc_nbitFieldCreate(lbnw, 0);
		stats[lbnw-1]++;
		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			strGen = goc_stringAddLen( strGen, vals + val, 1 );
		}
		if ( GOC_IS_DEBUG )
			printf("test_004 [%d] [GOC_DEBUG]: Ciag testowy: [%s]\n", LICZBA_TESTOW, strGen);
		goc_nbitFieldFromString(pole, strGen);
		str = goc_nbitFieldToString(pole);
		if ( !goc_stringEquals( str, strGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny string\n" );
			strFail = goc_stringAdd( strFail, "\tOczekiwano: [" );
			strFail = goc_stringAdd( strFail, strGen );
			strFail = goc_stringAdd( strFail, "]\n" );
			strFail = goc_stringAdd( strFail, "\tOtrzymano: [" );
			strFail = goc_stringAdd( strFail, str );
			strFail = goc_stringAdd( strFail, "]\n" );
			GOC_FERROR( strFail );
		}
		str = goc_stringFree( str );
		strGen = goc_stringFree( strGen );
		pole = goc_nbitFieldRemove( pole );
	}
	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_004 [STATS]: Time: %04d ticks\n", timertick);
	}
	if ( GOC_IS_STATS )
	{
		int i;
		for (i=0; i<4; i++)
			printf("test_004 [STATS]: Dla NBitPola o danych %d-bitowych przeprowadzono test %d razy\n", i+1, stats[i] );
	}
}

// Wstawianie wartosci
void test_005()
{
	// test na ustawianie wartosci
	// liczba bitow losowana - pomiedzy 1 a 4
	// liczba danych losowana
	// dane losowane

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	int stats[4];
	int ld = 0; // liczba danych
	int lbnw = 0; // liczba bitow
	char *str = NULL; // ciag generowany przez funkcje
	char *strGen = NULL; // ciag obliczany recznie

	memset(stats, 0, sizeof(int)*4);
	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);

	while ( LICZBA_TESTOW-- )
	{
		ld = 1+goc_random(255);
		lbnw = 1+goc_random(4);
		pole = goc_nbitFieldCreate(lbnw, 0);
		stats[lbnw-1]++;

		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			int pos = goc_random(pole->ld);
			goc_nbitFieldInsert(pole, pos, val);
			strGen = goc_stringInsertCharAt( strGen, pos, vals[val] );
		}
		if ( GOC_IS_DEBUG )
			printf("test_005 [%d] [GOC_DEBUG]: Ciag testowy: [%s]\n", LICZBA_TESTOW, strGen);
		str = goc_nbitFieldToString(pole);
		if ( !goc_stringEquals( str, strGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny string\n" );
			strFail = goc_stringAdd( strFail, "\tOczekiwano: [" );
			strFail = goc_stringAdd( strFail, strGen );
			strFail = goc_stringAdd( strFail, "]\n" );
			strFail = goc_stringAdd( strFail, "\tOtrzymano: [" );
			strFail = goc_stringAdd( strFail, str );
			strFail = goc_stringAdd( strFail, "]\n" );
			GOC_FERROR( strFail );
		}
		str = goc_stringFree( str );
		strGen = goc_stringFree( strGen );
		pole = goc_nbitFieldRemove( pole );
	}

	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_005 [STATS]: Time: %04d ticks\n", timertick);
	}
	if ( GOC_IS_STATS )
	{
		int i;
		for (i=0; i<4; i++)
			printf("test_005 [STATS]: Dla NBitPola o danych %d-bitowych przeprowadzono test %d razy\n", i+1, stats[i] );
	}
}

// testy na usuwanie wartosci z pola
void test_006()
{
	// test na usuwanie wartosci
	// liczba bitow losowana - pomiedzy 1 a 4
	// liczba danych losowana
	// dane losowane

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	int stats[4];
	int ld = 0; // liczba danych
	int lbnw = 0; // liczba bitow
	char *str = NULL; // ciag generowany przez funkcje
	char *strGen = NULL; // ciag obliczany recznie

	memset(stats, 0, sizeof(int)*4);
	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);

	while ( LICZBA_TESTOW-- )
	{
		ld = 1+goc_random(255);
		lbnw = 1+goc_random(4);
		pole = goc_nbitFieldCreate(lbnw, 0);
		stats[lbnw-1]++;

		// utworzenie wartosci bazowej
		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			goc_nbitFieldAdd(pole, val);
		}
		strGen = goc_nbitFieldToString( pole );
		if ( GOC_IS_DEBUG )
			printf("test_006 [%d] [GOC_DEBUG]: Ciag testowy przed usuwaniem: [%s]\n", LICZBA_TESTOW, strGen);
		// usuwanie danych
		ld = goc_random(pole->ld); // ile udunac
		while ( ld-- )
		{
			int pos = goc_random(pole->ld);
			goc_nbitFieldCut(pole, pos);
			strGen = goc_stringDelAtPos( strGen, pos );
		}
		if ( GOC_IS_DEBUG )
			printf("test_006 [%d] [GOC_DEBUG]: Ciag testowy po usuwaniu: [%s]\n", LICZBA_TESTOW, strGen);
		str = goc_nbitFieldToString(pole);
		if ( !goc_stringEquals( str, strGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny string\n" );
			strFail = goc_stringAdd( strFail, "\tOczekiwano: [" );
			strFail = goc_stringAdd( strFail, strGen ? strGen : "NULL" );
			strFail = goc_stringAdd( strFail, "]\n" );
			strFail = goc_stringAdd( strFail, "\tOtrzymano: [" );
			strFail = goc_stringAdd( strFail, str ? str : "NULL" );
			strFail = goc_stringAdd( strFail, "]\n" );
			GOC_FERROR( strFail );
		}
		str = goc_stringFree( str );
		strGen = goc_stringFree( strGen );
		pole = goc_nbitFieldRemove( pole );
	}

	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_006 [STATS]: Time: %04d ticks\n", timertick);
	}
	if ( GOC_IS_STATS )
	{
		int i;
		for (i=0; i<4; i++)
			printf("test_006 [STATS]: Dla NBitPola o danych %d-bitowych przeprowadzono test %d razy\n", i+1, stats[i] );
	}
}

void test_007()
{
	// test na usuwanie ciagow wartosci od wylosowanej pozycji, a¿ usunie
	// ca³o¶æ
	// liczba bitow losowana - pomiedzy 1 a 4
	// liczba danych losowana
	// dane losowane
	//
	// usuwanie wszystkich wartosci
	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	int stats[4];
	int ld = 0; // liczba danych
	int lbnw = 0; // liczba bitow
	char *str = NULL; // ciag generowany przez funkcje
	char *strGen = NULL; // ciag obliczany recznie

	memset(stats, 0, sizeof(int)*4);
	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);

	while ( LICZBA_TESTOW-- )
	{
		ld = 1+goc_random(255);
		lbnw = 1+goc_random(4);
		pole = goc_nbitFieldCreate(lbnw, 0);
		stats[lbnw-1]++;

		// utworzenie wartosci bazowej
		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			goc_nbitFieldAdd(pole, val);
		}
		strGen = goc_nbitFieldToString( pole );
		if ( GOC_IS_DEBUG )
			printf("test_007 [%d] [GOC_DEBUG]: Ciag testowy przed usuwaniem: [%s]\n", LICZBA_TESTOW, strGen);
		// usuwanie danych
		ld = pole->ld; // ile usunac
		while ( ld-- )
		{
			int pos = goc_random(pole->ld);
			goc_nbitFieldCut(pole, pos);
			strGen = goc_stringDelAtPos( strGen, pos );
		}
		if ( GOC_IS_DEBUG )
			printf("test_007 [%d] [GOC_DEBUG]: Ciag testowy po usuwaniu: [%s]\n", LICZBA_TESTOW, strGen);
		str = goc_nbitFieldToString(pole);
		if ( !goc_stringEquals( str, strGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny string\n" );
			strFail = goc_stringAdd( strFail, "\tOczekiwano: [" );
			strFail = goc_stringAdd( strFail, strGen ? strGen : "NULL" );
			strFail = goc_stringAdd( strFail, "]\n" );
			strFail = goc_stringAdd( strFail, "\tOtrzymano: [" );
			strFail = goc_stringAdd( strFail, str ? str : "NULL" );
			strFail = goc_stringAdd( strFail, "]\n" );
			GOC_FERROR( strFail );
		}
		str = goc_stringFree( str );
		strGen = goc_stringFree( strGen );
		pole = goc_nbitFieldRemove( pole );
	}

	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_007 [STATS]: Time: %04d ticks\n", timertick);
	}
	if ( GOC_IS_STATS )
	{
		int i;
		for (i=0; i<4; i++)
			printf("test_007 [STATS]: Dla NBitPola o danych %d-bitowych przeprowadzono test %d razy\n", i+1, stats[i] );
	}
}

void test_008()
{
	// test na ustawianie wartosci
	// liczba bitow 8
	// liczba danych losowana
	// dane losowane

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	int ld = 0; // liczba danych
	int lbnw = 8; // liczba bitow
	unsigned char *pGen = NULL; // ciag obliczany recznie
	_GOC_TABEL_SIZETYPE_ nGen = 0;

	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);

	while ( LICZBA_TESTOW-- )
	{
		ld = 1+goc_random(255);
		pole = goc_nbitFieldCreate(lbnw, 0);

		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			int pos = goc_random(pole->ld);
			goc_nbitFieldInsert(pole, pos, val);
			pGen = goc_tableInsert(pGen, &nGen, 1, pos);
			pGen[pos] = val;
		}
		//if ( GOC_IS_DEBUG )
		//	printf("test_008 [%d] [GOC_DEBUG]: Ciag testowy: [%s]\n", LICZBA_TESTOW, strGen);

		if ( memcmp( pole->dane, pGen, nGen ) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledny dane\n" );
			GOC_FERROR( strFail );
		}
		pGen = goc_tableClear( pGen, &nGen );
		pole = goc_nbitFieldRemove( pole );
	}

	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_008 [STATS]: Time: %04d ticks\n", timertick);
	}
}

// testy na usuwanie wartosci z pola
void test_009()
{
	// test na usuwanie wartosci
	// liczba bitow 8
	// liczba danych losowana
	// dane losowane

	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	int ld = 0; // liczba danych
	int lbnw = 8; // liczba bitow
	unsigned char *pGen = NULL; // ciag obliczany recznie
	_GOC_TABEL_SIZETYPE_ nGen = 0;

	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);

	while ( LICZBA_TESTOW-- )
	{
		ld = 1+goc_random(255);
		pole = goc_nbitFieldCreate(lbnw, 0);

		// utworzenie wartosci bazowej
		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			goc_nbitFieldAdd(pole, val);
		}
		pGen = malloc(pole->ld);
		nGen = pole->ld;
		memcpy(pGen, pole->dane, pole->ld);
		if ( GOC_IS_DEBUG )
		{
			printf("test_009 [%d] [GOC_DEBUG]: Ciag przed usuwaniem: ",
				LICZBA_TESTOW);
			printBytes(pGen, nGen);
		}
		// usuwanie danych
		ld = goc_random(pole->ld); // ile udunac
		while ( ld-- )
		{
			int pos = goc_random(pole->ld);
			goc_nbitFieldCut(pole, pos);
			pGen = goc_tableRemove(pGen, &nGen, 1, pos);
		}
		if ( GOC_IS_DEBUG )
		{
			printf("test_009 [%d] [GOC_DEBUG]: Ciag testowy po usuwaniu: ", LICZBA_TESTOW);
			printBytes(pole->dane, pole->ld);
		}
		if ( memcmp(pole->dane, pGen, pole->ld) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledne dane\n" );
			GOC_FERROR( strFail );
		}
		pGen = goc_tableClear(pGen, &nGen);
		pole = goc_nbitFieldRemove( pole );
	}

	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_009 [STATS]: Time: %04d ticks\n", timertick);
	}
}

void test_010()
{
	// test na usuwanie ciagow wartosci od wylosowanej pozycji, a¿ usunie
	// ca³o¶æ
	// liczba bitow 8
	// liczba danych losowana
	// dane losowane
	//
	// usuwanie wszystkich wartosci
	int LICZBA_TESTOW = 100; // liczba przebiegow testu
	GOC_StNBitField *pole = NULL;
	int ld = 0; // liczba danych
	int lbnw = 8; // liczba bitow
	unsigned char *pGen = NULL; // ciag obliczany recznie
	_GOC_TABEL_SIZETYPE_ nGen = 0;

	srand(time(NULL));
	if ( GOC_IS_STATS )
		starttimer(timertick, sigtimeralarm);

	while ( LICZBA_TESTOW-- )
	{
		ld = 1+goc_random(255);
		pole = goc_nbitFieldCreate(lbnw, 0);

		// utworzenie wartosci bazowej
		while (ld--)
		{
			int val = goc_random((1<<lbnw));
			goc_nbitFieldAdd(pole, val);
		}
		pGen = malloc(pole->ld);
		nGen = pole->ld;
		memcpy(pGen, pole->dane, pole->ld);
		
		if ( GOC_IS_DEBUG )
		{
			printf("test_010 [%d] [GOC_DEBUG]: Ciag testowy przed usuwaniem: ", LICZBA_TESTOW);
			printBytes(pGen, nGen);
		}
		// usuwanie danych
		ld = pole->ld; // ile usunac
		while ( ld-- )
		{
			int pos = goc_random(pole->ld);
			goc_nbitFieldCut(pole, pos);
			pGen = goc_tableRemove(pGen, &nGen, 1, pos);
		}
		if ( GOC_IS_DEBUG )
		{
			printf("test_010 [%d] [GOC_DEBUG]: Ciag testowy po usuwaniu:", LICZBA_TESTOW);
			printBytes(pole->dane, pole->ld);
		}

		if ( memcmp(pole->dane, pGen, pole->ld) )
		{
			char *strFail = NULL;
			strFail = goc_stringAdd( strFail, "\nERROR: Otrzymano bledne dane\n" );
			GOC_FERROR( strFail );
		}
		pGen = goc_tableClear(pGen, &nGen);
		pole = goc_nbitFieldRemove( pole );
	}

	if ( GOC_IS_STATS )
	{
		stoptimer();
		printf("test_010 [STATS]: Time: %04d ticks\n", timertick);
	}
}

// Program przyjmuje flagi:
// -stats wypisuj informacje statystyczne
// -verbose wypisuj komunikaty verbose programu testowego
// -debug wypisuj informacje typue debug
int main(int argc, char **argv)
{
	
	int number_failed;

	if ( argc > 1 )
	{
		int i;
		for ( i=0; i<argc; i++ )
		{
			if ( goc_stringEquals(argv[i], "-stats") )
				flag |= GOC_FLAGSTATS;
			else if ( goc_stringEquals(argv[i], "-debug") )
				flag |= GOC_FLAGDEBUG;
		}
	}

	// convert to string i from string
//	tc_core = tcase_create ("To String/From String");
	test_001();
	test_002();
	test_003();
	test_004();

	// insert value
//	tc_core = tcase_create ("Insert value");
	test_005();
	test_008();

	// remove value
//	tc_core = tcase_create ("Remove value");
	test_006();
	test_007();
	test_009();
	test_010();
	
	return 0;
}
