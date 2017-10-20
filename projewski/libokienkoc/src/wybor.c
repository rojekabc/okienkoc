#include <string.h>
#include <stdio.h>
#include "wybor.h"
#include <tools/screen.h>
#include "napis.h"
#include <tools/mystr.h>
#include "global-inc.h"
#include "system.h"

const char* GOC_ELEMENT_CHOICE = "GOC_Choice";

int goc_choiceSetText(GOC_HANDLER uchwyt, const char *tekst)
{
	GOC_StChoice *wybor = (GOC_StChoice*)uchwyt;
	wybor->tekst = goc_stringCopy(wybor->tekst, tekst);
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

static int choicePaint(GOC_HANDLER uchwyt)
{
	char buf[4];
	GOC_StChoice *wybor = (GOC_StChoice*)uchwyt;
	GOC_StElement elem;

	if ( !(wybor->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	memcpy(&elem, wybor, sizeof(GOC_StElement));
	elem.x += 4;
	elem.width -= 4;
	goc_elementGetLabelDrawer(uchwyt)(&elem, wybor->tekst);
	goc_textallcolor(wybor->kolorKlamry);
	goc_gotoxy(goc_elementGetScreenX( uchwyt ), goc_elementGetScreenY( uchwyt ));
	sprintf(buf, "%c%c%c", wybor->typ[0], ' ', wybor->typ[1]);
	fputs(buf, stdout);
	goc_gotoxy(goc_elementGetScreenX( uchwyt )+1, goc_elementGetScreenY( uchwyt ));
	goc_textallcolor(wybor->kolorOdp);
	putc(wybor->stan == GOC_TRUE ? wybor->typ[2] : wybor->typ[3], stdout);
	goc_gotoxy(goc_elementGetScreenX( uchwyt )+1, goc_elementGetScreenY( uchwyt ));
	fflush(stdout);
	return GOC_ERR_OK;
}

static int choiceFocus(GOC_HANDLER uchwyt)
{
	GOC_StChoice *wybor = (GOC_StChoice*)uchwyt;
	if ( !(wybor->flag & GOC_EFLAGA_ENABLE) )
		return GOC_ERR_REFUSE;
	if ( !(wybor->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	wybor->color = wybor->kolorPoleAktywny;
	wybor->kolorOdp = wybor->kolorOdpAktywny;
	wybor->kolorKlamry = wybor->kolorKlamryAktywny;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

static int choiceFreeFocus(GOC_HANDLER uchwyt)
{
	GOC_StChoice *wybor = (GOC_StChoice*)uchwyt;
	wybor->color = wybor->kolorPoleNieaktywny;
	wybor->kolorOdp = wybor->kolorOdpNieaktywny;
	wybor->kolorKlamry = wybor->kolorKlamryNieaktywny;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

static int choiceHotKeyAction(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_MSG_ACTION( msgaction );
	goc_systemSendMsg(uchwyt, msgaction);
	return GOC_ERR_OK;
}

static int choiceAction(GOC_HANDLER uchwyt)
{
	GOC_StChoice *wybor = (GOC_StChoice*)uchwyt;
	if ( wybor->stan == GOC_TRUE )
		wybor->stan = GOC_FALSE;
	else
		wybor->stan = GOC_TRUE;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg(uchwyt, msg);
	return GOC_ERR_OK;
}

int goc_choiceListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID ) {
		GOC_CREATE_ELEMENT(GOC_StChoice, wybor, msg);
		wybor->stan = GOC_FALSE;
		memcpy(wybor->typ, GOC_TYPE_CHOICEROUND, 2);
		memcpy(&wybor->typ[2], GOC_TYPE_CHOICEASTERIX, 2);
		wybor->kolorPoleAktywny = wybor->color;
		wybor->kolorPoleNieaktywny = wybor->color;
		wybor->kolorOdpAktywny = GOC_WHITE | GOC_FBOLD;
		wybor->kolorOdpNieaktywny = GOC_WHITE | GOC_FBOLD;
		wybor->kolorKlamryAktywny = GOC_WHITE | GOC_FBOLD;
		wybor->kolorKlamryNieaktywny = GOC_WHITE;
		wybor->kolorKlamry = wybor->kolorKlamryNieaktywny;
		wybor->kolorOdp = wybor->kolorOdpNieaktywny;
		wybor->tekst = NULL;
		goc_hkAdd((GOC_HANDLER)wybor, 0x20, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			choiceHotKeyAction);
		return GOC_ERR_OK;
	} else if ( msg->id == GOC_MSG_DESTROY_ID ) {
		GOC_StChoice *wybor = (GOC_StChoice*)uchwyt;
		if ( wybor->tekst )
			free( wybor->tekst );
		return goc_elementDestroy(uchwyt);
	} else if ( msg->id == GOC_MSG_PAINT_ID ) {
		return choicePaint(uchwyt);
	} else if ( msg->id == GOC_MSG_FOCUS_ID ) {
		return choiceFocus(uchwyt);
	} else if ( msg->id == GOC_MSG_FOCUSFREE_ID ) {
		return choiceFreeFocus(uchwyt);
	} else if ( msg->id == GOC_MSG_ACTION_ID ) {
		return choiceAction(uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}
