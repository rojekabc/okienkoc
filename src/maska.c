#include "global-inc.h"
#include <stdio.h>
#ifndef _DEBUG
#	include <string.h>
#endif

#include "maska.h"
#include "screen.h"
#include "tablica.h"
#include "znak.h"
#include "conflog.h"

const char* GOC_ELEMENT_MASK = "GOC_Mask";
const char* GOC_MSG_MAPSETSIZE = "GOC_MsgMapSetSize";
const char* GOC_MSG_MAPGETCHARAT = "GOC_MsgMapGetCharAt";
const char* GOC_MSG_MAPADDCHAR = "GOC_MsgMapAddChar";
const char* GOC_MSG_MAPSETPOINT = "GOC_MsgMapSetPoint";
const char* GOC_MSG_MAPPAINT = "GOC_MsgMapPaint";
const char* GOC_MSG_MAPGETPOINT = "GOC_MsgMapGetPoint";
const char* GOC_MSG_MAPGETCHAR = "GOC_MsgMapGetChar";
const char* GOC_MSG_POSMAPFREEPOINT = "GOC_MsgMapFreePoint";
const char* GOC_MSG_CURSORCHANGE = "GOC_MsgCursorChange";
const char* GOC_MSG_MAPFILL = "GOC_MsgMapFill";

int goc_maskSet(GOC_HANDLER uchwyt, unsigned char x, unsigned char y, int val)
{
	GOC_StPoint punkt;
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	GOC_DEBUG("-> goc_maskSet");
	punkt.x = x;
	punkt.y = y;
	goc_systemSendMsg(maska->pMapa[maska->nMapa-1], GOC_MSG_MAPSETPOINT,
		&punkt, val);
	GOC_DEBUG("<- goc_maskSet");
	return GOC_ERR_OK;
}

int goc_maskSetRealArea(GOC_HANDLER uchwyt, int d, int w)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	int k;
	GOC_DEBUG("-> goc_maskSetRealArea");
	maska->d = d;
	maska->w = w;
	maska->xp = 0;
	maska->yp = 0;
	maska->kursor.x = 0;
	maska->kursor.y = 0;
	for (k=0; k<maska->nMapa; k++)
	{
		goc_systemSendMsg( maska->pMapa[k],
			GOC_MSG_MAPSETSIZE, (void*)(uintptr_t)d, w );
	}
//	lbnw = maska->dane->lbnw;
//	goc_nbitFieldRemove(maska->dane);
//	maska->dane = goc_nbitFieldCreate(lbnw, maska->d*maska->w);
	GOC_DEBUG("<- goc_maskSetRealArea");
	return GOC_ERR_OK;
}

static int maskPaintMap(GOC_HANDLER uchwyt, GOC_StArea* obszar, GOC_StPoint* punkt)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	int k;
	GOC_DEBUG("-> maskPaintMap");

	// wypelnianie znakami do wypisania
	for (k=0; k<maska->nMapa; k++)
	{
		goc_systemSendMsg(maska->pMapa[k],
			GOC_MSG_MAPPAINT, obszar, (uintptr_t)punkt);
	}
	fflush(stdout);
	GOC_DEBUG("<- maskPaintMap");
	return GOC_ERR_OK;
}
static int maskPaint(GOC_HANDLER uchwyt)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	int k;
	GOC_StPoint punkt;
	// GOC_StArea obszar;
	GOC_StFillArea obszar;
	GOC_DEBUG("-> maskPaint");

	obszar.x = maska->xp;
	obszar.y = maska->yp;
	obszar.width = goc_elementGetWidth(uchwyt);
	obszar.height = goc_elementGetHeight(uchwyt);
	for ( k=0; k<maska->nMapa; k++)
	{
		int check;
		check = goc_elementGetWidth(maska->pMapa[k]);
		if ( obszar.width > check )
			obszar.width = check;
		check = goc_elementGetHeight(maska->pMapa[k]);
		if ( obszar.height > check )
			obszar.height = check;
	}
	obszar.pElement = (GOC_StChar*)malloc(sizeof(GOC_StChar)*obszar.width*obszar.height);
	memset(obszar.pElement, 0, sizeof(GOC_StChar)*obszar.width*obszar.height);

	punkt.x = goc_elementGetScreenX( uchwyt );
	punkt.y = goc_elementGetScreenY( uchwyt );
