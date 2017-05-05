#include "global-inc.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include <stdio.h>

#include "grupa.h"
#include "screen.h"
#include "hotkey.h"

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
		goc_systemSendMsg( c, GOC_MSG_PAINT, 0, 0);
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
int goc_groupListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
#ifdef _DEBUG
	fprintf(stderr, "%s:%d goc_groupListener.\n", __FILE__, __LINE__);
#endif
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StGroup* grupa = (GOC_StGroup*)malloc(sizeof(GOC_StGroup));
		memcpy(grupa, elem, sizeof(GOC_StElement));
		*retuchwyt = (GOC_HANDLER)grupa;
//		goc_hkAdd( *retuchwyt, 9, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
//			grupaHotKeyTab );
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		return goc_elementDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_FOCUS )
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
	else if ( wiesc == GOC_MSG_FOCUSFREE )
	{
		GOC_StGroup *grupa = (GOC_StGroup*)uchwyt;
		grupa->flag &= ~GOC_FORMFLAGFOCUS;
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSGFOCUSNEXT )
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
	else if ( wiesc == GOC_MSG_FOCUSPREV )
	{
		goc_systemFocusPrev(uchwyt);
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_PAINT )
	{
		return groupPaint(uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}
