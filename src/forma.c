#include "global-inc.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include <stdio.h>

#include "forma.h"
#include "screen.h"
#include "hotkey.h"
#include "conflog.h"

const char* GOC_ELEMENT_FORM = "GOC_Form";

static int goc_formPaint(GOC_HANDLER uchwyt)
{
	GOC_HANDLER c;
	GOC_BOOL start = GOC_TRUE;
	GOC_StForm *forma = (GOC_StForm*)uchwyt;
	GOC_DEBUG("-> goc_formPaint");
	if ( !(forma->flag & GOC_EFLAGA_PAINTED) )
	{
		GOC_DEBUG("<- goc_formPaint");
		return GOC_ERR_REFUSE;
	}
	while ( goc_systemFindChildNext( uchwyt, &c, start ) == GOC_ERR_OK )
	{
		goc_systemSendMsg( c, GOC_MSG_PAINT, 0, 0);
		start = GOC_FALSE;
	}
	GOC_DEBUG("<- goc_formPaint");
	return GOC_ERR_OK;
}

int goc_formHotKeyEsc(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf)
{
	int ret;
	GOC_DEBUG("-> goc_formHotKeyEsc");
	ret = goc_formHide(uchwyt);
	GOC_DEBUG("<- goc_formHotKeyEsc");
	return ret;
}

int goc_formHotKeyTab(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf)
{
	int ret;
	GOC_DEBUG("-> goc_formHotKeyTab");
	ret = goc_systemSendMsg(uchwyt, GOC_MSGFOCUSNEXT, 0, 0);
	GOC_DEBUG("<- goc_formHotKeyTab");
	return ret;
}

// skrocona func, ktora uaktywnia flagi i pokazuje formularz, przekazujac
// mu focus
int goc_formShow(GOC_HANDLER uchwyt)
{
	GOC_StForm *forma = (GOC_StForm*)uchwyt;
	GOC_DEBUG("-> goc_formShow");
	forma->flag |= GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED;
	forma->focus = goc_systemGetFocusedElement();
	goc_systemFocusOn(uchwyt);
	fflush(stdout);
	GOC_DEBUG("<- goc_formShow");
	return GOC_ERR_OK;
}

// pokaz formularz i rownoczesnie rozpocznij oczekiwanie na zakonczenie jego
// aktywnosci. Po tym momencie program przejdzie dalej. W przeciwnym wypadku bedzie
// czekal dopoki jedna z podanych flag bedzie ustawiona
int goc_formShowLock(GOC_HANDLER uchwyt, int flags)
{
	GOC_MSG wiesc;
	GOC_StForm *forma = (GOC_StForm*)uchwyt;
	GOC_DEBUG("-> goc_formShowLock");
	goc_formShow(uchwyt);
	while ( (forma->flag & flags) )
	{
		goc_systemCheckMsg( & wiesc );
		if ( goc_systemIsChildExists( uchwyt ) != GOC_ERR_OK )
			break;
	}
	GOC_DEBUG("<- goc_formShowLock");
	return GOC_ERR_OK;
}

// skrocona func, ktora deaktywuje flagi i chowa formularz, odbierajac
// mu focus
int goc_formHide(GOC_HANDLER uchwyt)
{
	GOC_StForm *forma = (GOC_StForm*)uchwyt;
	GOC_HANDLER uojc;
	GOC_DEBUG("-> goc_formHide");
	uojc = forma->ojciec;
	forma->flag &= ~(GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED | GOC_FORMFLAGFOCUS);
	goc_systemFocusOn(forma->focus);
//	goc_systemFocusNext(uojc);
	goc_systemRepaintUnder(uchwyt);
	GOC_DEBUG("<- goc_formHide");
	return GOC_ERR_OK;
}

int goc_formListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf)
{
	int ret = GOC_ERR_OK;
	GOC_DEBUG("-> goc_formListener");
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StForm* forma = (GOC_StForm*)malloc(sizeof(GOC_StForm));
		memcpy(forma, elem, sizeof(GOC_StElement));
		*retuchwyt = (GOC_HANDLER)forma;
		goc_hkAdd( *retuchwyt, 27, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			goc_formHotKeyEsc );
		goc_hkAdd( *retuchwyt, 9, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			goc_formHotKeyTab );
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		ret = goc_elementDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_FOCUS )
	{
		GOC_StForm *forma = (GOC_StForm*)uchwyt;
		if ( !(forma->flag & GOC_EFLAGA_ENABLE) )
			ret = GOC_ERR_REFUSE;
		if ( forma->flag & GOC_FORMFLAGFOCUS )
			ret = GOC_ERR_REFUSE;
		if ( ret == GOC_ERR_OK )
		{
			forma->flag |= GOC_FORMFLAGFOCUS;
			goc_systemFocusNext(uchwyt);
			goc_systemClearArea(uchwyt);
			goc_formPaint(uchwyt);
		}
	}
	else if ( wiesc == GOC_MSG_FOCUSFREE )
	{
		ret = GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSGFOCUSNEXT )
	{
		goc_systemFocusNext(uchwyt);
		ret = GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_FOCUSPREV )
	{
		goc_systemFocusPrev(uchwyt);
		ret = GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_PAINT )
	{
		ret = goc_formPaint(uchwyt);
	}
	else if ( wiesc == GOC_MSG_FINISH )
	{
		GOC_StForm *forma = (GOC_StForm*)uchwyt;
		if ( forma->flag & GOC_FORMFLAGFOCUS )
		{
			ret = goc_formHide(uchwyt);
		}
		else
		{
			goc_systemDestroyElement(uchwyt);
		}
	}
	else if ( wiesc == GOC_MSG_CHAR )
	{
		ret = GOC_ERR_OK; // Nie wypuszczaj sygna³u wy¿ej
	}
	else
	{
		ret = GOC_ERR_UNKNOWNMSG;
	}
	GOC_DEBUG("<- goc_formListener");
	return ret;
}