/*
	GOC_BDEBUG("Punkt (%d,%d) Obszar (%d,%d)+(%d,%d)",
		punkt.x, punkt.y,
		obszar.x, obszar.y,
		obszar.width, obszar.height);
*/
	// wypelnianie obszaru danymi
	for (k=0; k<maska->nMapa; k++)
	{
		goc_systemSendMsg(maska->pMapa[k],
			GOC_MSG_MAPFILL, &obszar, 0);
	}

	// rysowanie na ekranie
	{
		// GOC_StMapRaw *map = (GOC_StMapRaw*)uchwyt;
		// int pos;
		int pos;
		int j, i;
		GOC_COLOR lastkolor = 0;
		GOC_StChar znak;
		// wstepne wyliczenie przesuniecia
		// pos = obszar->y*map->width + obszar->x;
		pos = 0;
		for ( j=0; j<obszar.height; j++ )
		{
			goc_gotoxy(punkt.x, punkt.y + j * maska->ys);
			for ( i=0; i<obszar.width; i++, pos++ )
			{
				znak = obszar.pElement[pos];
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
				if ( maska->xs > 1)
					goc_moveright(maska->xs-1);
			}
		}
	}
	// zwalnianie pamieci
	free(obszar.pElement);
	/*
	// wypelnianie znakami do wypisania
	for (k=0; k<maska->nMapa; k++)
	{
		goc_systemSendMsg(maska->pMapa[k],
			GOC_MSG_MAPPAINT, &obszar, (unsigned int)(&punkt));
	}
	*/
	goc_gotoxy(
		goc_elementGetScreenX( uchwyt ) + (maska->kursor.x-maska->xp)*maska->xs,
		goc_elementGetScreenY( uchwyt ) + (maska->kursor.y-maska->yp)*maska->ys);
	fflush(stdout);
	GOC_DEBUG("<- maskPaint");
	return GOC_ERR_OK;
}

int goc_maskGet(GOC_HANDLER uchwyt, int x, int y)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	GOC_StPoint punkt;
	int value, k;
	GOC_DEBUG("-> goc_maskGet");
	for (k=0; k<maska->nMapa; k++ )
	{
		punkt.x = x;
		punkt.y = y;
		goc_systemSendMsg(maska->pMapa[k],
			GOC_MSG_MAPGETPOINT,
			&punkt, (uintptr_t)(&value));
	}
	return value;
}

// okre¶lenie punktu na rzeczywistym ekranie
// w stosuku do polozenia obiektu, ktory ma zostaæ
// odrysowany
int goc_maskPaintPoint(GOC_HANDLER uchwyt, int x, int y)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	int k;
	GOC_StPoint punkt;
	GOC_StArea obszar;
	GOC_DEBUG("-> goc_maskPaintPoint");

	obszar.x = maska->xp + x;
	obszar.y = maska->yp + y;
	obszar.width = 1;
	obszar.height = 1;

	punkt.x = goc_elementGetScreenX( uchwyt ) + x;
	punkt.y = goc_elementGetScreenY( uchwyt ) + y;
	goc_saveXY();
	// wypelnianie znakami do wypisania
	for (k=0; k<maska->nMapa; k++)
	{
		goc_systemSendMsg(maska->pMapa[k],
			GOC_MSG_MAPPAINT, &obszar, (uintptr_t)(&punkt));
	}
	goc_loadXY();
	fflush(stdout);
	GOC_DEBUG("<- goc_maskPaintPoint");
	return GOC_ERR_OK;
}

static int maskFocus(GOC_HANDLER uchwyt)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	GOC_DEBUG("-> maskFocus");
	if ( !(maska->flag & GOC_EFLAGA_PAINTED) )
	{
		GOC_DEBUG("<- maskFocus");
		return GOC_ERR_REFUSE;
	}
	if ( !(maska->flag & GOC_EFLAGA_ENABLE) )
	{
		GOC_DEBUG("<- maskFocus");
		return GOC_ERR_REFUSE;
	}
	// Postaw kursor w ostanim po³o¿eniu (nie odrysowywuj)
	goc_gotoxy(
		goc_elementGetScreenX( uchwyt ) + (maska->kursor.x-maska->xp)*maska->xs,
		goc_elementGetScreenY( uchwyt ) + (maska->kursor.y-maska->yp)*maska->ys);
	fflush(stdout);
//	goc_systemSendMsg(uchwyt, GOC_MSG_PAINT, 0, 0);
	GOC_DEBUG("<- maskFocus");
	return GOC_ERR_OK;
}

static int maskFreeFocus(GOC_HANDLER uchwyt)
{
//	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	GOC_DEBUG("-> maskFreeFocus");
//	goc_systemSendMsg(uchwyt, GOC_MSG_PAINT, 0, 0);
	GOC_DEBUG("<- maskFreeFocus");
	return GOC_ERR_OK;
}

