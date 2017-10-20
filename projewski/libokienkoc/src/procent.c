#include <string.h>
#include <stdio.h>

#include "global-inc.h"
#include "procent.h"
#include "ramka.h"
#include "napis.h"
#include <tools/screen.h>
#include "system.h"

const char* GOC_ELEMENT_PRECENT = "GOC_Precent";

int goc_precentPaint(GOC_HANDLER uchwyt)
{
	GOC_StPrecent* procent = (GOC_StPrecent*)uchwyt;
	GOC_StElement elem;
	int n, i;
	GOC_POSITION wOff;
	memcpy( &elem, procent, sizeof(GOC_StElement) );
	elem.color = procent->kolorRamka;
	goc_elementGetFrameDrawer(uchwyt)(&elem, "[ ][ ][ ]");
	wOff = goc_elementGetWidth(uchwyt) - 2;
	if ( procent->max > procent->min )
		n = (wOff * procent->pozycja)/(procent->max-procent->min);
	else
		n = (wOff * procent->pozycja);
	goc_gotoxy(goc_elementGetScreenX( uchwyt ) + 1, goc_elementGetScreenY( uchwyt ));
	goc_textallcolor(procent->kolorJasny);
	for (i=0; i<n; i++)
		putchar(procent->znakJasny);
	n = wOff;
	goc_textallcolor(procent->kolorCiemny);
	for(; i<n; i++ )
		putchar(procent->znakCiemny);
	goc_gotoxy(goc_elementGetScreenX( uchwyt ) + 1, goc_elementGetScreenY( uchwyt ));
	fflush(stdout);
	return GOC_ERR_OK;
}

int goc_precentFocus(GOC_HANDLER uchwyt)
{
	GOC_StPrecent* procent = (GOC_StPrecent*)uchwyt;
	if ( !(procent->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	if ( !(procent->flag & GOC_EFLAGA_ENABLE) )
		return GOC_ERR_REFUSE;
	procent->kolorRamka = procent->kolorAktywny;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

int goc_precentFreeFocus(GOC_HANDLER uchwyt)
{
	GOC_StPrecent* procent = (GOC_StPrecent*)uchwyt;
	procent->kolorRamka = procent->kolorNieaktywny;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

static int precentHotKeyPlus(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StPrecent* procent = (GOC_StPrecent*)uchwyt;
	if ( procent->pozycja < procent->max ) {
		(procent->pozycja)++;
	}
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(uchwyt,  msgpaint);
	return GOC_ERR_OK;
}
static int precentHotKeyMinus(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StPrecent* procent = (GOC_StPrecent*)uchwyt;
	if ( procent->pozycja > procent->min ) {
		(procent->pozycja)--;
	}
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(uchwyt, msgpaint);
	return GOC_ERR_OK;
}

int goc_precentListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID ) {
		GOC_CREATE_ELEMENT(GOC_StPrecent, procent, msg);
		procent->znakJasny = '*';
		procent->znakCiemny = ' ';
		procent->kolorJasny = GOC_WHITE | GOC_FBOLD;
		procent->kolorCiemny = GOC_WHITE;
		procent->kolorAktywny = procent->color;
		procent->kolorNieaktywny = GOC_WHITE;
		procent->kolorRamka = procent->kolorNieaktywny;
		procent->max = 10;
		procent->min = 0;
		procent->pozycja = 0;
		goc_hkAdd( (GOC_HANDLER)procent, 0x602, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			precentHotKeyPlus );
		goc_hkAdd( (GOC_HANDLER)procent, 0x601, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			precentHotKeyMinus );
		return GOC_ERR_OK;
	} else if ( msg->id == GOC_MSG_DESTROY_ID ) {
		return goc_elementDestroy(uchwyt);
	} else if ( msg->id == GOC_MSG_PAINT_ID ) {
		return goc_precentPaint(uchwyt);
	} else if ( msg->id == GOC_MSG_FOCUS_ID ) {
		return goc_precentFocus(uchwyt);
	} else if ( msg->id == GOC_MSG_FOCUSFREE_ID ) {
		return goc_precentFreeFocus(uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}
