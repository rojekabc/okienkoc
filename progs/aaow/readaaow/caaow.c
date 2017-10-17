#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <okienkoc/mystr.h>
#include <okienkoc/okienkoc.h>
/*
#include "gen/gamedata.h"
#include "gen/build.h"
#include "gen/city.h"
#include "gen/capital.h"
#include "gen/factory.h"
#include "gen/fort.h"
#include "gen/supply.h"
#include "gen/airport.h"
#include "gen/village.h"
#include "gen/bridgeh.h"
#include "gen/bridgev.h"
#include "gen/fly.h"
#include "gen/bomber.h"
#include "gen/fighter.h"
*/
#include "aaow.h"

// TODO: Wykorzystaæ po³o¿enie flag do zorientowania stron
// konfliktu i dobarwienia budunków znajduj±cych siê na mapie
//
// TODO: Rozpisanie mapy na puste przerwy pomiêdzy kolumnami

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

#define GREEN_FLAG 13
#define RED_FLAG 12
#define GREEN_BASE 4
#define RED_BASE 3

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
	// clear with single tree (8) - t 1
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

// d - 2b (damages)
// f - 2b (front line)
// t - 2b (terrain)
// b - 4b (buildings)
//
// definicje mapy dla przeksztalcenia
// 0x00 - trees
// 0x01 - clear terrain
// 0x02 - river
// 0x03 - mountains
//
unsigned char terrainDataDefs[] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 10 clear
	0x02, 0x02, // 2 river
	0x01, 0x01, 0x01, // 3 clear
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 15 forest
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 11 clear
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, // 19 river
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, // 15 mountain
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 // 11 clear
};

unsigned char buildDataDefs[] = {
	0x03, 0x03, 0x03, // city
	0x07, 0x07, // capital (red/green)
	0x04, // airport
	0x02, // factory
	0x01, // supply
	0x06, // fort
	0x05, // village
	0x08, // bridge horizontal
	0x09 // bridge vertical
};

unsigned char nation[MAPWIDTH][MAPHEIGHT];

FILE *out = NULL;
FILE *plik = NULL;

StGameData *gameData = NULL;
GOC_HANDLER maska = 0;

void readTitle()
{
	char title[TITLELENGTH+1];
	memset(title, 0, TITLELENGTH+1);
	fseek(plik, DATATITLE, 0);
	fread(title, 1, TITLELENGTH, plik);
	gameData->title = goc_stringCopy(gameData->title, title);
}

void readMemo()
{
	char memo[MEMOLINELENGTH * MEMOLINESAMMOUNT +1];
	memset(memo, 0, MEMOLINELENGTH * MEMOLINESAMMOUNT +1);
	fseek(plik, DATAMEMO, 0);
	fread(memo, 1, MEMOLINELENGTH * MEMOLINESAMMOUNT, plik);
	gameData->description = goc_stringCopy(gameData->description, memo);
}

#define TOGOC(variable) (GOC_StValuePoint*)(((char*)variable)+4)
#define FROMGOC(variable) (((char*)variable)-4)