static int maskHotKeyUp(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	if ( maska->kursor.y > 0 )
	{
		(maska->kursor.y)--;
		goc_systemSendMsg(uchwyt, GOC_MSG_CURSORCHANGE, &(maska->kursor), 0);
	}
	if ( maska->kursor.y < maska->yp )
		(maska->yp)--;
	else
	{
		// Takie wstawki zapewniaj±, ¿e gdy nie ma przesuniêcia to nie
		// trzeba wykonywaæ ca³kowitego przerysowywania, tylko przesuniêcie
		// kursora
		goc_gotoxy(
			goc_elementGetScreenX( uchwyt ) + (maska->kursor.x-maska->xp)*maska->xs,
			goc_elementGetScreenY( uchwyt ) + (maska->kursor.y-maska->yp)*maska->ys);
		fflush(stdout);
		return GOC_ERR_OK;
	}
	goc_systemSendMsg(uchwyt, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

static int maskHotKeyDown(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
		if ( maska->kursor.y < maska->w-1 )
		{
			(maska->kursor.y)++;
			goc_systemSendMsg(uchwyt, GOC_MSG_CURSORCHANGE, &(maska->kursor), 0);
		}
		if ( maska->kursor.y >= maska->yp + goc_elementGetHeight(uchwyt) )
			(maska->yp)++;
		else
		{
			goc_gotoxy(
				goc_elementGetScreenX( uchwyt ) + (maska->kursor.x-maska->xp)*maska->xs,
				goc_elementGetScreenY( uchwyt ) + (maska->kursor.y-maska->yp)*maska->ys);
			fflush(stdout);
			return GOC_ERR_OK;
		}
	goc_systemSendMsg(uchwyt, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

static int maskHotKeyRight(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
		if ( maska->kursor.x < maska->d-1 )
		{
			(maska->kursor.x)++;
			goc_systemSendMsg(uchwyt, GOC_MSG_CURSORCHANGE, &(maska->kursor), 0);
		}
		if ( maska->kursor.x >= maska->xp + goc_elementGetWidth(uchwyt) )
			(maska->xp)++;
		else
		{
			goc_gotoxy(
				goc_elementGetScreenX( uchwyt ) + (maska->kursor.x-maska->xp)*maska->xs,
				goc_elementGetScreenY( uchwyt ) + (maska->kursor.y-maska->yp)*maska->ys);
			fflush(stdout);
			return GOC_ERR_OK;
		}
	goc_systemSendMsg(uchwyt, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

static int maskHotKeyLeft(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
		if ( maska->kursor.x > 0 )
		{
			(maska->kursor.x)--;
			goc_systemSendMsg(uchwyt, GOC_MSG_CURSORCHANGE, &(maska->kursor), 0);
		}
		if ( maska->kursor.x < maska->xp )
			(maska->xp)--;
		else
		{
			goc_gotoxy(
				goc_elementGetScreenX( uchwyt ) + (maska->kursor.x-maska->xp)*maska->xs,
				goc_elementGetScreenY( uchwyt ) + (maska->kursor.y-maska->yp)*maska->ys);
			fflush(stdout);
			return GOC_ERR_OK;
		}
	goc_systemSendMsg(uchwyt, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

int goc_maskSetValue(GOC_HANDLER uchwyt, short pozycja, char code, GOC_COLOR color)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	GOC_DEBUG("-> goc_maskSetValue");
	if ( maska->nMapa )
	{
		GOC_StChar znak;
		znak.code = code;
		znak.color = color;
		goc_systemSendMsg(maska->pMapa[maska->nMapa-1], GOC_MSG_MAPADDCHAR,
			&znak, pozycja);
	}
//	maska->pZnak[pozycja] = znak;
//	maska->pKolor[pozycja] = color;
	GOC_DEBUG("<- goc_maskSetValue");
	return GOC_ERR_OK;
}

/* Funkcja wycofana - ustawienia uzaleznione od mapy
// zakres wartosci wycofany
int maskaUstRodzajWartosci(GOC_HANDLER uchwyt, unsigned char liczbaBitow, unsigned int zakresWartosci)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	GOC_DEBUG("-> maskaUstRodzajWartosci");

	GOC_DEBUG("<- maskaUstRodzajWartosci");
	return GOC_ERR_OK;
}
*/

// zwraca numer identyfikujacy mape
int goc_maskAddMap(GOC_HANDLER uchwyt, GOC_HANDLER mapa)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;
	GOC_DEBUG("-> goc_maskAddMap");
	GOC_FDEBUG(goc_stringAddInt(goc_stringAdd(NULL, "Dodaje "), (int)mapa));
	maska->pMapa=goc_tableAdd(maska->pMapa, &maska->nMapa, sizeof(GOC_HANDLER));
	maska->pMapa[maska->nMapa-1] = mapa;
	GOC_DEBUG("<- goc_maskAddMap");
	return maska->nMapa-1;
}

void goc_maskRemMaps(GOC_HANDLER uchwyt)
{
	GOC_StMask *maska = (GOC_StMask*)uchwyt;

	GOC_DEBUG("-> goc_maskRemMaps");
	//for (i=0; i<maska->nMapa; i++)
	//	goc_systemSendMsg(maska->pMapa[maska->nMapa-1], GOC_MSG_DESTROY,NULL,0);
	maska->pMapa = goc_tableClear(maska->pMapa, &(maska->nMapa));

	GOC_DEBUG("<- goc_maskRemMaps");
}

int goc_maskListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_DEBUG("-> goc_maskListener");
	int errCode = GOC_ERR_OK;
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StMask* maska = (GOC_StMask*)malloc(sizeof(GOC_StMask));

		memcpy(maska, elem, sizeof(GOC_StElement));
		maska->d = goc_elementGetWidth((GOC_HANDLER)elem);
		maska->w = goc_elementGetHeight((GOC_HANDLER)elem);
		maska->xp = 0;
		maska->yp = 0;
		maska->xs = 1;
		maska->ys = 1;
		maska->kursor.x = 0;
		maska->kursor.y = 0;
		maska->pMapa = NULL;
		maska->nMapa = 0;
//		goc_maskAddMap((GOC_HANDLER)maska, maparaw = goc_elementCreate(
//			GOC_ELEMENT_RAWMAP, 0, 0, maska->d, maska->w,
//			GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_WHITE,(GOC_HANDLER)maska));
//		goc_maparawAddChar(maparaw, '0', GOC_WHITE, 0);
//		goc_maparawAddChar(maparaw, '1', GOC_WHITE | GOC_FBOLD, 1);
		// TODO:
//		maska->zw = 1;
//		maska->pKolor = NULL;
//		maska->pZnak = NULL;
//		maska->dane = goc_nbitFieldCreate(1, maska->d * maska->w);
//		maska->nZnak = 0;
//		maska->pKolor = goc_tableAdd(maska->pKolor, &maska->nZnak, sizeof(GOC_COLOR));
//		maska->pKolor = goc_tableAdd(maska->pKolor, &maska->nZnak, sizeof(GOC_COLOR));
//		maska->pKolor[0] = GOC_WHITE;
//		maska->pKolor[1] = GOC_WHITE | GOC_FBOLD;
//		maska->nZnak = 0;
//		maska->pZnak = goc_tableAdd(maska->pZnak, &maska->nZnak, 1);
//		maska->pZnak = goc_tableAdd(maska->pZnak, &maska->nZnak, 1);
//		maska->pZnak[0] = '0';
//		maska->pZnak[1] = '1';
		*retuchwyt = (GOC_HANDLER)maska;
		goc_hkAdd(*retuchwyt, 0x603, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			maskHotKeyUp);
		goc_hkAdd(*retuchwyt, 0x600, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			maskHotKeyDown);
		goc_hkAdd(*retuchwyt, 0x602, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			maskHotKeyRight);
		goc_hkAdd(*retuchwyt, 0x601, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			maskHotKeyLeft);
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		GOC_StMask *maska = (GOC_StMask*)uchwyt;
	//	goc_maskRemMaps(uchwyt);
		maska->pMapa = goc_tableClear(maska->pMapa, &maska->nMapa);
//		maska->pZnak = goc_tableClear(maska->pZnak, &maska->nZnak);
//		maska->nZnak = 1;
//		maska->pKolor = goc_tableClear(maska->pKolor, &maska->nZnak);
//		goc_nbitFieldRemove(maska->dane);
		errCode = goc_elementDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_FOCUS )
	{
		errCode = maskFocus(uchwyt);
	}
	else if ( wiesc == GOC_MSG_FOCUSFREE )
	{
		errCode = maskFreeFocus(uchwyt);
	}
	else if ( wiesc == GOC_MSG_MAPPAINT )
	{
		errCode = maskPaintMap(
			uchwyt, (GOC_StArea*)pBuf, (GOC_StPoint*)nBuf);
	}
	else if ( wiesc == GOC_MSG_PAINT )
	{
		errCode = maskPaint(uchwyt);
	}
	else
	{
		errCode = GOC_ERR_UNKNOWNMSG;
	}
	GOC_DEBUG("<- goc_maskListener");
	return errCode;
}
