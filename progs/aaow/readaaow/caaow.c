#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <tools/mystr.h>
#define GOC_PRINTERROR
#include <tools/log.h>
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
#include "original.h"
#include "common.h"

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

void readMap()
{
	unsigned char map[MAPWIDTH * MAPHEIGHT];
	fseek(plik, DATAMAP, 0);
	fread(map, 1, MAPWIDTH * MAPHEIGHT, plik);

	gameData->mapwidth = MAPWIDTH;
	gameData->mapheight = MAPHEIGHT;
	// nanos wartosci terenu
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
	// ustal mape narodowosci
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
		else if ( type[i] == 2 )
		{
			// bomber
			fly = (StFly*)fobAlloc(cn_Bomber);
		} else if ( type[i] == 0 ) {
			continue;
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
		if ( vp == NULL ) {
			goc_stringFree( fly->pilotName );
			free( fly );
			continue;
		}
		airport = (StAirport*)FROMGOC(vp);
		airport->pfly = goc_tableAdd(airport->pfly, &airport->nfly,
				sizeof(StFly*));
		airport->pfly[airport->nfly-1] = fly;
	}
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

	createCommonObjects();

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

	// uzupelnianie danych w strukturze gameData
	fobSerialize((fobElement*)gameData, out);
	// gamedataSerialize(gameData, out);
	while (goc_systemCheckMsg( &msg ));
	// TODO: gamedataFree(gameData);
	fclose(out);
	return 0;
}
