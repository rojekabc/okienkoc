#include "przycisk.h"
#include "ramka.h"
#include "napis.h"
#include "screen.h"
#include "mystr.h"

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
	goc_systemSendMsg((GOC_HANDLER)przycisk, GOC_MSG_PAINT, 0, 0);
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
	goc_systemSendMsg((GOC_HANDLER)przycisk, GOC_MSG_PAINT, 0, 0);
	fflush(stdout);
	return GOC_ERR_OK;
}

static int buttonHotKeyAction(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf)
{
	return goc_systemSendMsg(uchwyt, GOC_MSG_ACTION, 0, 0);
}

int goc_buttonListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf)
{
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StButton* przycisk = (GOC_StButton*)malloc(sizeof(GOC_StButton));
		memcpy(przycisk, elem, sizeof(GOC_StElement));
		przycisk->tekst = NULL;
		przycisk->kolorRamkaAktywny = 0x07;
		przycisk->kolorRamkaNieaktywny = 0x07;
		przycisk->kolorPoleAktywny = przycisk->color;
		przycisk->kolorPoleNieaktywny = 0x07;
		przycisk->kolorRamka = przycisk->kolorRamkaNieaktywny;
		przycisk->color = przycisk->kolorPoleNieaktywny;
		przycisk->typ = GOC_BUTTONTYPE_BRACKET;
		*retuchwyt = (GOC_HANDLER)przycisk;
		goc_hkAdd( *retuchwyt, 0x0D, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			buttonHotKeyAction );
		goc_hkAdd( *retuchwyt, 0x20, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			buttonHotKeyAction );
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		GOC_StButton *przycisk = (GOC_StButton*)uchwyt;
		if ( przycisk->tekst )
			free( przycisk->tekst );
		return goc_elementDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_PAINT )
	{
		return buttonPaint((GOC_StButton*)uchwyt);
	}
	else if ( wiesc == GOC_MSG_FOCUSFREE )
	{
		return goc_buttonFreeFocus((GOC_StButton*)uchwyt);
	}
	else if ( wiesc == GOC_MSG_FOCUS )
	{
		return buttonFocus((GOC_StButton*)uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}
