#include "rmetronom.h"
#include <ao/ao.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#define GOC_PRINTINFO
//#define GOC_PRINTDEBUG
#define GOC_PRINTERROR
#include <tools/log.h>
#include <tools/mystr.h>
#include <tools/tablica.h>
#include <tools/screen.h>
#include <tools/term.h>

#define SAMPLE_RATE 44100
//#define SAMPLE_FORMAT AO_FMT_LITTLE
#define SAMPLE_FORMAT AO_FMT_NATIVE
#define SAMPLE_BITS 16
#define SAMPLE_CHANNELS 2

#define MAX_SHORT 32768.0

int aodriverid = -1;
ao_device *aodev = NULL;

#define BEAT_BASE 0
#define BEAT_ACCENT 1
#define BEAT_INTER 2
#define BEAT_QUIET 3

typedef struct
{
	float power;
	float freq;
	char *pFrame;
	unsigned int nFrame;
} Beat;

# define FLAG_PRINTCOUNT 1

typedef struct
{
	// tempo
	int tempo;
	// metrum
	int measure;
	// beat-y
	Beat* pBeat;
	_GOC_TABEL_SIZETYPE_ nBeat;
	// czas trwania tick-a
	float tdur;
	// czas grania w sekundach
	int pdur;
	// liczba bitów wewnêtrznych
	int inter;
	int flags;
	// liczba taktów do zagrania
	int tacts;
	// pattern do wygrywania
	char *pattern;
} Parameters;

Parameters* params;

// zajmij pamiêæ i przypisz domy¶lne warto¶ci
Parameters* allocParameters()
{
	Parameters* p = (Parameters*)malloc(sizeof(Parameters));
	memset(p, 0, sizeof(Parameters));
	if ( !p )
		GOC_ERROR("Cannot allocate Parameters");
	p->tempo = 60;
	p->pBeat = malloc(4*sizeof(Beat));
	p->nBeat = 4;
	memset(p->pBeat, 0, p->nBeat*sizeof(Beat));

	p->pBeat[BEAT_BASE].power = 0.75;
	p->pBeat[BEAT_BASE].freq = 880.00;
	p->pBeat[BEAT_ACCENT].power = 0.75;
	p->pBeat[BEAT_ACCENT].freq = 880.00;
	p->pBeat[BEAT_INTER].power = 0.75;
	p->pBeat[BEAT_INTER].freq = 880.00;
	p->pBeat[BEAT_QUIET].power = 0;
	p->pBeat[BEAT_QUIET].freq = 0.00;

	// p->freq = 880.0;
	// p->power = 0.75;
	p->tdur = 0.01;
	p->pdur = 3;
	p->measure = 4;
	p->inter = 0;
	p->flags = 0;
	p->tacts = 1;
	return p;
}

Parameters* freeParameters(Parameters* p)
{
	free(p->pBeat);
	free(p);
	return NULL;
}

int openOutputDevice()
{
	ao_sample_format aofrmt;

	memset( &aofrmt, 0, sizeof(ao_sample_format));
	aofrmt.bits = SAMPLE_BITS;
	aofrmt.rate = SAMPLE_RATE;
	aofrmt.channels = SAMPLE_CHANNELS;
	aofrmt.byte_format = SAMPLE_FORMAT;

	closeOutputDevice();
	aodev = ao_open_live(aodriverid, &aofrmt, NULL);
	if ( !aodev )
		return -1;
	return 0;
}

void closeOutputDevice()
{
	if ( aodev )
		ao_close(aodev);
	aodev = NULL;
}

void endProgram(int ret)
{
	params = freeParameters(params);

	GOC_DEBUG("Closing libao Device\n");
	closeOutputDevice();

	GOC_DEBUG("Closing libao\n");
	ao_shutdown();
//	goc_termRestore();
	exit(ret);
}

int argString(char** argv, int pos, int argc, char** string)
{
	*string = goc_stringCopy(NULL, argv[pos+1]);
	return 2;
}

int argCount(char** argv, int pos, int argc, int* argf)
{
	*argf |= FLAG_PRINTCOUNT;
	return 1;
}

int argInt(char** argv, int pos, int argc, int* argi)
{
	sscanf(argv[pos+1], "%d", argi);
	return 2;
}

