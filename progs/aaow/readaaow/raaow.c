#include <malloc.h>
#include <stdio.h>

#define OUTFILE "data/campaign.out"
#define MAPFILE "data/campaign.11"

/*
 * 0000 - 0018 Title
 * 0019 - 0120 Memo
 * 0121 - 0137 ??? (ustawienia)
 * 0138 - 02C7 Map
 * 02C8 - 0457 Mapa frontu i zniszczen
 *     2 najstarsze bity mówi¹ o zniszczeniach - 00 brak/01 male/02 duze/03 zniszczone
 *     2 kolejne bity mowi¹ o froncie 01 red/02 green/03 neutral
 *     4 ostatnie bity, s¹ ustawione na 6 jesli jest to front (nieznane zastosowanie)
 * 0458 - 04BB polozenia pilotow
 * 04BC - 04ED strona pilota (1 green 2 red)
 * 04EE - 051F rodzaj samolotu (1 mysliwiec 2 bombowiec)
 * 0520 - 0551 Odznaczenia pilotów 1E to wszystkie mozliwe odznaczenia - 6 odznaczen (wartosci co 5, czyli zapewne po 5 zaslugach
 *		nastepuje przypisanie medalu)
 * 0552 - 0907 Piloci [jeden pilot po 16 znakow/moze 20]
 *
 * Maksymalna liczba samolotow 50
 *
 *
 * Ustawienia:
 * Play green/red side
 * Enemy seen always/when close/when far
 * Aircraft repair rate is fast/slow/medium
 * Target repair rate is fast/slow/medium
 * Mountain are low/high
 * Your Aircraft range is long/short/medium
 * Your factories build fighters often/never/seldom
 * Your factories build bombers often/never/seldom
 * Enemy Aircraft range is long/short/medium
 * Enemy factories build fighters often/never/seldom
 * Enemy factories build bombers often/never/seldom
 *
 * Oponents with different strategy (fisrt oponent/second oponent)
 * 
 */

// zidentyfikowane elementy - position
#define DATATITLE 0x0000
#define TITLELENGTH 24
#define DATAMEMO 0x0019
#define MEMOLINELENGTH 24
#define MEMOLINESAMMOUNT 11
#define DATAMAP 0x0138
#define DATADMG 0x02C8
#define MAPHEIGHT 20
#define MAPWIDTH 20
#define DATAPILOT 0x0552
#define PILOTNAMELENGHT 19
#define PILOTMAX 50
#define DATAORDERS 0x0520
#define DATAPOS 0x458
#define DATASIDE 0x04BC
#define DATATYPE 0x04EE

char mapDefs[] = {
	// city - b 0
	'c', 'c', 'c',
	// red capital - b 1
	'r',
	// green capital - b 2
	'g',
	// airbase - b 3
	'a',
	// factory - b 4
	'f',
	// supply depot - b 5
	's',
	// fort - b 6
	'F',
	// village - b 7
	'v',
	// bridge - b 8, 9
	'-', '|',
	// red side flag - none
	'$',
	// green side flag - none
	'#',
	//compass - none
	'+',
	// forrest (12) - t 0
	'T', 'T',  'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T',
	// forrest-clear - t 0
	 't', 't', 't',
	 // clear with single tree - t 1
	 '.', '.', '.', '.', '.', '.', '.', '.', 
	 // clear - t 1
	 ' ', ' ', ' ',
	 // river (19) - t 2
	 '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', 
	 // mountain (15) - t 3
	 '^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^',
	 // damaged clear - none
	 '+', '+', '+',
	 // Damages forest - none
	 '*', '*', '*',
	 // Unknown - none
	 'X', 'X', 'X', 'X',
	 // Damages forest - none
	 '*'

}; // 00 - 55
// d 0, 1, 2, 3
char dmgDefs[] = {
	' ', '.', '+', '*'
};

// f 0, 1, 2, 3
char frontDefs[] = {
	' ', 'R', 'G', '|'
};

// d - 2b
// f - 2b
// t - 2b
// b - 4b

FILE *out = NULL;
FILE *plik = NULL;

void printString(char *str, int len, int newLine)
{
	int i=0;
	int j=0;
	for ( i=0; i<len; i++ , j++)
	{
		if ( j % newLine == 0 )
			printf("\n");
		if (( str[i] >= 32 ) && ( str[i] < 'z' ))
			fputc(str[i], out);
		else
			fprintf(out, " %02x", (int)str[i]);
	}
}

