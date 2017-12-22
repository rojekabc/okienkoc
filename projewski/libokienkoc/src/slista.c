#include <string.h>
#include <stdio.h>
#include "slista.h"

#include <tools/screen.h>
#include "global-inc.h"
#include "system.h"

const char* GOC_ELEMENT_SELLIST = "GOC_SelectionList";

static int sellistDestroy(GOC_HANDLER uchwyt)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	slista->pZaznaczony = goc_nbitFieldRemove(slista->pZaznaczony);
	GOC_MSG_DESTROY( msg );
	return goc_listListener(uchwyt, msg);
}

static int sellistInit(GOC_HANDLER uchwyt)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	GOC_MSG_INIT( msg );
	goc_listListener(uchwyt, msg);
	slista->kolorPoleZaznaczony = GOC_GREEN;
	slista->kolorPoleKursorZaznaczony = GOC_GREEN | GOC_FBOLD;
	slista->pZaznaczony = goc_nbitFieldCreate(1, 0);
	return GOC_ERR_OK;
}

int goc_sellistUnselect(GOC_HANDLER uchwyt, int pos)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	if ( goc_arrayIsEmpty( &slista->columns ) ) {
		return GOC_ERR_REFUSE;
	}
	goc_nbitFieldSet(slista->pZaznaczony, pos, 0);
	return GOC_ERR_OK;
}

int goc_sellistSelect(GOC_HANDLER uchwyt, int pos)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	if ( goc_arrayIsEmpty( &slista->columns ) ) {
		return GOC_ERR_REFUSE;
	}
	goc_nbitFieldSet(slista->pZaznaczony, pos, 1);
	return GOC_ERR_OK;
}

GOC_BOOL goc_sellistIsSelected(GOC_HANDLER uchwyt, int pos)
{
	int tmp;
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	if ( goc_arrayIsEmpty( &slista->columns ) ) {
		return GOC_FALSE;
	}
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
	if ( goc_arrayIsEmpty( &slista->columns ) ) {
		return slista->color;
	}
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

static int sellistAddText(GOC_HANDLER uchwyt, void* element)
{
	int ret;
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	GOC_MSG_LISTADD( msg, element );
	ret = goc_listListener(uchwyt, msg);
	if ( ret == GOC_ERR_OK )
		goc_nbitFieldAdd(slista->pZaznaczony, 0);
	return ret;
}

static int sellistClear(GOC_HANDLER uchwyt)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	GOC_MSG_LISTCLEAR( msg );
	int ret = goc_listListener(uchwyt, msg);
	if ( ret == GOC_ERR_OK )
		goc_nbitFieldClear(slista->pZaznaczony);
	return ret;
}

static int sellistSetExt(
	GOC_HANDLER uchwyt, const char **pText, unsigned int size)
{
	GOC_StSelList *slista = (GOC_StSelList*)uchwyt;
	GOC_MSG_LISTSETEXTERNAL( msg, pText, size );
	int ret=goc_listListener(uchwyt, msg);
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

int goc_sellistListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID ) {
		GOC_CREATE_ELEMENT(GOC_StSelList, slista, msg);
		GOC_MSG_INIT( msginit );
		return goc_systemSendMsg((GOC_HANDLER)slista, msginit);
	} else if ( msg->id == GOC_MSG_DESTROY_ID ) {
		return sellistDestroy(uchwyt);
	} else if ( msg->id == GOC_MSG_LISTSETCOLOR_ID ) {
		GOC_StMsgListSetColor* msgcolor = (GOC_StMsgListSetColor*)msg;
		msgcolor->color = goc_sellistSetColor(uchwyt, msgcolor->position);
		return GOC_ERR_OK;
	} else if ( msg->id == GOC_MSG_INIT_ID ) {
		return sellistInit(uchwyt);
	} else if ( msg->id == GOC_MSG_LISTADD_ID ) {
		GOC_StMsgListAdd* msgadd = (GOC_StMsgListAdd*) msg;
		return sellistAddText(uchwyt, msgadd->element);
	} else if ( msg->id == GOC_MSG_LISTCLEAR_ID ) {
		return sellistClear(uchwyt);
	} else if ( msg->id == GOC_MSG_LISTSETEXTERNAL_ID ) {
		GOC_StMsgListSetExternal* msgext = (GOC_StMsgListSetExternal*)msg;
		return sellistSetExt(uchwyt, msgext->pTextArray, msgext->nTextArray);
	} else {
		return goc_listListener(uchwyt, msg);
	}
	return GOC_ERR_UNKNOWNMSG;
}