static int nasluchBuild(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_MAPGETCHAR_ID )
	{
		// here is our own organization
		GOC_StMsgMapChar* msgchar = (GOC_StMsgMapChar*)msg;
		// goc_systemDefaultAction(uchwyt, msg);
		switch ( msgchar->position &= 0x0F ) {
			case 0x00: msgchar->charcode = 0; break;
			case 0x01: msgchar->charcode = 'S'; break;
			case 0x02: msgchar->charcode = 'f'; break;
			case 0x03: msgchar->charcode = 'O'; break;
			case 0x04: msgchar->charcode = 'L'; break;
			case 0x05: msgchar->charcode = 'o'; break;
			case 0x06: msgchar->charcode = '#'; break;
			case 0x07: msgchar->charcode = '0'; break;
			case 0x08: msgchar->charcode = '-'; break;
			case 0x09: msgchar->charcode = '|'; break;
		}
		switch (msgchar->position &=0xF0) {
			case 0x10: msgchar->charcolor=GOC_GREEN | GOC_FBOLD; break;
			case 0x20: msgchar->charcolor=GOC_RED | GOC_FBOLD; break;
			default: msgchar->charcolor=GOC_WHITE; break;
		}
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_POSMAPFREEPOINT_ID )
	{
		/* TODO FREE
		   switch ( punkt->value )
		   {
		   case 0x03:
		   cityFree((StCity*)punkt);
		   break;
		   case 0x07:
		   capitalFree((StCapital*)punkt);
		   break;
		   case 0x04:
		   airportFree((StAirport*)punkt);
		   break;
		   case 0x02:
		   factoryFree((StFactory*)punkt);
		   break;
		   case 0x01:
		   supplyFree((StSupply*)punkt);
		   break;
		   case 0x06:
		   fortFree((StFort*)punkt);
		   break;
		   case 0x05:
		   villageFree((StVillage*)punkt);
		   break;
		   case 0x08:
		   bridgehFree((StBridgeH*)punkt);
		   break;
		   case 0x09:
		   bridgevFree((StBridgeV*)punkt);
		   break;
		   default:
		   goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
		   break;
		   }
		   */
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

// Algorytm koloryzowania narodowo¶ci terenów i budynków
// Doprowadzane s± do trzech stanów - green / red / neutral
// Neutralne znajduj± siê na froncie
static void colorNation(int x, int y, unsigned char value)
{
	if ( x < 0 )
		return;
	if ( y < 0 )
		return;
	if ( x >= MAPWIDTH )
		return;
	if ( y >= MAPHEIGHT )
		return;


	// okre¶lenie zmiany orientacji
	if ( value & 0x01 )
	{
		// natrafiono na liniê frontu
		if ( nation[x][y] == 0x00 )
			return;
		if ( nation[x][y] != 0x80 )
			return;
		// wykonaj pomost frontu
		value &= 0xF0;
		if ( value == 0x10 )
			value = 0x20;
		else if ( value == 0x20 )
			value = 0x10;
	}

	// warto¶æ nieustalona - czas j± okre¶liæ
	if ( nation[x][y] == 0x80 )
	{
		nation[x][y] = value;

		colorNation(x, y-1, value);
		colorNation(x, y+1, value);

		colorNation(x+1, y-1, value);
		colorNation(x+1, y, value);
		colorNation(x+1, y+1, value);

		colorNation(x-1, y-1, value);
		colorNation(x-1, y, value);
		colorNation(x-1, y+1, value);
	}
	// wykonanie pomostowania zmiennego przez front
	// jak siê okazuje nie zawsze s± wszystkie flagi
	// albo przej¶cia nastêpuj± poza lini± frontu
	else if ( nation[x][y] == 0x00 )
	{
		// czy wystêpuje orientacja pionowa frontu
		if (( y == 0 ) || ( y == MAPHEIGHT-1 ))
			return;
		// warto¶ci musz± byæ ró¿ne od frontu, co oznacza
		// mo¿liwo¶æ wykonania przeskoku
		if ( nation[x][y-1] && nation[x][y+1] )
		{
			if (nation[x][y-1] != 0x80)
				colorNation(x, y+1, nation[x][y-1] | 0x01 );
			if (nation[x][y+1] != 0x80)
				colorNation(x, y-1, nation[x][y+1] | 0x01 );
			return;
		}
		// sprawdzenie orientacji poziomej frontu
		if (( x == 0 ) || ( x == MAPWIDTH-1 ))
			return;
		// warto¶ci musz± byæ ró¿ne od frontu, co oznacza
		// mo¿liwo¶æ wykonania przeskoku
		if ( nation[x-1][y] && nation[x+1][y] )
		{
			if (nation[x+1][y] != 0x80)
				colorNation(x-1, y, nation[x+1][y] | 0x01 );
			if (nation[x-1][y] != 0x80)
				colorNation(x+1, y, nation[x-1][y] | 0x01 );
		}
		// przej¶cie lewo-prawo skro¶ne
		if ( nation[x-1][y-1] && nation[x+1][y+1] && (!nation[x-1][y]) && (!nation[x][y-1]) )
		{
			if (nation[x+1][y+1] != 0x80)
				colorNation(x-1, y-1, nation[x+1][y+1] | 0x01 );
			if (nation[x-1][y-1] != 0x80)
				colorNation(x+1, y+1, nation[x-1][y-1] | 0x01 );
		}
		// przej¶cie prawo-lewo skro¶ne
		if ( nation[x-1][y+1] && nation[x+1][y-1] && (!nation[x+1][y]) && (!nation[x][y-1]) )
		{
			if (nation[x+1][y-1] != 0x80)
				colorNation(x-1, y+1, nation[x+1][y-1] | 0x01 );
			if (nation[x-1][y+1] != 0x80)
				colorNation(x+1, y-1, nation[x-1][y+1] | 0x01 );
		}
	}
}

GOC_HANDLER terrain = 0;
GOC_HANDLER build = 0;
GOC_HANDLER front = 0;


void readMap()
{
	unsigned char map[MAPWIDTH * MAPHEIGHT];
	fseek(plik, DATAMAP, 0);
	fread(map, 1, MAPWIDTH * MAPHEIGHT, plik);

	gameData->mapwidth = MAPWIDTH;
	gameData->mapheight = MAPHEIGHT;
	// budowanie mapy terenu
	if ( terrain == 0 )
	{
		terrain = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
				GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);
	}
	// definiuj wartosci
	goc_maparawSetBPV(terrain, 2); // 2 bity na warto¶æ
	goc_maparawAddChar(terrain, 't', GOC_GREEN, 0);
	goc_maparawAddChar(terrain, '.', GOC_WHITE, 1);
	goc_maparawAddChar(terrain, '=', GOC_CYAN, 2);
	goc_maparawAddChar(terrain, '^', GOC_RED, 3);
	// nano¶ warto¶ci terenu
	{
		int i=0;
		int j=0;
		int nDefs = sizeof(terrainDataDefs);
		int v;

		for ( j=0; j<MAPHEIGHT; j++ )
		{
			for ( i=0; i<MAPWIDTH; i++)
			{
				v = map[j*MAPWIDTH + i];
				if ( v < nDefs )
					goc_maparawSetPoint(terrain, i, j, terrainDataDefs[v]);
				else
					goc_maparawSetPoint(terrain, i, j, 0x01); // clear
			}
		}
		{
			// serializowanie danych
			GOC_StMapRaw *map = (GOC_StMapRaw*)terrain;
			gameData->terrain = goc_nbitFieldToString(map->dane);
		}
	}
	// budowanie mapy budynków
	if ( build == 0 )
	{
		build = goc_elementCreate(GOC_ELEMENT_POSMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
				GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);
	}
	// definiuj wartosci
	/*
	   goc_mapaposAddChar(build,  0 , GOC_WHITE, 0x00); // nic - don't add!
	   goc_mapaposAddChar(build, 'S', GOC_WHITE, 0x01); // supply
	   goc_mapaposAddChar(build, 'f', GOC_WHITE, 0x02); // factory
	   goc_mapaposAddChar(build, 'O', GOC_WHITE, 0x03); // city
	   goc_mapaposAddChar(build, 'L', GOC_WHITE, 0x04); // airport
	   goc_mapaposAddChar(build, 'o', GOC_WHITE, 0x05); // village
	   goc_mapaposAddChar(build, '#', GOC_WHITE, 0x06); // fort
	   goc_mapaposAddChar(build, '0', GOC_WHITE, 0x07); // capital
	   goc_mapaposAddChar(build, '-', GOC_WHITE, 0x08); // bridge
	   goc_mapaposAddChar(build, '|', GOC_WHITE, 0x09); // bridge
	   */
	// ustal mapê narodowosci
	{
		int i=0;
		int j=0;
		int v;

		for ( j=0; j<MAPHEIGHT; j++ )
		{
			for ( i=0; i<MAPWIDTH; i++)
			{
				v = map[j*MAPWIDTH + i];
				if (( v == GREEN_FLAG ) || ( v == GREEN_BASE ))
				{
					colorNation(i, j, 0x10);
				}
				else if (( v == RED_FLAG ) || ( v == RED_BASE ))
				{
					colorNation(i, j, 0x20);
				}

			}
		}
	}
	// nanos budynki
	{
		int i=0;
		int j=0;
		int nDefs = sizeof(buildDataDefs);
		int v;

		for ( j=0; j<MAPHEIGHT; j++ )
		{
			for ( i=0; i<MAPWIDTH; i++)
			{
				v = map[j*MAPWIDTH + i];
				if ( v < nDefs )
				{
					StBuild *punkt;
					switch ( buildDataDefs[v] )
					{
						case 0x03:
							punkt = (StBuild*)fobAlloc(cn_City);
							punkt->value = 3;
							break;
						case 0x07:
							punkt = (StBuild*)fobAlloc(cn_Capital);
							punkt->value = 7;
							break;
						case 0x04:
							punkt = (StBuild*)fobAlloc(cn_Airport);
							punkt->value = 4;
							break;
						case 0x02:
							punkt = (StBuild*)fobAlloc(cn_Factory);
							punkt->value = 2;
							break;
						case 0x01:
							punkt = (StBuild*)fobAlloc(cn_Supply);
							punkt->value = 1;
							break;
						case 0x06:
							punkt = (StBuild*)fobAlloc(cn_Fort);
							punkt->value = 6;
							break;
						case 0x05:
							punkt = (StBuild*)fobAlloc(cn_Village);
							punkt->value = 5;
							break;
						case 0x08:
							punkt = (StBuild*)fobAlloc(cn_BridgeH);
							punkt->value = 8;
							break;
						case 0x09:
							punkt = (StBuild*)fobAlloc(cn_BridgeV);
							punkt->value = 9;
							break;
						default:
							punkt = (StBuild*)fobAlloc(cn_Build);
							punkt->value = buildDataDefs[v];
							break;
					}
					punkt->x = i;
					punkt->y = j;
					punkt->value |= nation[i][j];
					punkt->flag = nation[i][j];
					goc_mapaposAddPoint(build,TOGOC(punkt));
					gameData->pbuild = goc_tableAdd(gameData->pbuild, &gameData->nbuild, sizeof(void*));
					gameData->pbuild[gameData->nbuild-1] = punkt;
				}
			}
		}
	}
	// ustaw funkcje nasluchujaca
	goc_elementSetFunc(build, GOC_FUNID_LISTENER, nasluchBuild);
}