void printMap(unsigned char *str, int len, int newLine, char *pDefs, int nDefs)
{
	int i=0;
	int j=0;
	for ( i=0; i<len; i++ , j++)
	{
		if ( j % newLine == 0 )
			printf("\n");
		if ( str[i] < nDefs )
			fputc(pDefs[str[i]], out);
		else
			fputc('X', out);
	}
}

void readTitle()
{
	char title[TITLELENGTH];
	fseek(plik, DATATITLE, 0);
	fread(title, 1, TITLELENGTH, plik);
	fprintf(out, "Title:[");
	printString(title, TITLELENGTH, TITLELENGTH);
	fprintf(out, "]\n");
}

void readMemo()
{
	char memo[MEMOLINELENGTH * MEMOLINESAMMOUNT];
	fseek(plik, DATAMEMO, 0);
	fread(memo, 1, MEMOLINELENGTH * MEMOLINESAMMOUNT, plik);
	fprintf(out, "Memo:[");
	printString(memo, MEMOLINELENGTH * MEMOLINESAMMOUNT, MEMOLINELENGTH);
	fprintf(out, "]\n");
}

void readMap()
{
	unsigned char map[MAPWIDTH * MAPHEIGHT];
	fseek(plik, DATAMAP, 0);
	fread(map, 1, MAPWIDTH * MAPHEIGHT, plik);
	fprintf(out, "Map:[");
	printMap(map, MAPWIDTH * MAPHEIGHT, MAPWIDTH, mapDefs, sizeof(mapDefs));
	fprintf(out, "]\n");
}

void readDmg()
{
	unsigned char map[MAPWIDTH * MAPHEIGHT];
	int i;
	fseek(plik, DATADMG, 0);
	fread(map, 1, MAPWIDTH * MAPHEIGHT, plik);
	fprintf(out, "Damages:[");
	for (i=0; i<MAPWIDTH * MAPHEIGHT; i++)
		map[i] >>= 6;
	printMap(map, MAPWIDTH * MAPHEIGHT, MAPWIDTH, dmgDefs, sizeof(dmgDefs));
	fprintf(out, "]\n");
}

void readFront()
{
	unsigned char map[MAPWIDTH * MAPHEIGHT];
	int i;
	fseek(plik, DATADMG, 0);
	fread(map, 1, MAPWIDTH * MAPHEIGHT, plik);
	fprintf(out, "Front:[");
	for (i=0; i<MAPWIDTH * MAPHEIGHT; i++)
	{
		map[i] >>= 4;
		map[i] &= 0x03;
	}
	printMap(map, MAPWIDTH * MAPHEIGHT, MAPWIDTH, frontDefs, sizeof(frontDefs));
	fprintf(out, "]\n");
}

void readPilots()
{
	char pilot[PILOTNAMELENGHT];
	unsigned char orders[PILOTMAX];
	unsigned short pos[PILOTMAX];
	unsigned char side[PILOTMAX];
	unsigned char type[PILOTMAX];
	int i;
	fseek(plik, DATAORDERS, 0);
	fread(orders, 1, PILOTMAX, plik);

	fseek(plik, DATAPOS, 0);
	fread(pos, 2, PILOTMAX, plik);

	fseek(plik, DATASIDE, 0);
	fread(side, 1, PILOTMAX, plik);
	
	fseek(plik, DATATYPE, 0);
	fread(type, 1, PILOTMAX, plik);

	fseek(plik, DATAPILOT, 0);
	fprintf(out, "Piloci:[");
	for ( i=0; i<PILOTMAX; i++ )
	{
		fread(pilot, 1, PILOTNAMELENGHT, plik);
		printString(pilot, PILOTNAMELENGHT, PILOTNAMELENGHT);
		fprintf(out, "[%d]\n\t[Orders: %02x][position: %02d,%02d][side:%s][type:%s]", i, orders[i]/5, pos[i]%20, (pos[i]/20)-20,
			(side[i] == 1 ? "green": "red"), (type[i] == 1 ? "fighter" : "bomber"));
	}
	fprintf(out, "]\n");
}

int main(int argc, char **argv)
{
	if ( argc < 2 )
	{
		printf("Usage: %s [plik]\n", argv[0]);
		printf("\tProgram dokonuje konwersji plików kampanii z gry\n");
		printf("\tAncient Art Of War - Skies na pliki w formacie\n");
		printf("\tdostêpnym dla gry Text AAOW\n");
		return 0;
	}
	plik = fopen(argv[1], "rb");
	out = stdout;

	readTitle();
	readMemo();
	readMap();
	readDmg();
	readFront();
	readPilots();

	fclose(plik);
	return 0;
}
