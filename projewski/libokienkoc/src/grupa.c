#include "global-inc.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include <stdio.h>

#include "grupa.h"
#include <tools/screen.h>
#include "hotkey.h"
#include "system.h"

const char* GOC_ELEMENT_GROUP = "GOC_Group";

static int groupPaint(GOC_HANDLER uchwyt)
{
	GOC_HANDLER c;
	GOC_BOOL start = GOC_TRUE;
	GOC_StGroup *grupa = (GOC_StGroup*)uchwyt;
	if ( !(grupa->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	while ( goc_systemFindChildNext( uchwyt, &c, start ) == GOC_ERR_OK )
	{
		GOC_MSG_PAINT( msg );
		goc_systemSendMsg( c, msg);
		start = GOC_FALSE;
	}
	return GOC_ERR_OK;
}
/*
int grupaHotKeyTab(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf)
{
	return goc_systemSendMsg(uchwyt, GOC_MSGFOCUSNEXT, 0, 0);
}
*/
int goc_groupListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
#ifdef _DEBUG
	fprintf(stderr, "%s:%d goc_groupListener.\n", __FILE__, __LINE__);
#endif
	if ( msg->id == GOC_MSG_CREATE_ID ) {
		GOC_CREATE_ELEMENT(GOC_StGroup, grupa, msg);
//		goc_hkAdd( *retuchwyt, 9, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
//			grupaHotKeyTab );
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		return goc_elementDestroy(uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUS_ID )
	{
		GOC_StGroup *grupa = (GOC_StGroup*)uchwyt;
		if ( !(grupa->flag & GOC_EFLAGA_ENABLE) )
			return GOC_ERR_REFUSE;
		// poniewa¿ jest wyszukiwanie elementu, który mo¿e
		// przej±æ focus przy pierwszym wywo³aniu jest zaznaczane,
		// ¿e element ten przejmuje focus. Przy nastêpnym nastêpuje
		// jego zwolnienie
		if ( grupa->flag & GOC_FORMFLAGFOCUS )
			return GOC_ERR_REFUSE;
		grupa->flag |= GOC_FORMFLAGFOCUS;
		goc_systemFocusNext(uchwyt);
//		goc_textallcolor(GOC_BLACK|GOC_BBLACK);
//		goc_clearscreen();
//		groupPaint(uchwyt);
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_FOCUSFREE_ID )
	{
		GOC_StGroup *grupa = (GOC_StGroup*)uchwyt;
		grupa->flag &= ~GOC_FORMFLAGFOCUS;
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_FOCUSNEXT_ID )
	{
	// TODO: sprawdzenie, czy focus jest na ostatnim elemencie grupy i jesli
	// tak to wypuszczenie (w zale¿no¶ci od ustawionej flagi - mo¿e byæ
	// flag blokowania, która zabroni wypuszczania zdarzeñ poza obszar
	// grupy, dopóty bêdzie ona istnia³a)
		if ( goc_systemFocusNextEnd(uchwyt) == GOC_ERR_REFUSE )
		{
			GOC_StGroup *grupa = (GOC_StGroup*)uchwyt;
			if ( goc_systemFocusNext( grupa->ojciec ) == GOC_ERR_OK )
			{
				grupa->flag &= ~GOC_FORMFLAGFOCUS;
				return GOC_ERR_OK;
			}
			else
			{
				// pozostaw focus przy grupie, ale zacznij od
				// nowa obrot
				goc_systemFocusNext(uchwyt);
			}
			return GOC_ERR_OK;
		}
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_FOCUSPREV_ID )
	{
		goc_systemFocusPrev(uchwyt);
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_PAINT_ID )
	{
		return groupPaint(uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}