void readDmg()
{
	unsigned char map[MAPWIDTH * MAPHEIGHT];
	int i, j;

	fseek(plik, DATADMG, 0);
	fread(map, 1, MAPWIDTH * MAPHEIGHT, plik);
	for (j=0; j<MAPHEIGHT; j++)
	{
		for (i=0; i<MAPWIDTH; i++)
		{
			GOC_StValuePoint* vp = NULL;
			map[j*MAPWIDTH + i] >>= 6;
			vp = goc_mapaposReadPoint(build, i, j);
			if (vp != NULL)
			{
				StBuild* fbuild = (StBuild*)FROMGOC(vp);
				fbuild->live -= 25*map[j*MAPWIDTH + i];
			}
		}
	}
}

void readFront()
{
	unsigned char map[MAPWIDTH * MAPHEIGHT];
	int i;

	fseek(plik, DATADMG, 0);
	fread(map, 1, MAPWIDTH * MAPHEIGHT, plik);
	for (i=0; i<MAPWIDTH * MAPHEIGHT; i++)
	{
		map[i] >>= 4;
		map[i] &= 0x03;
	}

	// budowanie mapy frontu
	if ( front == 0 )
	{
		front = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
				GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);
	}

	// definiuj wartosci
	goc_maparawSetBPV(front, 2); // 2 bity na warto¶æ
	goc_maparawAddChar(front, 0, GOC_WHITE, 0);
	goc_maparawAddChar(front, 'x', GOC_RED | GOC_FBOLD, 1);
	goc_maparawAddChar(front, 'x', GOC_GREEN | GOC_FBOLD, 2);
	goc_maparawAddChar(front, 'x', GOC_CYAN | GOC_FBOLD, 3);
	// nano¶ warto¶ci frontu
	{
		int i=0;
		int j=0;
		int v;

		for ( j=0; j<MAPHEIGHT; j++ )
		{
			for ( i=0; i<MAPWIDTH; i++)
			{
				v = map[j*MAPWIDTH+i];
				goc_maparawSetPoint(front, i, j, v );
				// ustawienie narodowo¶ci na neutraln±
				if ( v != 0x00 )
				{
					nation[i][j] = 0x00;
				}
			}
		}
		{
			// serializowanie danych
			GOC_StMapRaw *map = (GOC_StMapRaw*)front;
			gameData->front = goc_nbitFieldToString(map->dane);
		}
	}
	// ustal w³asno¶æ
}