int argBeatFreq(char** argv, int pos, int argc, Parameters* p)
{
	int bpos;
	float freq;

	if ( argc <= pos + 2 )
	{
		GOC_ERROR("Wrong arguments use");
		return -1;
	}

	sscanf(argv[pos+1], "%d", &bpos);
	sscanf(argv[pos+2], "%f", &freq);

	p->pBeat[bpos].freq = freq;

	return 3;
}

int argBeatPower(char** argv, int pos, int argc, Parameters* p)
{
	int bpos;
	float power;

	if ( argc <= pos + 2 )
	{
		GOC_ERROR("Wrong arguments use");
		return -1;
	}

	sscanf(argv[pos+1], "%d", &bpos);
	sscanf(argv[pos+2], "%f", &power);
	p->pBeat[bpos].power = power;

	return 3;
}

int argHelp(char** argv, int pos, int argc, void* param)
{
	printf("Usage: %s [parameters]\n\n", argv[0]);
	printf("List of parameters:\n");
	printf("\t-t <int>\tSet tempo [beats per minute] (Default: 60)\n");
	printf("\t-f 0 <float>\tSet frequency of base beat [Hz] (Default: 880.00)\n");
	printf("\t-f 1 <float>\tSet frequency of accented first beat [Hz] (Default: 880.00)\n");
	printf("\t-f 2 <float>\tSet frequency of intermidded beat [Hz] (Default: 880.00)\n");
	printf("\t-p 0 <float>\tSet power of base beat [0..1] (Default: 0.75)\n");
	printf("\t-p 1 <float>\tSet power of accented first beat [0..1] (Default: 0.75)\n");
	printf("\t-p 2 <float>\tSet power of intermidded beat [0..1] (Default: 0.00)\n");
	printf("\t-dp <int>\tHow long to play [s] (Default: 3)\n");
	printf("\t-db <float>\tHow long to play one beat [s] (Default: 0.01)\n");
	printf("\t-m <int>\tSet a measure (Default: 4)\n");
	printf("\t-i <int>\tSet intermediate beats (Default: 0)\n");
	printf("\t-c\t\tEnable counting visually (Default: disabled)\n");
	printf("\t-n <int>\tSet number of tacts to play - disable -dp switch (Default: 0)\n");
	printf("\t-h\t\tPrint this help\n");

	endProgram(0);
	return 1;
}

typedef int (*PrgArgFun)(char **, int, int, void*);
typedef struct
{
	const char* argName;
	// zwraca liczbe zinterpretowanych argumentow
	// argumenty: argv, pos, argc, modifiedParameter
	PrgArgFun argFunction; //int (*argFunction)(char**, int, int, void*);
	// ustawiany parametr
	void* parameter;
} ProgramArgument;

int interpretArgs(int argc, char** argv, Parameters* p)
{
	ProgramArgument arguments[] =
	{
		{"-h", (PrgArgFun)argHelp, NULL},
		{"--help", (PrgArgFun)argHelp, NULL},
		{"-?", (PrgArgFun)argHelp, NULL},
		{"-t", (PrgArgFun)argInt, &p->tempo},
		{"-dp", (PrgArgFun)argInt, &p->pdur},
		{"-db", (PrgArgFun)argInt, &p->tdur},
		{"-f", (PrgArgFun)argBeatFreq, p},
		{"-p", (PrgArgFun)argBeatPower, p},
		{"-i", (PrgArgFun)argInt, &p->inter},
		{"-m", (PrgArgFun)argInt, &p->measure},
		{"-c", (PrgArgFun)argCount, &p->flags},
		{"-n", (PrgArgFun)argInt, &p->tacts},
		{"--pattern", (PrgArgFun)argString, &p->pattern}
	};

	int i = 1;
	int j;
	int nj = sizeof(arguments)/sizeof(ProgramArgument);

	while ( i < argc )
	{
		for (j=0; j<nj; j++)
	       	{
			if ( goc_stringEquals(arguments[j].argName, argv[i]) )
			{
				int off = arguments[j].argFunction(argv, i, argc, arguments[j].parameter);
				if ( off < 0 )
					return off;
				i += off;
				break;
			}
		}
		if ( j == nj )
		{
			GOC_BERROR("Unknown argument '%s'", argv[i]);
			argHelp(argv, i, argc, NULL);
			break;
		}
	}
	return 0;
}

