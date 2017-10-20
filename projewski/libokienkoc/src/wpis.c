#include <string.h>
#include <stdio.h>
#include "wpis.h"
#include <tools/screen.h>
#include <tools/mystr.h>
#include "hotkey.h"
#include "global-inc.h"
#include "system.h"

const char* GOC_ELEMENT_EDIT = "GOC_Edit";

int goc_editRemText( GOC_HANDLER u )
{
	GOC_StEdit *wpis = (GOC_StEdit*)u;
	wpis->tekst = goc_stringFree(wpis->tekst);
	wpis->start = 0;
	wpis->kursor = 0;
	return GOC_ERR_OK;
}

const char *goc_editGetText( GOC_HANDLER u )
{
	GOC_StEdit *wpis = (GOC_StEdit*)u;
	return wpis->tekst;
}

int goc_editSetText( GOC_HANDLER u, const char *tekst )
{
	GOC_StEdit *wpis = (GOC_StEdit*)u;
	wpis->tekst = goc_stringCopy(wpis->tekst, tekst);
	wpis->start = 0;
	return GOC_ERR_OK;
}

static int editFocus(GOC_StEdit *wpis)
{
	if ( !(wpis->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	if ( !(wpis->flag & GOC_EFLAGA_ENABLE) )
		return GOC_ERR_REFUSE;
	wpis->kursor = 0;
	wpis->start = 0;
	wpis->color = wpis->kolorPolaZaznaczony;
	wpis->flag |= GOC_FLG_EDITSELECTED;
	wpis->kolorRamki = wpis->kolorRamkiAktywny;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	goc_gotoxy(
		goc_elementGetScreenX( (GOC_HANDLER)wpis ) + 1,
		goc_elementGetScreenY( (GOC_HANDLER)wpis ));
	fflush(stdout);
	return GOC_ERR_OK;
}

static int editFreeFocus(GOC_StEdit *wpis)
{
	wpis->kolorRamki = wpis->kolorRamkiNieaktywny;
	wpis->color = wpis->kolorPolaNieaktywny;
	wpis->flag &= ~GOC_FLG_EDITSELECTED;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	fflush(stdout);
	return GOC_ERR_OK;
}

static void editRemChar(GOC_StEdit *wpis, int pozycja)
{
	wpis->tekst = goc_stringDelAtPos(wpis->tekst, pozycja);
}

static void editInsertChar(GOC_StEdit *wpis, int pozycja, char znak)
{
	wpis->tekst = goc_stringInsertCharAt(wpis->tekst, pozycja, znak);
}

static int editHotKeyHome(GOC_HANDLER uchwyt, GOC_StMessage* msghk)
{
	GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
	GOC_POSITION wOff;

	wpis->kursor = 0;
	if (wpis->flag & GOC_FLG_EDITSELECTED)
	{
		wpis->color = wpis->kolorPolaAktywny;
		wpis->flag &= ~GOC_FLG_EDITSELECTED;
	}
	wOff = goc_elementGetWidth( uchwyt ) - 3;
	if (wpis->kursor - wpis->start > wOff)
	  	wpis->start = wpis->kursor - wOff;
	if (wpis->kursor < wpis->start)
	  	wpis->start = wpis->kursor;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	return GOC_ERR_OK;
}
static int editHotKeyDel(GOC_HANDLER uchwyt, GOC_StMessage* msghk)
{
	GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
	GOC_POSITION wOff;

	if (wpis->flag & GOC_FLG_EDITSELECTED)
		goc_editRemText((GOC_HANDLER)wpis);
	  else
		editRemChar(wpis, wpis->kursor);
	if (wpis->flag & GOC_FLG_EDITSELECTED)
	{
		wpis->color = wpis->kolorPolaAktywny;
		wpis->flag &= ~GOC_FLG_EDITSELECTED;
	}
	wOff = goc_elementGetWidth( uchwyt ) - 3;
	if (wpis->kursor - wpis->start > wOff)
	  	wpis->start = wpis->kursor - wOff;
	if (wpis->kursor < wpis->start)
	  	wpis->start = wpis->kursor;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	return GOC_ERR_OK;
}
static int editHotKeyEnd(GOC_HANDLER uchwyt, GOC_StMessage* msghk)
{
	GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
	GOC_POSITION wOff;

	wpis->kursor = goc_stringLength(wpis->tekst);
	if (wpis->flag & GOC_FLG_EDITSELECTED)
	{
		wpis->color = wpis->kolorPolaAktywny;
		wpis->flag &= ~GOC_FLG_EDITSELECTED;
	}
	wOff = goc_elementGetWidth( uchwyt ) - 3;
	if (wpis->kursor - wpis->start > wOff)
	  	wpis->start = wpis->kursor - wOff;
	if (wpis->kursor < wpis->start)
	  	wpis->start = wpis->kursor;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	return GOC_ERR_OK;
}

static int editHotKeyRight(GOC_HANDLER uchwyt, GOC_StMessage* msghk)
{
	GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
	GOC_POSITION wOff;

	if (wpis->kursor < goc_stringLength(wpis->tekst))
	  	(wpis->kursor)++;
	if (wpis->flag & GOC_FLG_EDITSELECTED)
	{
		wpis->color = wpis->kolorPolaAktywny;
		wpis->flag &= ~GOC_FLG_EDITSELECTED;
	}
	wOff = goc_elementGetWidth( uchwyt ) - 3;
	if (wpis->kursor - wpis->start > wOff)
	  	wpis->start = wpis->kursor - wOff;
	if (wpis->kursor < wpis->start)
	  	wpis->start = wpis->kursor;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	return GOC_ERR_OK;
}

static int editHotKeyLeft(GOC_HANDLER uchwyt, GOC_StMessage* msghk)
{
	GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
	GOC_POSITION wOff;

	if (wpis->kursor > 0)
	  	(wpis->kursor)--;
	if (wpis->flag & GOC_FLG_EDITSELECTED)
	{
		wpis->color = wpis->kolorPolaAktywny;
		wpis->flag &= ~GOC_FLG_EDITSELECTED;
	}
	wOff = goc_elementGetWidth( uchwyt ) - 3;
	if (wpis->kursor - wpis->start > wOff)
	  	wpis->start = wpis->kursor - wOff;
	if (wpis->kursor < wpis->start)
	  	wpis->start = wpis->kursor;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	return GOC_ERR_OK;
}

static int editHotKeyBacksp(GOC_HANDLER uchwyt, GOC_StMessage* msghk)
{
	GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
	GOC_POSITION wOff;

	if (wpis->kursor)
	{
		(wpis->kursor)--;
	  	editRemChar(wpis, wpis->kursor);
	}
	if (wpis->flag & GOC_FLG_EDITSELECTED)
	{
		wpis->color = wpis->kolorPolaAktywny;
		wpis->flag &= ~GOC_FLG_EDITSELECTED;
	}
	wOff = goc_elementGetWidth( uchwyt ) - 3;
	if (wpis->kursor - wpis->start > wOff)
	  	wpis->start = wpis->kursor - wOff;
	if (wpis->kursor < wpis->start)
	  	wpis->start = wpis->kursor;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	return GOC_ERR_OK;
}

static int editGetChar(GOC_StEdit *wpis, int znak)
{
	GOC_POSITION wOff;

	if (znak & 0xFFFFFF00)
		return GOC_ERR_UNKNOWNMSG;
  	else
	{
		if (znak < 32)
		  	return GOC_ERR_UNKNOWNMSG;
		if (wpis->flag & GOC_FLG_EDITSELECTED)
			goc_editRemText((GOC_HANDLER)wpis);
		/* 0x7F - Backspace */
		if (znak != 0x7F)
	  	{
			editInsertChar(wpis, wpis->kursor, (char)znak);
			(wpis->kursor)++;
		}
	}
	if (wpis->flag & GOC_FLG_EDITSELECTED)
	{
		wpis->color = wpis->kolorPolaAktywny;
		wpis->flag &= ~GOC_FLG_EDITSELECTED;
	}
	wOff = goc_elementGetWidth( (GOC_HANDLER)wpis ) - 3;
	if (wpis->kursor - wpis->start > wOff)
	  	wpis->start = wpis->kursor - wOff;
	if (wpis->kursor < wpis->start)
	  	wpis->start = wpis->kursor;
	GOC_MSG_PAINT( msg );
	goc_systemSendMsg((GOC_HANDLER)wpis, msg);
	return GOC_ERR_OK;
}

int goc_editListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT(GOC_StEdit, wpis, msg);
		wpis->kursor = 0;
		wpis->start = 0;
		wpis->kolorRamkiNieaktywny = GOC_WHITE;
		wpis->kolorRamkiAktywny = GOC_WHITE | GOC_FBOLD;
		wpis->kolorPolaAktywny = wpis->color;
		wpis->kolorPolaNieaktywny = GOC_WHITE;
		wpis->kolorPolaZaznaczony = GOC_BWHITE;
		wpis->kolorRamki = wpis->kolorRamkiNieaktywny;
		wpis->tekst = 0;
		goc_hkAdd((GOC_HANDLER)wpis, 0x114, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			editHotKeyHome);
		goc_hkAdd((GOC_HANDLER)wpis, 0x116, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			editHotKeyDel);
		goc_hkAdd((GOC_HANDLER)wpis, 0x117, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			editHotKeyEnd);
		goc_hkAdd((GOC_HANDLER)wpis, 0x602, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			editHotKeyRight);
		goc_hkAdd((GOC_HANDLER)wpis, 0x601, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			editHotKeyLeft);
		goc_hkAdd((GOC_HANDLER)wpis, 0x7F, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			editHotKeyBacksp);
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_PAINT_ID )
	{
		GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
		GOC_StElement elem;
		if ( !(wpis->flag & GOC_EFLAGA_PAINTED) )
			return GOC_ERR_OK;
		memcpy( &elem, wpis, sizeof(GOC_StElement) );
		elem.color = wpis->kolorRamki;
		goc_elementGetFrameDrawer(uchwyt)( &elem, "[ ][ ][ ]" );
		elem.x++;
		elem.width--;
		elem.width--;
		elem.height=1;
		elem.color = wpis->color;
		goc_elementGetLabelDrawer(uchwyt)(
			&elem, &wpis->tekst[wpis->start] );
		goc_gotoxy(
			goc_elementGetScreenX( uchwyt ) + 1 + wpis->kursor - wpis->start,
			goc_elementGetScreenY( uchwyt ) );
		fflush(stdout);
		return GOC_ERR_OK;
//		return labelPaint((GOC_StLabel*)uchwyt);
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		GOC_StEdit *wpis = (GOC_StEdit*)uchwyt;
		wpis->tekst = goc_stringFree( wpis->tekst );
		return goc_elementDestroy(uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUS_ID )
	{
		return editFocus((GOC_StEdit*)uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUSFREE_ID )
	{
		return editFreeFocus((GOC_StEdit*)uchwyt);
	}
	else if ( msg->id == GOC_MSG_CHAR_ID )
	{
		GOC_StMsgChar* msgchar = (GOC_StMsgChar*)msg;
		return editGetChar((GOC_StEdit*)uchwyt, msgchar->charcode);
	}
	return GOC_ERR_UNKNOWNMSG;
}