void readPilots()
{
	char pilot[PILOTNAMELENGHT +1];
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

	for ( i=0; i<PILOTMAX; i++ )
	{
		GOC_StValuePoint *vp = NULL;
		StFly* fly = NULL;
		StAirport* airport = NULL;
		memset(pilot, 0, PILOTNAMELENGHT+1);
		fread(pilot, 1, PILOTNAMELENGHT, plik);

		if ( type[i] == 1 )
		{
			// fighter
			fly = (StFly*)fobAlloc(cn_Fighter);
		}
		else
		{
			// bomber
			fly = (StFly*)fobAlloc(cn_Bomber);
		}
		fly->pilotName = goc_stringCopy(fly->pilotName, pilot);
		fly->pilotExp = orders[i];
		fly->live = 100;
		if ( side[i] == 1 )
			fly->flag = 0x10;
		else
			fly->flag = 0x20;
		fly->fuel = 0; // TODO:

		// Find and add to an airport
		vp = goc_mapaposReadPoint(build, pos[i]%20, pos[i]/20-20);
		if ( vp == NULL )
			continue;
		airport = (StAirport*)FROMGOC(vp);
		airport->pfly = goc_tableAdd(airport->pfly, &airport->nfly,
				sizeof(StFly*));
		airport->pfly[airport->nfly-1] = fly;
	}
}

