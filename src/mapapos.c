#include <string.h>
#include "global-inc.h"
#include "mapapos.h"
#include "screen.h"
#include "maska.h"

// #define GOC_PRINTDEBUG
#include "log.h"
#include <stdio.h>

const char* GOC_ELEMENT_POSMAP = "GOC_PositionMap";

// TODO: pozycja
int goc_mapaposAddChar(GOC_HANDLER uchwyt, char znak, GOC_COLOR color, int pozycja)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	if ( map->nZnak <= pozycja )
	{
		pozycja = map->nZnak;
		map->pZnak = goc_tableAdd(map->pZnak, &map->nZnak, sizeof(GOC_StChar));
	}
	map->pZnak[pozycja].code = znak;
	map->pZnak[pozycja].color = color;
	return GOC_ERR_OK;
}

int goc_mapaposSetSize(GOC_HANDLER uchwyt, int d, int w)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	map->width = d;
	map->height = w;
	return GOC_ERR_OK;
}

int goc_mapposFill(GOC_HANDLER handler, GOC_StFillArea* area)
{
	GOC_StMapPos *map = (GOC_StMapPos*)handler;
	GOC_StChar znak;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		GOC_StValuePoint *pointDraw = map->pPunkt[i];
		// sprawdz, czy znajduje sie na rysowanym obszarze
		if ( goc_areaIsInside( (*area), (*pointDraw) ) )
		{
			goc_systemSendMsg(handler, GOC_MSG_MAPGETCHAR, &znak, (uintptr_t)pointDraw);
			if ( znak.code )
			{
				area->pElement[(pointDraw->y - area->y)*area->width + (pointDraw->x - area->x)] = znak;
			}
		}
	}
	return GOC_ERR_OK;
}

// uchwyt - okreslenie mapy
// obszar - okreslenie pozycji w mapie i dlugosci rysowanego obszaru
// punkt - okreslenie punktu poczatkowego na screenie, w którym jest pocz±tek
// 	rysowania
int goc_mapaposPaint(GOC_HANDLER uchwyt, GOC_StArea *obszar, GOC_StPoint *punkt)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	GOC_COLOR lastkolor = 0;
	GOC_StChar znak;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		GOC_StValuePoint *pointDraw = map->pPunkt[i];
		// sprawdz, czy znajduje sie na rysowanym obszarze
		if ( goc_areaIsInside( (*obszar), (*pointDraw) ) )
		{
			goc_systemSendMsg(uchwyt, GOC_MSG_MAPGETCHAR, &znak, (uintptr_t)pointDraw);
			if ( znak.code )
			{
				goc_gotoxy(
					punkt->x + pointDraw->x - obszar->x,
					punkt->y + pointDraw->y - obszar->y
				);
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
	}
	return GOC_ERR_OK;
}

int goc_mapaposRemPoint(GOC_HANDLER uchwyt, int x, int y)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	int i = map->nPunkt;
	GOC_StPoint punkt;
	punkt.x = x;
	punkt.y = y;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		if (( punkt.x == map->pPunkt[i]->x )
			&& ( punkt.y == map->pPunkt[i]->y ))
		{
			// je¿eli kto¶ ma co¶ jeszcze zmieniæ powiadom
			// o tym fakcie, ¿e nastapi zwolnienie
			goc_systemSendMsg(uchwyt, GOC_MSG_POSMAPFREEPOINT,
				map->pPunkt[i], 0);
			map->pPunkt = goc_tableRemove(
				map->pPunkt, &map->nPunkt,
				sizeof(GOC_StValuePoint*), i);
			return GOC_ERR_OK;
		}
	}
	return GOC_ERR_OK;
}

// Zmienia wartosc punktu
int goc_mapaposChgPoint(GOC_HANDLER uchwyt, int x, int y, int value)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	int i = map->nPunkt;
	while ( i-- )
	{
		GOC_StValuePoint *punkt = map->pPunkt[i];
		if (( punkt->x == x ) && ( punkt->y == y ))
		{
			punkt->value = value;
		}
	}
	return GOC_ERR_OK;
}

// Zwraca wartosc
int goc_mapposGetPoint(GOC_HANDLER uchwyt, int x, int y)
{
	int value;
	GOC_StPoint punkt;
	punkt.x = x;
	punkt.y = y;

	if ( goc_systemSendMsg(uchwyt, GOC_MSG_MAPGETPOINT, &punkt, (uintptr_t)(&value)) == GOC_ERR_OK )
		return value;
	else
		return 0;
}

GOC_StValuePoint* goc_mapaposReadPoint(GOC_HANDLER uchwyt, int x, int y)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		if (( x == map->pPunkt[i]->x ) && ( y == map->pPunkt[i]->y ))
			return map->pPunkt[i];
	}
	return NULL;
}

static int mapaposGetPoint(GOC_HANDLER uchwyt, GOC_StPoint *punkt, int *value)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	int i = map->nPunkt;
	if ( value == NULL )
		return GOC_ERR_OK;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		if (( punkt->x == map->pPunkt[i]->x )
			&& ( punkt->y == map->pPunkt[i]->y ))
		{
			*value = map->pPunkt[i]->value;
			return GOC_ERR_OK;
		}
	}
	return GOC_ERR_OK;
}