void generateBeat(Parameters* params, int ibeat)
{
	// wyznaczanie wielko¶ci pojedynczej ramki d¼wieku w bajtach, jest to
	// 	liczba bajtów na jedn± próbê
	// 	* liczba próbek dla jednej ramki
	// liczba bajtów na jedn± próbê to
	// 	liczba bajtów na kana³
	// 	* liczba kana³ów
	// liczba próbek dla jednej ramki
	// 	liczba próbek  na 1 sekundê (tzw. rate)
	// 	* liczba sekund w minucie (60)
	// 	/ tempo
	int nsamples = SAMPLE_RATE * 60 / params->tempo / (params->inter+1);
	int nFrame = SAMPLE_BITS / 8 * SAMPLE_CHANNELS * nsamples;
	// warto¶æ próbki
	int sample;
	// moment w czasie w ramce
	float moment;
	// licznik
	int i;

	params->pBeat[ibeat].nFrame = nFrame;
	params->pBeat[ibeat].pFrame = malloc(nFrame);
	GOC_BINFO("Generate beat %d with params: duration: %.2fs freq: %.2fHz power: %.2f frames: %d",
		ibeat, params->tdur, params->pBeat[ibeat].freq, params->pBeat[ibeat].power, params->pBeat[ibeat].nFrame);

	// wyznaczenie w próbce danych na czas trwania ticku w danym momencie
	// Jedna sekunda i/SAMPLE_RATE to moment w czasie od 0..1 sec
	for ( i=0; i<nsamples; i++ )
	{
		// moment [sec] w ramce, któr± budujemy
		moment = (float)i/SAMPLE_RATE;
		if ( moment > params->tdur )
			break; // cisza
		// i/SAMPLE_RATE to moment dla przebiegu fali o danej czêstotliwo¶ci
		// w stosunku do pe³nej jednosekundowej próbki, w którym
		// zostaje pobrana próbka
		sample = (int)(params->pBeat[ibeat].power*MAX_SHORT*sin(2*M_PI*params->pBeat[ibeat].freq*((float)i/SAMPLE_RATE)));
		// DLA 16 bitowego dwukana³owego
		params->pBeat[ibeat].pFrame[i<<2] = params->pBeat[ibeat].pFrame[(i<<2)+2] = sample & 0xFF;
		params->pBeat[ibeat].pFrame[(i<<2)+1] = params->pBeat[ibeat].pFrame[(i<<2)+3] = (sample>>8) & 0xFF;
	}

}

#define NEXTPATTERN() \
		if ( params->pattern ) \
		{ \
			playedPattern = params->pattern[patternPosition]; \
			patternPosition++; \
			if ( patternPosition >= patternLen ) \
				patternPosition = 0; \
		}

