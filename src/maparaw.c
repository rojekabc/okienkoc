#include <string.h>
#include "global-inc.h"
#include "maparaw.h"
#include "screen.h"
#include "maska.h"

#include "conflog.h"
#include <stdio.h>

const char* GOC_ELEMENT_RAWMAP = "GOC_RawMap";

// TODO: pozycja
int goc_maparawAddChar(GOC_HANDLER uchwyt, char znak, GOC_COLOR color, int pozycja)
{
	GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
	if ( map->nZnak <= pozycja )
	{
		pozycja = map->nZnak;
		map->pZnak = goc_tableAdd(map->pZnak, &map->nZnak, sizeof(GOC_StChar));
	}
	map->pZnak[pozycja].code = znak;
	map->pZnak[pozycja].color = color;
	return GOC_ERR_OK;
}

int goc_maparaw_SetSize(GOC_HANDLER uchwyt, int d, int w)
{
	GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
	int lb = map->dane->lbnw;
	map->width = d;
	map->height = w;
	map->dane = goc_nbitFieldRemove(map->dane);
	map->dane = goc_nbitFieldCreate(lb, d*w);
	return GOC_ERR_OK;
}

// ustaw liczbe bitow na wartosc
int goc_maparawSetBPV(GOC_HANDLER uchwyt, int bpv)
{
	GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
	map->dane = goc_nbitFieldRemove(map->dane);
	map->dane = goc_nbitFieldCreate(bpv, map->width*map->height);
	return GOC_ERR_OK;
}

int goc_maprawFill(GOC_HANDLER handler, GOC_StFillArea* area)
{
	GOC_StMapRaw *map = (GOC_StMapRaw*)handler;
	int j, i, k;
	int pos;
	GOC_StChar znak;

	pos = area->y*map->width + area->x;
	k = 0;
	for ( j=0; j<area->height; j++ )
	{
		for ( i=0; i<area->width; i++, pos++ )
		{
			int val = goc_nbitFieldGet(map->dane, pos);
			znak = map->pZnak[val];
			if ( znak.code )
				area->pElement[k] = znak;
			k++;
		}
		// przemieszczenie korygujace
		pos -= area->width;
		pos += map->width;
	}
}

// uchwyt - okreslenie mapy
// obszar - okreslenie pozycji w mapie i dlugosci rysowanego obszaru
// punkt - okreslenie punktu poczatkowego na screenie, w którym jest pocz±tek
// 	rysowania
int goc_maprawPaint(GOC_HANDLER uchwyt, GOC_StArea *obszar, GOC_StPoint *punkt)
{
	GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
	int val;
	int pos;
	int j, i;
	GOC_COLOR lastkolor = 0;
	GOC_StChar znak;
	// wstepne wyliczenie przesuniecia
	pos = obszar->y*map->width + obszar->x;
	for ( j=0; j<obszar->height; j++ )
	{
		goc_gotoxy(punkt->x, punkt->y + j);
		for ( i=0; i<obszar->width; i++, pos++ )
		{
			val = goc_nbitFieldGet(map->dane, pos);
			znak = map->pZnak[val];
			if ( znak.code )
			{
				// czy wystapila zmiana koloru
				if ( lastkolor ^ znak.color )
					goc_textallcolor(lastkolor = znak.color);
				putchar(znak.code);
			}
			else
			{
				goc_moveright(1);
			}
		}
		// przemieszczenie korygujace
		pos -= obszar->width;
		pos += map->width;
	}
	return GOC_ERR_OK;
}

int goc_maparawSetPoint(GOC_HANDLER uchwyt, int x, int y, int value)
{
	GOC_StPoint punkt;
	punkt.x = x;
	punkt.y = y;
	goc_systemSendMsg(uchwyt, GOC_MSG_MAPSETPOINT, &punkt, value);
	return GOC_ERR_OK;
}

int goc_maparawGetPoint(GOC_HANDLER uchwyt, int x, int y)
{
	GOC_StPoint punkt;
	int value;
	punkt.x = x;
	punkt.y = y;
	goc_systemSendMsg(uchwyt, GOC_MSG_MAPGETPOINT, &punkt, (int)&value);
	return value;
}

int goc_maparawListener(
	GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, unsigned int nBuf)
{
	int retCode = GOC_ERR_OK;
	GOC_DEBUG("-> goc_maparawListener");
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StMapRaw *map = malloc(sizeof(GOC_StMapRaw));
		memset(map, 0, sizeof(GOC_StMapRaw));
		memcpy(map, elem, sizeof(GOC_StElement));
		map->dane = goc_nbitFieldCreate(1, elem->width * elem->height);
		*retuchwyt = (GOC_HANDLER)map;
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		map->dane = goc_nbitFieldRemove(map->dane);
		map->pZnak = goc_tableClear(map->pZnak, &map->nZnak);
		retCode = goc_elementDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_MAPSETSIZE )
	{
		retCode = goc_maparaw_SetSize(uchwyt, (int)pBuf, (int)nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPGETCHARAT )
	{
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		GOC_StPoint *punkt = (GOC_StPoint*)pBuf;
		GOC_StChar *znak = (GOC_StChar*)nBuf;
		int val = goc_nbitFieldGet(map->dane, punkt->y*map->width + punkt->x);
		*znak = map->pZnak[val];

	}
	else if ( wiesc == GOC_MSG_MAPADDCHAR )
	{
		GOC_StChar *znak = (GOC_StChar*)pBuf;
		retCode = goc_maparawAddChar(uchwyt, znak->code, znak->color, nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPSETPOINT )
	{
		GOC_StPoint *punkt = (GOC_StPoint*)pBuf;
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		goc_nbitFieldSet( map->dane, map->width*punkt->y + punkt->x, nBuf );
	}
	else if ( wiesc == GOC_MSG_MAPGETPOINT )
	{
		GOC_StPoint *punkt = (GOC_StPoint*)pBuf;
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		int *value = (int *)nBuf;
		if ( value )
			*value = goc_nbitFieldGet( map->dane, map->width*punkt->y + punkt->x );
	}
	else if ( wiesc == GOC_MSG_MAPPAINT )
	{
		retCode = goc_maprawPaint(uchwyt, (GOC_StArea*)pBuf, (GOC_StPoint*)nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPFILL )
	{
		retCode = goc_maprawFill(uchwyt, (GOC_StFillArea*)pBuf);
	}
	else
		retCode = GOC_ERR_UNKNOWNMSG;
	GOC_DEBUG("<- goc_maparawListener");
	return retCode;
}
