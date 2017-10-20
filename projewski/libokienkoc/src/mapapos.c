#include <string.h>
#include "global-inc.h"
#include "mapapos.h"
#include <tools/screen.h>
#include "maska.h"
#include "system.h"

// #define GOC_PRINTDEBUG
#include <tools/log.h>
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

int goc_mapposFill(GOC_HANDLER handler, GOC_StMsgMapFill* msg)
{
	GOC_StMapPos *map = (GOC_StMapPos*)handler;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		GOC_StValuePoint *pointDraw = map->pPunkt[i];
		// sprawdz, czy znajduje sie na rysowanym obszarze
		if ( goc_areaIsInside( msg->mapx, msg->mapy, msg->width, msg->height,
			pointDraw->x, pointDraw->y ) )
		{
			GOC_MSG_MAPGETCHAR(msgchar, pointDraw->value);
			goc_systemSendMsg(handler, msgchar);
			if ( msgcharFull.charcode )
			{
				msg->pElements[(pointDraw->y - msg->mapy)*msg->width + (pointDraw->x - msg->mapx)].code = msgcharFull.charcode;
				msg->pElements[(pointDraw->y - msg->mapy)*msg->width + (pointDraw->x - msg->mapx)].color = msgcharFull.charcolor;
			}
		}
	}
	return GOC_ERR_OK;
}

// uchwyt - okreslenie mapy
// obszar - okreslenie pozycji w mapie i dlugosci rysowanego obszaru
// punkt - okreslenie punktu poczatkowego na screenie, w którym jest pocz±tek
// 	rysowania
int goc_mapaposPaint(GOC_HANDLER uchwyt, GOC_StMsgMapPaint* msg)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	GOC_COLOR lastkolor = 0;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		GOC_StValuePoint *pointDraw = map->pPunkt[i];
		// sprawdz, czy znajduje sie na rysowanym obszarze
		if ( goc_areaIsInside( msg->mapx, msg->mapy, msg->width, msg->height, pointDraw->x, pointDraw->y ) )
		{
			GOC_MSG_MAPGETCHAR(msgchar, pointDraw->value);
			goc_systemSendMsg(uchwyt, msgchar);
			if ( msgcharFull.charcode )
			{
				goc_gotoxy(
					msg->screenx + pointDraw->x - msg->mapx,
					msg->screeny + pointDraw->y - msg->mapy
				);
				// czy wystapila zmiana koloru
				if ( lastkolor ^ msgcharFull.charcolor )
					goc_textallcolor(lastkolor = msgcharFull.charcolor);
				putchar(msgcharFull.charcode);
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
			GOC_MSG_POSMAPFREEPOINT(msgfree,
				map->pPunkt[i]->x, map->pPunkt[i]->y, map->pPunkt[i]->value);
			goc_systemSendMsg(uchwyt, msgfree);
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
	GOC_MSG_MAPGETPOINT(msg, x, y);
	if ( goc_systemSendMsg(uchwyt, msg) == GOC_ERR_OK ) {
		return msgFull.value;
	}
	else {
		return 0;
	}
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

static int mapaposGetPoint(GOC_HANDLER uchwyt, GOC_StMsgMapPointValue *msg)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		if (( msg->x == map->pPunkt[i]->x )
			&& ( msg->y == map->pPunkt[i]->y ))
		{
			msg->value = map->pPunkt[i]->value;
			return GOC_ERR_OK;
		}
	}
	return GOC_ERR_OK;
}

int goc_mapaposSetPoint(GOC_HANDLER uchwyt, int x, int y, int value)
{
	GOC_MSG_MAPSETPOINT(msg, x, y, value);
	return goc_systemSendMsg(uchwyt, msg);
}

static int mapaposSetPoint(GOC_HANDLER uchwyt, GOC_StMsgMapPointValue* msg)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	int i = map->nPunkt;

	// przegladaj dostepne punkty
	while ( i-- )
	{
		if (( msg->x == map->pPunkt[i]->x )
			&& ( msg->y == map->pPunkt[i]->y ))
		{
			map->pPunkt[i]->value = msg->value;
			return GOC_ERR_OK;
		}
	}

	// dodaj nowy punkt
	{
		GOC_StValuePoint *apunkt = malloc(sizeof(GOC_StValuePoint));
		apunkt->x = msg->x;
		apunkt->y = msg->y;
		apunkt->value = msg->value;
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

static int mapaposGetChar(GOC_HANDLER uchwyt, GOC_StMsgMapChar* msg)
{
	GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
	msg->charcode = map->pZnak[msg->position].code;
	msg->charcolor = map->pZnak[msg->position].color;
	return GOC_ERR_OK;
}

int goc_mapaposListener(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_DEBUG("-> goc_mapaposListener");
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT(GOC_StMapPos, map, msg);
		GOC_DEBUG("<- goc_mapaposListener");
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		GOC_StMapPos *map = (GOC_StMapPos*)uchwyt;
		int i = map->nPunkt;
		map->pZnak = goc_tableClear(map->pZnak, &map->nZnak);
		// Usuwanie pojedynczych punktów
		while ( i-- )
		{
			GOC_MSG_POSMAPFREEPOINT( msg,
				map->pPunkt[i]->x, map->pPunkt[i]->y, map->pPunkt[i]->value );
			goc_systemSendMsg(uchwyt, msg);
		}
		map->pPunkt = goc_tableClear(map->pPunkt, &map->nPunkt);
		GOC_DEBUG("<- goc_mapaposListener");
		return goc_elementDestroy(uchwyt);
	}
	else if ( msg->id == GOC_MSG_MAPSETSIZE_ID )
	{
		GOC_DEBUG("<- goc_mapaposListener");
		GOC_StMsgMapSetSize* msgsize = (GOC_StMsgMapSetSize*)msg;
		return goc_mapaposSetSize(uchwyt, msgsize->width, msgsize->height);
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
	else if ( msg->id == GOC_MSG_MAPADDCHAR_ID )
	{
		GOC_StMsgMapChar* msgadd = (GOC_StMsgMapChar*)msg;
		GOC_DEBUG("<- goc_mapaposListener");
		return goc_mapaposAddChar(uchwyt,
			msgadd->charcode, msgadd->charcolor, msgadd->position);
	}
	else if ( msg->id == GOC_MSG_MAPSETPOINT_ID )
	{
		GOC_DEBUG("<- goc_mapaposListener");
		return mapaposSetPoint(uchwyt, (GOC_StMsgMapPointValue*)msg);
	}
	else if ( msg->id == GOC_MSG_MAPGETPOINT_ID )
	{
		GOC_DEBUG("<- goc_mapaposListener");
		return mapaposGetPoint(uchwyt, (GOC_StMsgMapPointValue*)msg);
	}
	else if ( msg->id == GOC_MSG_MAPPAINT_ID )
	{
		return goc_mapaposPaint(uchwyt, (GOC_StMsgMapPaint*)msg);
	}
	else if ( msg->id == GOC_MSG_MAPGETCHAR_ID )
	{
		return mapaposGetChar(uchwyt, (GOC_StMsgMapChar*)msg);
	}
	else if ( msg->id == GOC_MSG_MAPFILL_ID )
	{
		return goc_mapposFill(uchwyt, (GOC_StMsgMapFill*)msg);
	}
	else if ( msg->id == GOC_MSG_POSMAPFREEPOINT_ID )
	{
		// Je¶li nikt tego nie zrobi - zrób to sam
		// free(pBuf);
	}
	GOC_DEBUG("<- goc_mapaposListener");
	return GOC_ERR_UNKNOWNMSG;
}