GOC_HANDLER labelDesc;

int mapaNasluch(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CURSORCHANGE_ID )
	{
		GOC_StMsgMapPoint* msgmp = (GOC_StMsgMapPoint*)msg;
		char buf[80];
		GOC_StValuePoint *v;
		int p;
		goc_labelRemLines(labelDesc);
		p = goc_maparawGetPoint(terrain, msgmp->x, msgmp->y);
		switch ( p )
		{
			case 0:
				sprintf(buf, "Terrain: forrest" );
				break;
			case 1:
				sprintf(buf, "Terrain: clear" );
				break;
			case 2:
				sprintf(buf, "Terrain: river" );
				break;
			case 3:
				sprintf(buf, "Terrain: mountains" );
				break;
			default:
				sprintf(buf, "Terrain: unknown" );
				break;
		}
		goc_labelAddLine(labelDesc, buf);
		v = goc_mapaposReadPoint(build, msgmp->x, msgmp->y);
		if ( v != NULL )
		{
			switch ( v-> value & 0x0F )
			{
				case 0x0:
					sprintf(buf, "");
					break;
				case 0x1:
					sprintf(buf, "Build: Supply");
					break;
				case 0x2:
					sprintf(buf, "Build: Factory");
					break;
				case 0x3:
					sprintf(buf, "Build: City");
					break;
				case 0x4:
					sprintf(buf, "Build: Airport");
					break;
				case 0x5:
					sprintf(buf, "Build: Village");
					break;
				case 0x6:
					sprintf(buf, "Build: Fort");
					break;
				case 0x7:
					sprintf(buf, "Build: Capital");
					break;
				case 0x8:
				case 0x9:
					sprintf(buf, "Build: Bridge");
					break;
				default:
					sprintf(buf, "Build: unknown" );
					break;
			}
		}
		else
			sprintf(buf, "Build:");
		goc_labelAddLine(labelDesc, buf);
		if ( v != NULL ) {
			switch ( v->value & 0xF0 )
			{
				case 0x00:
					sprintf(buf, "Front:" );
					break;
				case 0x10:
					sprintf(buf, "Front: red power" );
					break;
				case 0x20:
					sprintf(buf, "Front: green power" );
					break;
				case 0x30:
					sprintf(buf, "Front: none power" );
					break;
				default:
					sprintf(buf, "Front: unknown" );
					break;
			}
		}
		goc_labelAddLine(labelDesc, buf);
		// Airport
		if (( v != NULL ) && ( v->value & 0x0F == 0x04 ))
		{
			StAirport *airport = (StAirport*)v;
			int i;
			for (i = 0; i < airport->nfly; i++ )
			{
				goc_labelAddLine(labelDesc,
						airport->pfly[i]->pilotName);
			}
		}
		goc_systemClearArea(labelDesc);
		GOC_MSG_PAINT( msgpaint );
		goc_systemSendMsg( labelDesc, msgpaint );
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

int main(int argc, char **argv)
{
	GOC_StMessage msg;
	char *outFilename = NULL;
	if ( argc < 2 )
	{
		printf("Usage: %s [plik]\n", argv[0]);
		printf("\tProgram dokonuje konwersji plików kampanii z gry\n");
		printf("\tAncient Art Of War - Skies na pliki w formacie\n");
		printf("\tdostêpnym dla gry Text AAOW\n");
		return 0;
	}
	plik = fopen(argv[1], "rb");
	outFilename = goc_stringCopy(NULL, argv[1]);
	outFilename = goc_stringAdd(outFilename, ".xml");
	out = fopen(outFilename, "wb");
	outFilename = goc_stringFree(outFilename);
	gameData = (StGameData*)fobAlloc(cn_GameData);

	maska = goc_elementCreate( GOC_ELEMENT_MASK, 1, 1, MAPWIDTH, MAPHEIGHT, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_maskCharSpace(maska, 2, 1);
	labelDesc = goc_elementCreate( GOC_ELEMENT_LABEL, 50, 2, 20, 20, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	readTitle();
	readMemo();
	// ustalenie narodowo¶ci na nieznan±
	memset(nation, 0x80, MAPWIDTH*MAPHEIGHT);
	// odczytanie linii frontu przed map±, aby móc potem
	// ustalaæ przynale¿no¶æ terytorialn± obiektów
	readFront();
	readMap();
	readDmg();
	readPilots();

	fclose(plik);
	goc_maskAddMap(maska, terrain);
	goc_maskAddMap(maska, build);
	goc_maskAddMap(maska, front);
	goc_elementSetFunc(maska, GOC_FUNID_LISTENER, mapaNasluch);
	// uzupelnianie danych w strukturze gameData
	fobSerialize((fobElement*)gameData, out);
	// gamedataSerialize(gameData, out);
	while (goc_systemCheckMsg( &msg ));
	// TODO: gamedataFree(gameData);
	fclose(out);
	return 0;
}
