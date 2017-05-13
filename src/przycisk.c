#include "przycisk.h"
#include "ramka.h"
#include "napis.h"
#include "screen.h"
#include "mystr.h"
#include "system.h"

#include <string.h>
#include <stdio.h>
#include "global-inc.h"

const char* GOC_ELEMENT_BUTTON = "GOC_Button";

int goc_buttonSetType(GOC_HANDLER u, const char *typ)
{
	GOC_StButton *przycisk = (GOC_StButton*)u;
	przycisk->typ = typ;
	return GOC_ERR_OK;
}

int goc_buttonSetText(GOC_HANDLER u, const char *tekst)
{
	GOC_StButton *przycisk = (GOC_StButton*)u;
	przycisk->tekst = goc_stringCopy(przycisk->tekst, tekst);
	return GOC_ERR_OK;
}

static int buttonPaint(GOC_StButton* przycisk)
{
	GOC_StElement elem;
	if ( !(przycisk->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_OK;
	memcpy( &elem, przycisk, sizeof(GOC_StElement) );
	elem.color = przycisk->kolorRamka;
	goc_elementGetFrameDrawer( (GOC_HANDLER)przycisk )( &elem, przycisk->typ);
	elem.color = przycisk->color;
	(elem.x)++;
	(elem.width)--;
	(elem.width)--;
	elem.y += elem.height/2;
	elem.height = 1;
	goc_elementGetLabelDrawer( (GOC_HANDLER)przycisk )( &elem, przycisk->tekst );
	fflush(stdout);
	return GOC_ERR_OK;
}

static int goc_buttonFreeFocus(GOC_StButton *przycisk)
{
	przycisk->color = przycisk->kolorPoleNieaktywny;
	przycisk->kolorRamka = przycisk->kolorRamkaNieaktywny;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)przycisk, msg);
	return GOC_ERR_OK;
}

static int buttonFocus(GOC_StButton *przycisk)
{
	if ( !(przycisk->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	else if ( !(przycisk->flag & GOC_EFLAGA_ENABLE) )
		return GOC_ERR_REFUSE;
	przycisk->color = przycisk->kolorPoleAktywny;
	przycisk->kolorRamka = przycisk->kolorRamkaAktywny;
	goc_gotoxy(
		goc_elementGetScreenX( (GOC_HANDLER)przycisk ) + 1,
		goc_elementGetScreenY( (GOC_HANDLER)przycisk ) );
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)przycisk, msg);
	fflush(stdout);
	return GOC_ERR_OK;
}

static int buttonHotKeyAction(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_MSG_ACTION( msgaction );
	return goc_systemSendMsg(uchwyt, msgaction);
}

int goc_buttonListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT(GOC_StButton, przycisk, msg);
		przycisk->tekst = NULL;
		przycisk->kolorRamkaAktywny = 0x07;
		przycisk->kolorRamkaNieaktywny = 0x07;
		przycisk->kolorPoleAktywny = przycisk->color;
		przycisk->kolorPoleNieaktywny = 0x07;
		przycisk->kolorRamka = przycisk->kolorRamkaNieaktywny;
		przycisk->color = przycisk->kolorPoleNieaktywny;
		przycisk->typ = GOC_BUTTONTYPE_BRACKET;
		goc_hkAdd( (GOC_HANDLER)przycisk, 0x0D, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			buttonHotKeyAction );
		goc_hkAdd( (GOC_HANDLER)przycisk, 0x20, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			buttonHotKeyAction );
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		GOC_StButton *przycisk = (GOC_StButton*)uchwyt;
		if ( przycisk->tekst )
			free( przycisk->tekst );
		return goc_elementDestroy(uchwyt);
	}
	else if ( msg->id == GOC_MSG_PAINT_ID )
	{
		return buttonPaint((GOC_StButton*)uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUSFREE_ID )
	{
		return goc_buttonFreeFocus((GOC_StButton*)uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUS_ID )
	{
		return buttonFocus((GOC_StButton*)uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}
