#include <string.h>
#include "global-inc.h"
#include "maparaw.h"
#include "screen.h"
#include "maska.h"
#include "system.h"

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

int goc_maprawFill(GOC_HANDLER handler, GOC_StMsgMapFill* msg)
{
	GOC_StMapRaw *map = (GOC_StMapRaw*)handler;
	int j, i, k;
	int pos;
	GOC_StChar znak;

	pos = msg->mapy * map->width + msg->mapx;
	k = 0;
	for ( j = 0; j < msg->height; j++ )
	{
		for ( i = 0; i < msg->width; i++, pos++ )
		{
			int val = goc_nbitFieldGet(map->dane, pos);
			znak = map->pZnak[val];
			if ( znak.code ) {
				msg->pElements[k] = znak;
			}
			k++;
		}
		// przemieszczenie korygujace
		pos -= msg->width;
		pos += map->width;
	}
	return GOC_ERR_OK;
}

// uchwyt - okreslenie mapy
// obszar - okreslenie pozycji w mapie i dlugosci rysowanego obszaru
// punkt - okreslenie punktu poczatkowego na screenie, w którym jest pocz±tek
// 	rysowania
int goc_maprawPaint(GOC_HANDLER uchwyt, GOC_StMsgMapPaint *msg)
{
	GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
	int val;
	int pos;
	int j, i;
	GOC_COLOR lastkolor = 0;
	GOC_StChar znak;
	// wstepne wyliczenie przesuniecia
	pos = msg->mapy * map->width + msg->mapx;
	for ( j=0; j<msg->height; j++ )
	{
		goc_gotoxy(msg->screenx, msg->screeny + j);
		for ( i=0; i<msg->width; i++, pos++ )
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
		pos -= msg->width;
		pos += map->width;
	}
	return GOC_ERR_OK;
}

int goc_maparawSetPoint(GOC_HANDLER uchwyt, int x, int y, int value)
{
	GOC_MSG_MAPSETPOINT(msg, x, y, value);
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

int goc_maparawGetPoint(GOC_HANDLER uchwyt, int x, int y)
{
	GOC_MSG_MAPGETPOINT(msg, x, y);
	goc_systemSendMsg(uchwyt, msg);
	return msgFull.value;
}

int goc_maparawListener(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	int retCode = GOC_ERR_OK;
	GOC_DEBUG("-> goc_maparawListener");
	if ( msg->id == GOC_MSG_CREATE_ID ) {
		GOC_CREATE_ELEMENT(GOC_StMapRaw, map, msg);
		map->dane = goc_nbitFieldCreate(1, map->width * map->height);
	} else if ( msg->id == GOC_MSG_DESTROY_ID ) {
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		map->dane = goc_nbitFieldRemove(map->dane);
		map->pZnak = goc_tableClear(map->pZnak, &map->nZnak);
		retCode = goc_elementDestroy(uchwyt);
	} else if ( msg->id == GOC_MSG_MAPSETSIZE_ID ) {
		GOC_StMsgMapSetSize* msgsize = (GOC_StMsgMapSetSize*)msg;
		retCode = goc_maparaw_SetSize(uchwyt, msgsize->width, msgsize->height);
	} else if ( msg->id == GOC_MSG_MAPGETCHARAT_ID ) {
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		GOC_StMsgMapGetCharAt* msgchar = (GOC_StMsgMapGetCharAt*)msg;
		int val = goc_nbitFieldGet(map->dane, msgchar->y * map->width + msgchar->x);
		msgchar->charcolor = map->pZnak[val].color;
		msgchar->charcode = map->pZnak[val].code;
	} else if ( msg->id == GOC_MSG_MAPADDCHAR_ID ) {
		GOC_StMsgMapChar* msgadd = (GOC_StMsgMapChar*)msg;
		retCode = goc_maparawAddChar(uchwyt, msgadd->charcode, msgadd->charcolor, msgadd->position);
	} else if ( msg->id == GOC_MSG_MAPSETPOINT_ID ) {
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		GOC_StMsgMapPointValue* msgpv = (GOC_StMsgMapPointValue*)msg;
		goc_nbitFieldSet( map->dane, map->width * msgpv->y + msgpv->x, msgpv->value );
	} else if ( msg->id == GOC_MSG_MAPGETPOINT_ID ) {
		GOC_StMsgMapPointValue* msgpv = (GOC_StMsgMapPointValue*)msg;
		GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		msgpv->value = goc_nbitFieldGet( map->dane, map->width * msgpv->y + msgpv->x );
	} else if ( msg->id == GOC_MSG_MAPPAINT_ID ) {
		retCode = goc_maprawPaint(uchwyt, (GOC_StMsgMapPaint*)msg);
	} else if ( msg->id == GOC_MSG_MAPFILL_ID ) {
		retCode = goc_maprawFill(uchwyt, (GOC_StMsgMapFill*)msg);
	} else {
		retCode = GOC_ERR_UNKNOWNMSG;
	}
	GOC_DEBUG("<- goc_maparawListener");
	return retCode;
}
