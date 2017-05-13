#include <stdio.h>
#include <string.h>

#include "global-inc.h"
#include "pasek.h"
#include "napis.h"
#include "tablica.h"
#include "screen.h"
#include "system.h"

const char* GOC_ELEMENT_BAR = "GOC_Bar";

int goc_barAddText(GOC_HANDLER uchwyt, const char *tekst)
{
	GOC_StBar* pasek = (GOC_StBar*)uchwyt;
	pasek->tekst = goc_tableAdd(pasek->tekst, &(pasek->nText), sizeof(char*));
	pasek->tekst[pasek->nText-1] = strdup(tekst);
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

static int barPaint(GOC_HANDLER uchwyt)
{
	GOC_StBar *pasek = (GOC_StBar*)uchwyt;
	int i;
	GOC_StElement elem;
	GOC_POSITION w = goc_elementGetWidth(uchwyt);
	memcpy(&elem, pasek, sizeof(GOC_StElement));
	goc_elementGetLabelDrawer(uchwyt)(&elem, "");
	if ( pasek->nText == 0 )
		return GOC_ERR_OK;
	if ( pasek->rozmiar == GOC_BAR_COUNTEDSIZE )
		elem.width = w / pasek->nText;
	else
		elem.width = pasek->rozmiar;
	for ( i=pasek->start; i<pasek->nText; i++ )
	{
		if ( i<(elem.width%(pasek->nText)) )
			elem.width++;
		if ( elem.x + elem.width > pasek->x + w )
			elem.width = pasek->x + w - elem.x;
		if ( i != pasek->kursor )
			elem.color = pasek->color;
		else
			elem.color = pasek->kolorZaznaczony;
		goc_elementGetLabelDrawer(uchwyt)(&elem, pasek->tekst[i]);
		elem.x += elem.width;
		if ( i<(elem.width%(pasek->nText)) )
		{
			elem.x++;
			elem.width--;
		}
	}
	fflush(stdout);
	return GOC_ERR_OK;
}

// TODO: Przemieszczenie punktu startowego przy ilosci pól wykraczaj±cych
// poza dostêpny obszar
static int barHotKeyNext(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StBar *pasek = (GOC_StBar*)uchwyt;
	if ( pasek->kursor < (int)(pasek->nText-1) )
		pasek->kursor++;
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(uchwyt, msgpaint);
	return GOC_ERR_OK;
}
// TODO: Przemieszczenie punktu startowego przy ilosci pól wykraczaj±cych
// poza dostêpny obszar
static int barHotKeyPrev(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StBar *pasek = (GOC_StBar*)uchwyt;
	if ( pasek->kursor > 0 )
		(pasek->kursor)--;
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(uchwyt, msgpaint);
	return GOC_ERR_OK;
}

static int barFocus(GOC_HANDLER uchwyt)
{
	GOC_StBar *pasek = (GOC_StBar*)uchwyt;
	if ( !(pasek->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	if ( !(pasek->flag & GOC_EFLAGA_ENABLE) )
		return GOC_ERR_REFUSE;
	pasek->color = pasek->kolorAktywny;
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(uchwyt, msgpaint);
	return GOC_ERR_OK;
}

static int barFreeFocus(GOC_HANDLER uchwyt)
{
	GOC_StBar *pasek = (GOC_StBar*)uchwyt;
	pasek->color = pasek->kolorNieaktywny;
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(uchwyt, msgpaint);
	return GOC_ERR_OK;
}

int goc_barListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT(GOC_StBar, pasek, msg);
		pasek->tekst = NULL;
		pasek->nText = 0;
		pasek->start = 0;
		pasek->kursor = -1;
		pasek->kolorNieaktywny = GOC_WHITE;
		pasek->kolorAktywny = pasek->color;
		pasek->kolorZaznaczony = GOC_WHITE | GOC_FBOLD;
		pasek->rozmiar = GOC_BAR_COUNTEDSIZE;
		pasek->color = pasek->kolorNieaktywny;
		goc_hkAdd((GOC_HANDLER)pasek, 0x600, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			barHotKeyNext);
		goc_hkAdd((GOC_HANDLER)pasek, 0x601, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			barHotKeyPrev);
		goc_hkAdd((GOC_HANDLER)pasek, 0x602, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			barHotKeyNext);
		goc_hkAdd((GOC_HANDLER)pasek, 0x603, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			barHotKeyPrev);
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		GOC_StBar *pasek = (GOC_StBar*)uchwyt;
		int i;
		for (i=0; i<pasek->nText; i++)
			free(pasek->tekst[i]);
		pasek->tekst = goc_tableClear(pasek->tekst, &(pasek->nText));
		return goc_elementDestroy(uchwyt);
	}
	else if ( msg->id == GOC_MSG_PAINT_ID )
	{
		return barPaint(uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUS_ID )
	{
		return barFocus(uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUSFREE_ID )
	{
		return barFreeFocus(uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}
