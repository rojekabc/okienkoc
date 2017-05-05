#include <string.h>
#include <stdio.h>
#include "slista.h"

#include "screen.h"
//#include "mystr.h"
#include "tablica.h"
//#include "napis.h"
//#include "ramka.h"
//#include "hotkey.h"
#include "global-inc.h"

const char* GOC_ELEMENT_SELLIST = "GOC_SelectionList";

static int sellistDestroy(GOC_HANDLER uchwyt)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	slista->pZaznaczony = goc_nbitFieldRemove(slista->pZaznaczony);
	return goc_listListener(uchwyt, GOC_MSG_DESTROY, NULL, 0);
}

static int sellistInit(GOC_HANDLER uchwyt)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	goc_listListener(uchwyt, GOC_MSG_INIT, slista, 0);
	slista->kolorPoleZaznaczony = GOC_GREEN;
	slista->kolorPoleKursorZaznaczony = GOC_GREEN | GOC_FBOLD;
	slista->pZaznaczony = goc_nbitFieldCreate(1, 0);
	return GOC_ERR_OK;
}

int goc_sellistUnselect(GOC_HANDLER uchwyt, int pos)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	if ( slista->nKolumna == 0 )
		return GOC_ERR_REFUSE;
	goc_nbitFieldSet(slista->pZaznaczony, pos, 0);
	return GOC_ERR_OK;
}

int goc_sellistSelect(GOC_HANDLER uchwyt, int pos)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	if ( slista->nKolumna == 0 )
		return GOC_ERR_REFUSE;
	goc_nbitFieldSet(slista->pZaznaczony, pos, 1);
	return GOC_ERR_OK;
}

GOC_BOOL goc_sellistIsSelected(GOC_HANDLER uchwyt, int pos)
{
	int tmp;
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	if ( slista->nKolumna == 0 )
		return GOC_FALSE;
	// wyrownanie wzgledem kolumny
//	pos -= pos % slista->nKolumna;
	tmp = goc_nbitFieldGet(slista->pZaznaczony, pos);
	if ( tmp )
		return GOC_TRUE;
	else
		return GOC_FALSE;
}

static GOC_COLOR goc_sellistSetColor(GOC_HANDLER uchwyt, int pos)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	if ( slista->nKolumna == 0 )
		return slista->color;
//	pos -= pos % slista->nKolumna;
	if ( pos != slista->kursor )
	{
		if ( goc_sellistIsSelected(uchwyt, pos) )
			return slista->kolorPoleZaznaczony;
		else
			return slista->color;
	}
	else
	{
		if ( goc_sellistIsSelected(uchwyt, pos) )
			return slista->kolorPoleKursorZaznaczony;
		else
			return slista->kolorPoleKursor;
	}
}

static int sellistAddText(GOC_HANDLER uchwyt, char *pText)
{
	int ret;
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	ret = goc_listListener(uchwyt, GOC_MSG_LISTADDTEXT, pText, 0);
	if ( ret == GOC_ERR_OK )
		goc_nbitFieldAdd(slista->pZaznaczony, 0);
	return ret;
}

static int sellistClear(GOC_HANDLER uchwyt)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	int ret = goc_listListener(uchwyt, GOC_MSG_LISTCLEAR, NULL, 0);
	if ( ret == GOC_ERR_OK )
		goc_nbitFieldClear(slista->pZaznaczony);
	return ret;
}

static int sellistSetExt(
	GOC_HANDLER uchwyt, const char **pText, unsigned int size)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	int ret=goc_listListener(uchwyt, GOC_MSG_LISTSETEXTERNAL, pText, size);
	if ( ret == GOC_ERR_OK )
	{
		unsigned int i;
		for (i=0; i<size; i++)
			goc_nbitFieldAdd(slista->pZaznaczony, 0);
	}
	return ret;
}

int goc_sellistGetSelectPos(GOC_HANDLER uchwyt, int numer)
{
	int i;
	unsigned int tmp;
	int n = goc_listGetRowsAmmount(uchwyt);
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	for ( i = 0; i < n; i++ )
	{
		tmp = goc_nbitFieldGet(slista->pZaznaczony, i);
		if ( tmp )
		{
			if ( numer == 0 )
				return i;
			numer--;
		}
	}
	return -1;
}

int goc_sellistListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StSelList* slista = (GOC_StSelList*)malloc(sizeof(GOC_StSelList));
		memcpy(slista, elem, sizeof(GOC_StElement));
		*retuchwyt = (GOC_HANDLER)slista;
		return goc_systemSendMsg(*retuchwyt, GOC_MSG_INIT, slista, 0);
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		return sellistDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_LISTSETCOLOR )
	{
		*((GOC_COLOR*)pBuf) = goc_sellistSetColor(uchwyt, nBuf);
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_INIT )
	{
		return sellistInit(uchwyt);
	}
	else if ( wiesc == GOC_MSG_LISTADDTEXT )
	{
		return sellistAddText(uchwyt, pBuf);
	}
	else if ( wiesc == GOC_MSG_LISTCLEAR )
	{
		return sellistClear(uchwyt);
	}
	else if ( wiesc == GOC_MSG_LISTSETEXTERNAL )
	{
		return sellistSetExt(uchwyt, pBuf, nBuf);
	}
	else
		return goc_listListener(uchwyt, wiesc, pBuf, nBuf);
	return GOC_ERR_UNKNOWNMSG;
}