int goc_mapaposSetPoint(GOC_HANDLER uchwyt, int x, int y, int value)
{
	GOC_StPoint punkt;
	punkt.x = x;
	punkt.y = y;
	return goc_systemSendMsg(uchwyt, GOC_MSG_MAPSETPOINT, &punkt, value);
}

static int mapaposSetPoint(GOC_HANDLER uchwyt, GOC_StPoint *punkt, int value)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		if (( punkt->x == map->pPunkt[i]->x )
			&& ( punkt->y == map->pPunkt[i]->y ))
		{
			map->pPunkt[i]->value = value;
			return GOC_ERR_OK;
		}
	}

	// dodaj nowy punkt
	{
		GOC_StValuePoint *apunkt = malloc(sizeof(GOC_StValuePoint));
		apunkt->x = punkt->x;
		apunkt->y = punkt->y;
		apunkt->value = value;
		goc_mapaposAddPoint(uchwyt, apunkt);
	}

	return GOC_ERR_OK;
}

// Dodany punkt jest utworzony na zewn±trz i powinien
// byæ zwalniany poprzez wywo³anie zdefiniowanej funkcji uzytkownika
int goc_mapaposAddPoint(GOC_HANDLER uchwyt, GOC_StValuePoint *vpunkt)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	if (( map == NULL ) || ( vpunkt == NULL ))
		return GOC_ERR_WRONGARGUMENT;
	map->pPunkt = goc_tableAdd(map->pPunkt, &map->nPunkt,
		sizeof(GOC_StValuePoint*));
	map->pPunkt[map->nPunkt-1] = vpunkt;
	return GOC_ERR_OK;
}

static int mapaposAddChar(GOC_HANDLER uchwyt, GOC_StChar* znak, uintptr_t nBuf)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	GOC_StValuePoint* punkt = (GOC_StValuePoint*)nBuf;
	if ( znak == NULL )
		return GOC_ERR_OK;
	memcpy(znak, &map->pZnak[punkt->value], sizeof(GOC_StChar));
	return GOC_ERR_OK;
}

int goc_mapaposListener(
	GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	GOC_DEBUG("-> goc_mapaposListener");
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StMapPos *map = malloc(sizeof(GOC_StMapPos));
		memset(map, 0, sizeof(GOC_StMapPos));
		memcpy(map, elem, sizeof(GOC_StElement));
		*retuchwyt = (GOC_HANDLER)map;
		GOC_DEBUG("<- goc_mapaposListener");
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
		int i = map->nPunkt;
		map->pZnak = goc_tableClear(map->pZnak, &map->nZnak);
		// Usuwanie pojedynczych punktów
		while ( i-- )
		{
			goc_systemSendMsg(uchwyt, GOC_MSG_POSMAPFREEPOINT,
				map->pPunkt[i], 0);
		}
		map->pPunkt = goc_tableClear(map->pPunkt, &map->nPunkt);
		GOC_DEBUG("<- goc_mapaposListener");
		return goc_elementDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_MAPSETSIZE )
	{
		GOC_DEBUG("<- goc_mapaposListener");
		return goc_mapaposSetSize(uchwyt, (int)pBuf, (int)nBuf);
	}
	// TODO:
/*	else if ( wiesc == GOC_MSG_MAPGETCHARAT )
	{
		GOC_DEBUG("-> mapaposDajZnakNa");
		GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
		GOC_StPoint *punkt = (GOC_StPoint*)pBuf;
		GOC_StChar *znak = (GOC_StChar*)nBuf;
		GOC_DEBUG("<- mapaposDajZnakNa");
		GOC_DEBUG("<- goc_mapaposListener");
		return GOC_ERR_OK;

	}*/
	else if ( wiesc == GOC_MSG_MAPADDCHAR )
	{
		GOC_StChar *znak = (GOC_StChar*)pBuf;
		GOC_DEBUG("<- goc_mapaposListener");
		return goc_mapaposAddChar(uchwyt, znak->code, znak->color, nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPSETPOINT )
	{
		GOC_StPoint *punkt = (GOC_StPoint*)pBuf;
		GOC_DEBUG("<- goc_mapaposListener");
		return mapaposSetPoint(uchwyt, punkt, nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPGETPOINT )
	{
		GOC_StPoint *punkt = (GOC_StPoint*)pBuf;
		GOC_DEBUG("<- goc_mapaposListener");
		return mapaposGetPoint(uchwyt, punkt, (int*)nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPPAINT )
	{
		return goc_mapaposPaint(uchwyt, (GOC_StArea*)pBuf, (GOC_StPoint*)nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPGETCHAR )
	{
		return mapaposAddChar(uchwyt, (GOC_StChar*)pBuf, nBuf);
	}
	else if ( wiesc == GOC_MSG_MAPFILL )
	{
		return goc_mapposFill(uchwyt, (GOC_StFillArea*)pBuf);
	}
	else if ( wiesc == GOC_MSG_POSMAPFREEPOINT )
	{
		// Je¶li nikt tego nie zrobi - zrób to sam
		free(pBuf);
	}
	GOC_DEBUG("<- goc_mapaposListener");
	return GOC_ERR_UNKNOWNMSG;
}