int main(int argc, char** argv)
{
	// zak³ada siê, ¿e ramka zawiera próbkê, w której
	// jest d¼wiêk beat-u i cisza do nastêpnego beatu
	// 
	//
	// ramka
	//char *pFrame;
	// licznik
	int i;
//	goc_termInit();
	printf(GOC_STRING_WHITE);

	// Procedury inicjuj±ce
	GOC_DEBUG("Initializing libao");
	ao_initialize();

	GOC_DEBUG("Initializing libao Device");
	aodriverid = ao_default_driver_id();
	if ( aodriverid == -1 )
		return -1;
	openOutputDevice();

	params = allocParameters();

	if ( argc > 1 )
	{
		if ( interpretArgs(argc, argv, params) < 0 )
			endProgram(-1);
	}


	GOC_DEBUG("Allocating buffer");

	generateBeat(params, BEAT_BASE);
	generateBeat(params, BEAT_ACCENT);
	generateBeat(params, BEAT_INTER);
	generateBeat(params, BEAT_QUIET);

//	pFrame = malloc(nFrame);
//	memset(pFrame, 0, nFrame);
	GOC_BINFO("Tempo: %d Rate: %d Channels: %d Bits: %d",
			params->tempo, SAMPLE_RATE, SAMPLE_CHANNELS, SAMPLE_BITS);
//	GOC_BDEBUG("Allocated %d at 0x%X", nFrame, (unsigned int)pFrame);
	if ( params->flags & FLAG_PRINTCOUNT )
	{
		goc_clearscreen();
		goc_gotoxy(1,1);
	}

	printf("Playing ... \n");
	if ( params->pattern )
		printf("Use pattern: %s\n", params->pattern);
	fflush(stdout);
	{
		// to nie do koñca jest liczba ramek - to jest liczba
		// podstawowych beatów jakie maj± zostaæ zagrane - nie
		// s± liczone beat-y po¶rednie
		int nFrames;
		int imeasure = 0;
		int iinter;
		int patternPosition = 0;
		int patternLen = 0;

		char *prevString = goc_stringCopy(NULL, "");
		char playedPattern = '.';

		if ( params->pattern )
		{
			patternLen = strlen(params->pattern);
		}

		if ( params->tacts )
			// na podstawie liczby podanych taktów
			nFrames = params->tacts * params->measure;
		else
			// na podstawie podanego czasu
			nFrames = params->tempo * params->pdur / 60;

		NEXTPATTERN();

		// goc_saveXY();
		for (i=0; i<nFrames; i++)
		{
			if ( imeasure )
			{
				if ( params->flags & FLAG_PRINTCOUNT )
				{
					goc_gotoxy(1, 5);
					printf("%s%s%s%d%s", GOC_STRING_YELLOW, prevString, GOC_STRING_BWHITE, imeasure + 1, GOC_STRING_YELLOW);
					prevString = goc_stringAddInt(prevString, imeasure+1);
					prevString = goc_stringAdd(prevString, " .. ");
					fflush(stdout);
				}
				if ( playedPattern == '.' )
					ao_play(aodev, params->pBeat[BEAT_BASE].pFrame, params->pBeat[BEAT_BASE].nFrame);
				else
					ao_play(aodev, params->pBeat[BEAT_QUIET].pFrame, params->pBeat[BEAT_QUIET].nFrame);
				NEXTPATTERN();
			}
			else
			{
				if ( params->flags & FLAG_PRINTCOUNT )
				{
					goc_gotoxy(1, 5);
					printf("%s%s", GOC_STRING_YELLOW, prevString);
					goc_gotoxy(1, 5);
					printf("%s%d%s", GOC_STRING_BWHITE, imeasure + 1, GOC_STRING_YELLOW);
					prevString = goc_stringFree(prevString);
					prevString = goc_stringAddInt(prevString, imeasure+1);
					prevString = goc_stringAdd(prevString, " .. ");

					fflush(stdout);
				}
				if ( playedPattern == '.' )
					ao_play(aodev, params->pBeat[BEAT_ACCENT].pFrame, params->pBeat[BEAT_ACCENT].nFrame);
				else
					ao_play(aodev, params->pBeat[BEAT_QUIET].pFrame, params->pBeat[BEAT_QUIET].nFrame);
				NEXTPATTERN();
			}
			if ( (iinter = params->inter) )
			{
				while  ( iinter-- )
				{
					if ( params->flags & FLAG_PRINTCOUNT )
					{
						goc_gotoxy(1, 5);
						printf("%s%s%si%s", GOC_STRING_YELLOW, prevString, GOC_STRING_WHITE, GOC_STRING_YELLOW);
						prevString = goc_stringAdd(prevString, "i .. ");
						fflush(stdout);
					}
					if ( playedPattern == '.' )
						ao_play(aodev, params->pBeat[BEAT_INTER].pFrame, params->pBeat[BEAT_INTER].nFrame);
					else
						ao_play(aodev, params->pBeat[BEAT_QUIET].pFrame, params->pBeat[BEAT_QUIET].nFrame);
					NEXTPATTERN();
				}
			}
			imeasure++;
			imeasure %= params->measure;
		}
		if ( params->flags & FLAG_PRINTCOUNT )
		{
			goc_gotoxy(1, 5);
			printf("%s%s", GOC_STRING_YELLOW, prevString);
		}
		prevString = goc_stringFree(prevString);
	}
	printf("%s\nFinish\n", GOC_STRING_WHITE);

	if ( params->tacts )
	{
		GOC_BINFO("Played %d frames in %d tacts", i, params->tacts);
	}
	else
	{
		GOC_BINFO("Played %d frames in %d seconds", i, params->pdur);
	}

	endProgram(0);
	return 0;
}
