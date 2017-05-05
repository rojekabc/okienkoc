#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <dlfcn.h>
#include <signal.h>

#include "okienkoc.h"
#include "signals.h"
#include "conflog.h"
#include "iterm.h"
#include "mystr.h"
#include "typetable.h"

#include "global-inc.h"

#define GOC_SFLAG_WORK 1
#define GOC_SFLAG_INIT 2

const char* GOC_ELEMENT_SYSTEM = "GOC_System";
const char* GOC_MSG_NULL = "GOC_MsgNull";
const char* GOC_MSG_FOCUS = "GOC_MsgFocus";
const char* GOC_MSG_FOCUSFREE = "GOC_MsgFocusFree";
const char* GOC_MSG_CHAR = "GOC_MsgChar";
const char* GOC_MSG_REMOVE = "GOC_MsgRemove";
const char* GOC_MSG_PAINT = "GOC_MsgPaint";
const char* GOC_MSG_FINISH = "GOC_MsgFinish";
const char* GOC_MSG_CREATE = "GOC_MsgCreate";
const char* GOC_MSG_DESTROY = "GOC_MsgDestroy";
const char* GOC_MSGFOCUSNEXT = "GOC_MsgFocusNext";
const char* GOC_MSG_FOCUSPREV = "GOC_MsgForcusPrev";
const char* GOC_MSG_ACTION = "GOC_MsgAction";
const char* GOC_MSG_HOTKEY = "GOC_MsgHotKey";
const char* GOC_MSG_SYSTEMINITIATED = "GOC_MsgSystemInitiated";
const char* GOC_MSG_INIT = "GOC_MsgInit";
const char* GOC_MSG_CHARFREE = "GOC_MsgCharFree";
const char* GOC_MSG_KBDSTATECHANGE = "GOC_MsgKeyboardStateChange";
const char* GOC_MSG_CATCHSIGNAL = "GOC_MsgCatchSignal";

typedef struct
{
	GOC_FUN_LISTENER* func; // domyslna func nasluchu struktury
	GOC_IDENTIFIER id; // identyfikator struktury
} GOC_StDefListen;

// Zarejestrowane modu³y generuj±ce zdarzenia

static GOC_StITerm* freeTerm(GOC_StITerm* term)
{
	dlclose(term->handler);
	free(term);
	return NULL;
}

static GOC_StITerm* copyTerm(GOC_StITerm* term)
{
	return NULL;
}

GOC_TypeTable(GOC_TermTable, goc_termtable, GOC_StITerm*, freeTerm, copyTerm);
GOC_TermTable* pSystemGenerators;
// GOC_StITerm pTerminal;

static GOC_StDefListen* freeDefListen(GOC_StDefListen* listen)
{
	free(listen);
	return NULL;
}

static GOC_StDefListen* copyDefListen(GOC_StDefListen* listen)
{
	return NULL;
}

GOC_TypeTable(GOC_ListenTable, goc_listen, GOC_StDefListen*, freeDefListen, copyDefListen);
GOC_ListenTable *pDefListens;

static GOC_HANDLER handlerFree( GOC_HANDLER handler ) {return GOC_HANDLER_SYSTEM;}
static GOC_HANDLER handlerCopy( GOC_HANDLER handler ) {return handler;}
GOC_TypeTable(GOC_HandlerTable, goc_handler, GOC_HANDLER, handlerFree, handlerCopy);
GOC_HandlerTable *pElements;

GOC_HANDLER _focusElement;
GOC_FUN_LISTENER *_pFuncListen;
int _flag;
pthread_mutex_t mutex;

static unsigned int keyboardFlags = 0;

static void systemInit();
static void systemRunWork();
static int systemListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf);

static GOC_FUN_LISTENER* systemListenerFunc(GOC_HANDLER uchwyt)
{
	GOC_StElement *element = (GOC_StElement*)uchwyt;
	while ( element )
	{
		void *tmp = goc_elementGetFunc(
			(GOC_HANDLER)element, GOC_FUNID_LISTENER);
		if ( tmp )
			return tmp;
		element = (GOC_StElement*)(element->ojciec);
	}
	return _pFuncListen;
}

static GOC_FUN_LISTENER* goc_systemFindDefaultFunc(GOC_IDENTIFIER id)
{
	int i;
	for ( i=0; i<pDefListens->nElement; i++ )
		if (pDefListens->pElement[i]->id == id)
			return pDefListens->pElement[i]->func;
	return NULL;
}

static int systemClose(GOC_BOOL isError)
{
	GOC_DEBUG("-> systemClose");
	// set flags meaning system isn't working now
	_flag &= ~(GOC_SFLAG_WORK | GOC_SFLAG_INIT);
	// clear system hotkeys
	goc_hkClear(GOC_HANDLER_SYSTEM, GOC_HKFLAG_SYSTEM | GOC_HKFLAG_USER);
	// clear a console - system doesn't do this, when it'll be an error
	if ( isError == GOC_FALSE )
	{
		goc_textallcolor(GOC_WHITE | GOC_BBLACK);
		goc_clearscreen();
		goc_gotoxy(1, 1);
	}
	// System closes all event generators. They should set user settings
	// from moment before system runs.
	{
		int i;
		for (i=0; i < pSystemGenerators->nElement; i++ )
		{
			if ( pSystemGenerators->pElement[i]->close() )
			{
				GOC_ERROR("Cannot close event generator");
			}
		}
	}
	// Remove elements, which still are in system
	while (pElements->nElement)
		goc_systemDestroyElement(pElements->pElement[0]);
	pElements = goc_handlerFree( pElements );
	pDefListens = goc_listenFree( pDefListens );
	goc_systemFreeTimers();
	pthread_mutex_unlock( &mutex );
	GOC_DEBUG("<- systemClose");
	return GOC_ERR_OK;
}

// Funkcja czy¶ci obszar wy³±cznie pod wskazanym obiektem
int goc_systemClearArea(GOC_HANDLER uchwyt)
{
	GOC_DEBUG("-> goc_systemClearArea");

	register GOC_POSITION x, y, w, d;
	x = goc_elementGetScreenX( uchwyt );
	y = goc_elementGetScreenY( uchwyt );
	w = goc_elementGetHeight( uchwyt );
	d = goc_elementGetWidth( uchwyt );

	GOC_BDEBUG("goc_systemClearArea(%d, %d, %d, %d)", x, y, d, w);

	if ( w && d )
	{
		goc_textallcolor(GOC_WHITE | GOC_BBLACK);
		while ( w-- )
		{
			goc_gotoxy(x, y++);
			d = goc_elementGetWidth( uchwyt );
			while ( d-- )
				putchar(' ');
		}
	}

	GOC_DEBUG("<- goc_systemClearArea");
	return GOC_ERR_OK;
}

// Czyszczenie obszaru pod elementem i pod dzieæmi elementu
int goc_systemClearGroupArea(GOC_HANDLER uchwyt)
{
	// Czysc wszystkie dzieci
	GOC_HANDLER c = 0;
	GOC_BOOL start = GOC_TRUE;
	while (goc_systemFindChildNext(uchwyt, &c, start) == GOC_ERR_OK)
	{
		start = GOC_FALSE;
		goc_systemClearGroupArea(c);
	}
	goc_systemClearArea(uchwyt);
	fflush(stdout);
	return GOC_ERR_OK;
}

// Czyszczenie obszaru podanego elementu i elementów zgrupowanych dla tego¿
// elementu, a nastêpnie odrysowanie obszaru, jaki znajduje siê pod tym
// elementem, poprzez odwo³anie do rodzica z nakazem jego rysowania
int goc_systemRepaintUnder(GOC_HANDLER uchwyt)
{
	GOC_StElement *element = (GOC_StElement*)uchwyt;
	// zachowaj flage
	GOC_FLAGS flag = element->flag;
	// zrob element niewidocznym - to po to, ¿eby podczas odrysowywania
	// obszaru przez ojca nie nastêpowa³o rysowanie
	element->flag &= ~GOC_EFLAGA_PAINTED;
	goc_systemClearGroupArea(uchwyt);
	// odrysuj tlo
	goc_systemSendMsg(element->ojciec, GOC_MSG_PAINT, 0, 0);
	element->flag = flag;
	return GOC_ERR_OK;
}

int goc_systemDestroyElement(GOC_HANDLER uchwyt)
{
	int i;
	GOC_HANDLER c;
	if ( uchwyt == _focusElement )
	{
		_focusElement = 0;
	}
	// Usun najpierw dzieci
	while (goc_systemFindChildNext(uchwyt, &c, GOC_TRUE) == GOC_ERR_OK)
	{
		goc_systemDestroyElement(c);
	}
	// A teraz wywal ten element
	for ( i=0; i<pElements->nElement; i++ )
	{
		if ( pElements->pElement[i] == uchwyt )
		{
			goc_systemSendMsg(uchwyt, GOC_MSG_DESTROY, 0, 0);
			pElements = goc_handlerRemove(pElements, i);
			break;
		}
	}
	return GOC_ERR_END;
}

#define GOC_FLAGRESTART 1
static int systemFocusNext(GOC_HANDLER ojciec, GOC_FLAGS f)
{
	GOC_BOOL start = GOC_TRUE;
	GOC_HANDLER c, of; // c - uchwyt dziecka. of - zachowanie uchwytu focused
	int retCode;

	GOC_BDEBUG("-> goc_systemFocusNext dla %d", ojciec);

	of = _focusElement;
	/* zwolnij focus na obecnym elemencie */
	if ( _focusElement != 0 )
	{
		if ( _focusElement != ojciec )
		{
			retCode = goc_systemSendMsg( _focusElement, GOC_MSG_FOCUSFREE, 0, 0 );
			if ( retCode != GOC_ERR_OK )
				return retCode;
//			_focusElement = 0;
			// przejscie do elementu, z ktorego wlasnie zdjeto focus
			while ( goc_systemFindChildNext(ojciec, &c, start) == GOC_ERR_OK )
			{
				start = GOC_FALSE;
				if ( of == c )
					break;
			}
		}
	}
	/* znajdz nastepny element do przejêcia focus'a */
	while ( goc_systemFindChildNext(ojciec, &c, start ) == GOC_ERR_OK )
	{
		start = GOC_FALSE;
		if ( goc_systemSendMsg(c, GOC_MSG_FOCUS, 0, 0) == GOC_ERR_OK )
		{
			if ( _focusElement == of )
			{
				// Czy focus nie zostal zmieniony wewnatrz
				// elementu ? Jesli nie - ustaw
				_focusElement = c;
			}
			return GOC_ERR_OK;
		}
	}
	/* Czy nie przeszukiwac dalej - zakaz nawracania */
	if ( (f & GOC_FLAGRESTART) == 0 )
		return GOC_ERR_REFUSE;
	/* To oznacza, ze nastapilo przeszukanie calego ojca i nie znaleziono
	 * odpowiedniego dziecka, ktore przejeloby focus */
	if ( _focusElement == ojciec )
		return GOC_ERR_OK;
	/* znajdz element od poczatku do aktualnego elementu, ktory moze
	 * przejac focus */
	if ( of != 0 )
	{
		start = GOC_TRUE;
		while ( goc_systemFindChildNext(ojciec, &c, start) == GOC_ERR_OK )
		{
			start = GOC_FALSE;
			if ( goc_systemSendMsg(c, GOC_MSG_FOCUS, 0, 0) == GOC_ERR_OK )
			{
				if ( _focusElement == of )
				{
					// Czy focus nie zostal zmieniony
					// wewnatrz elementu ? Jesli nie - ustaw
					_focusElement = c;
				}
				return GOC_ERR_OK;
			}
			if ( of == c )
				break;
		}
	}
//	return GOC_ERR_OK;
	return GOC_ERR_REFUSE; // nie znaleziono nic
}

/*
 * Wykonuj przekazywanie focus'a z nawracaniem od poczatku
 */
int goc_systemFocusNext(GOC_HANDLER ojciec)
{
	return systemFocusNext(ojciec, GOC_FLAGRESTART);
}

/*
 * Wykonuj przekazywanie focus'a ale bez nawracania od poczatku
 * Zwroci GOC_ERR_REFUSE jezeli nie uda mu sie znalezc
 */
int goc_systemFocusNextEnd(GOC_HANDLER ojciec)
{
	return systemFocusNext(ojciec, 0);
}
#undef GOC_FLAGRESTART


int goc_systemFocusPrev(GOC_HANDLER ojciec)
{
	GOC_BOOL start = GOC_TRUE;
	GOC_HANDLER c, of;
	int retCode;

	GOC_BDEBUG("goc_systemFocusPrev dla %d", ojciec);

	of = _focusElement;
	/* zwolnij focus na obecnym elemencie */
	if ( _focusElement != 0 )
	{
		if ( _focusElement != ojciec )
		{
			retCode = goc_systemSendMsg( _focusElement, GOC_MSG_FOCUSFREE, 0, 0 );
			if ( retCode != GOC_ERR_OK )
				return retCode;
//			_focusElement = 0;
			// przejscie do elementu, z ktorego wlasnie zdjeto focus
			while ( goc_systemFindChildPrev(ojciec, &c, start) == GOC_ERR_OK )
			{
				start = GOC_FALSE;
				if ( of == c )
					break;
			}
		}
	}
	/* znajdz nastepny element do przejêcia focus'a */
	while ( goc_systemFindChildPrev(ojciec, &c, start ) == GOC_ERR_OK )
	{
		start = GOC_FALSE;
		if ( goc_systemSendMsg(c, GOC_MSG_FOCUS, 0, 0) == GOC_ERR_OK )
		{
			if ( _focusElement == of )
				_focusElement = c;
			return GOC_ERR_OK;
		}
	}
	/* To oznacza, ze nastapilo przeszukanie calego ojca i nie znaleziono
	 * odpowiedniego dziecka, ktore przejeloby focus */
	if ( _focusElement == ojciec )
		return GOC_ERR_OK;
	/* znajdz element od poczatku do aktualnego elementu, ktory moze
	 * przejac focus */
	if ( of != 0 )
	{
		start = GOC_TRUE;
		while ( goc_systemFindChildPrev(ojciec, &c, start) == GOC_ERR_OK )
		{
			start = GOC_FALSE;
			if ( goc_systemSendMsg(c, GOC_MSG_FOCUS, 0, 0) == GOC_ERR_OK )
			{
				if ( _focusElement == of )
					_focusElement = c;
				return GOC_ERR_OK;
			}
			if ( of == c )
				break;
		}
	}
	return GOC_ERR_OK;
}

#ifdef GOC_PRINTDEBUG

static const char *errCodesTable[] = 
{
	"GOC_ERR_OK", "GOC_ERR_UNKNOWNMSG", "GOC_ERR_UNKNOWNID",
	"GOC_ERR_CREATE", "GOC_ERR_WRONGARGUMENT", "GOC_ERR_END",
	"GOC_ERR_REFUSE", "GOC_ERR_FALSE", "GOC_ERR_PARENT",
	"GOC_ERR_NEXT"
};

static const char *interpretReturn(int retCode)
{
	return errCodesTable[retCode];
}

static const char *interpretMsgFull(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	char *result = NULL;
	result = goc_stringAdd(result, "GOC_HANDLER=");
	result = goc_stringAdd(result, uchwyt->id);
	result = goc_stringAdd(result, "(");
	result = goc_stringAddInt(result, uchwyt);
	result = goc_stringAdd(result, ") GOC_MSG=");
	result = goc_stringAdd(result, wiesc);
	result = goc_stringAdd(result, "(");
	result = goc_stringAddInt(result, wiesc);
	result = goc_stringAdd(result, ")");
	switch (wiesc)
	{
		case GOC_MSG_NULL:
			break;
		case GOC_MSG_FOCUS:
			break;
		case GOC_MSG_FOCUSFREE:
			break;
		case GOC_MSG_CHAR:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_REMOVE:
			break;
		case GOC_MSG_CREATE:
			result = goc_stringAdd(result, " PBUF=GOC_StElement{TODO}");
			break;
		case GOC_MSG_FINISH:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_DESTROY:
			break;
		case GOC_MSG_PAINT:
			break;
		case GOC_MSGFOCUSNEXT:
			break;
		case GOC_MSG_FOCUSPREV:
			break;
		case GOC_MSG_ACTION:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_INIT:
			break;
		case GOC_MSG_HOTKEY:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_SYSTEMINITIATED:
			break;
		case GOC_MSG_LISTCHANGE:
			result = goc_stringAdd(result, " PBUF=");
			result = goc_stringAdd(result, pBuf);
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_LISTSETCOLOR:
			break;
		case GOC_MSG_LISTADDTEXT:
			break;
		case GOC_MSG_LISTCLEAR:
			break;
		case GOC_MSG_LISTSETEXTERNAL:
			break;
		case GOC_MSG_FILELISTISFOLDER:
			break;
		case GOC_MSG_FILELISTADDFILE:
			break;
		case GOC_MSG_FILELISTLISTFOLDER:
			break;
		case GOC_MSG_FILELISTSETFOLDER:
			break;
		case GOC_MSG_LISTADDROW:
			break;
		case GOC_MSG_TIMERTICK:
			break;
		case GOC_MSG_FILELISTISUPFOLDER:
			break;
		case GOC_MSG_FILELISTGETFOLDERNAME:
			break;
		case GOC_MSG_MAPSETSIZE:
			break;
		case GOC_MSG_MAPGETCHARAT:
			break;
		case GOC_MSG_MAPADDCHAR:
			break;
		case GOC_MSG_MAPSETPOINT:
			break;
		case GOC_MSG_MAPPAINT:
			break;
		case GOC_MSG_CHARFREE:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_KBDSTATECHANGE:
			break;
		case GOC_MSG_CATCHSIGNAL:
			break;
		case GOC_MSG_MAPGETPOINT:
			break;
		case GOC_MSG_MAPGETCHAR:
			break;
		case GOC_MSG_POSMAPFREEPOINT:
			break;
		default:
			break;
	}
}
#endif // ifdef GOC_PRINTDEBUG

// Wykonywanie wie¶ci dopóty code powrotu nie bêdzie inny ni¿ GOC_ERR_UNKNOWNMSG
int goc_systemBroadcastMsg(GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	GOC_FUN_LISTENER *pFun = systemListenerFunc(_focusElement);
	int ret = GOC_ERR_UNKNOWNMSG;
	//GOC_HANDLER uchwyt = GOC_HANDLER_SYSTEM;
	GOC_HANDLER uchwyt = _focusElement;
	GOC_BDEBUG( "-> goc_systemBroadcastMsg %s", interpretMsgFull(uchwyt, wiesc, pBuf, nBuf) );
	if ( pFun != NULL )
	{
		ret = pFun(_focusElement, wiesc, pBuf, nBuf);
	}
	uchwyt = _focusElement;
	while ( ret == GOC_ERR_UNKNOWNMSG )
	{
		ret = goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
		if ( uchwyt == GOC_HANDLER_SYSTEM )
			break;
		uchwyt = ((GOC_StElement*)uchwyt)->ojciec;
	}
	GOC_BDEBUG( "<- goc_systemBroadcastMsg RET=%s(%d)", interpretReturn(ret), ret );
	return ret;
}

int goc_systemDefaultAction(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	int ret = GOC_ERR_OK;
	GOC_FUN_LISTENER *defFun = NULL;
	GOC_BDEBUG( "-> goc_systemDefaultAction %s", interpretMsgFull(uchwyt, wiesc, pBuf, nBuf) );
	if ( wiesc == GOC_MSG_CHAR )
	{
		GOC_HOTKEY last = NULL;
		while ( (last = goc_hkListKey(uchwyt, last, nBuf)) )
		{
			GOC_DEBUG("Znaleziono hotkey");
			if ( !(goc_hkGetFlag(last) & GOC_EFLAGA_ENABLE) )
				continue;
			GOC_DEBUG("Hotkey jest aktywny");
			defFun = goc_hkGetFunc(last);
			GOC_BDEBUG("Funkcja dla hotkey: %x", defFun);
			ret = defFun(uchwyt, GOC_MSG_HOTKEY, pBuf, nBuf);
			if ( ret == GOC_ERR_OK )
			{
				GOC_BDEBUG("<- goc_systemDefaultAction RET=%s(%d)", interpretReturn(ret), ret);
				return ret;
			}
		}
		defFun = NULL;
	}
	if ( uchwyt == GOC_HANDLER_SYSTEM )
	{
		ret = systemListener(uchwyt, wiesc, pBuf, nBuf);
		GOC_BDEBUG("<- goc_systemDefaultAction RET=%s(%d)", interpretReturn(ret), ret);
		return ret;
	}

	defFun = goc_systemFindDefaultFunc( ((GOC_StElement*)uchwyt)->id );
	if ( defFun )
	{
		GOC_DEBUG("Wykonanie domyslnej funkcji");
		ret = defFun(uchwyt, wiesc, pBuf, nBuf);
	}
	else
	{
		/* je¶li nie ma domy¶lnej wywo³aj parent'a */
		GOC_DEBUG("Brak domyslnej funkcji");
		ret = GOC_ERR_UNKNOWNMSG;
	}
	/* Nie obsluzono wiesci, niech zrobi to ojciec */
	if ( ret == GOC_ERR_UNKNOWNMSG )
	{
		GOC_DEBUG("Wiesc nieobsluzona - przekazanie do ojca");
		ret = goc_systemSendMsg(((GOC_StElement*)uchwyt)->ojciec, wiesc, pBuf, nBuf);
	}
	GOC_BDEBUG("<- goc_systemDefaultAction RET=%s(%d)", interpretReturn(ret), ret);
	return ret;
}

static int systemHotKeyESC(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	return goc_systemBroadcastMsg( GOC_MSG_FINISH, 0, GOC_ERR_OK );
}

static int systemHotKeyTab(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	if ( keyboardFlags & GOC_KBD_ANYSHIFT )
		return goc_systemBroadcastMsg(GOC_MSG_FOCUSPREV, 0, 0 );
	else
		return goc_systemBroadcastMsg( GOC_MSGFOCUSNEXT, 0, 0 );
}

static int systemHotKeySwitchTerm(
	GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	if ( keyboardFlags & GOC_KBD_ALTL )
		goc_termSwitchTo(nBuf-0xFF);
	return GOC_ERR_OK;
}

static int systemListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	if ( wiesc == GOC_MSG_FINISH )
	{
		return systemClose(GOC_FALSE);
	}
	else if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_FUN_LISTENER *defFun = NULL;
		defFun = goc_systemFindDefaultFunc( ((GOC_StElement*)pBuf)->id );
		if ( defFun )
		{
			return defFun(GOC_HANDLER_SYSTEM, wiesc, pBuf, nBuf );
		}
		else
		{
			return GOC_ERR_UNKNOWNID;
		}
	}
	else if ( wiesc == GOC_MSGFOCUSNEXT )
	{
		return goc_systemFocusNext(GOC_HANDLER_SYSTEM);
	}
	else if ( wiesc == GOC_MSG_FOCUSPREV )
	{
		return goc_systemFocusPrev(GOC_HANDLER_SYSTEM);
	}
	else if ( wiesc == GOC_MSG_PAINT )
	{
		GOC_HANDLER c;
		GOC_BOOL start = GOC_TRUE;
		while ( goc_systemFindChildNext(GOC_HANDLER_SYSTEM, &c, start ) == GOC_ERR_OK )
		{
			goc_systemSendMsg(c, GOC_MSG_PAINT, 0, 0);
			start = GOC_FALSE;
		}
		fflush(stdout);
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_CATCHSIGNAL )
	{
		// fprintf(stderr, "Zamykanie systemu\n");
		if (nBuf == SIGWINCH)
		{
			goc_clearscreen();
			return goc_systemSendMsg(GOC_HANDLER_SYSTEM, GOC_MSG_PAINT, NULL, 0);
		}
		return systemClose(GOC_FALSE);
	}
	return GOC_ERR_UNKNOWNMSG;
}

int goc_systemSendMsg(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	GOC_FUN_LISTENER *pFun = NULL;
	int ret;

	GOC_BDEBUG( "-> goc_systemSendMsg %s", interpretMsgFull(uchwyt, wiesc, pBuf, nBuf) );

	pFun = systemListenerFunc(uchwyt);
	if ( pFun != NULL )
	{
		ret = pFun(uchwyt, wiesc, pBuf, nBuf);
	}
	else
	{
		ret = goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
	}

	GOC_BDEBUG("<- goc_systemSendMsg RET=%s(%d)", interpretReturn(ret), ret);
	return ret;
}

/*
 * Funkcja:
 * Sprawdza, czy w systemie jest jaka¶ wie¶æ do przetworzenia
 *
 * Argumenty:
 * wiesc - otrzymana wie¶æ
 *
 * Zwraca:
 * Je¿eli system nadal funckjonuje zwracana jest warto¶æ != 0, je¿eli nie,
 * zwracana jest warto¶æ 0
 *
 */
int goc_systemCheckMsg(GOC_MSG *wiesc)
{
	// zainicjowanie systemu
	systemInit();
	systemRunWork();
	// zablokowanie, a¿ do momentu zdjêcia lock (a¿ do zamkniêcia systemu)
	pthread_mutex_lock(&mutex);
	if ( _flag & GOC_SFLAG_WORK )
	{
		return 1;
	}
	else
	{
		// zamykanie systemu
		
		// zwolnienie systemów generowania
		pSystemGenerators = goc_termtableFree( pSystemGenerators );

		// usuniêcie mutex
		pthread_mutex_destroy(&mutex);
		return 0;
	}
}

/*
 * Funkcja:
 * Zarejestrowanie modu³u generuj±cego zdarzenia.
 *
 * Argumenty:
 * libname - nazwa biblioteki, która bêdzie generowa³a zdarzenia
 * event - func nas³uchuj±ca przychodz±ce zdarzenia
 *
 * Zwraca:
 * GOC_ERR_OK je¶li nie by³o problemów
 *
 */
int goc_systemRegisterMsgGenerator(
	const char *libname, void (*event)(GOC_EVENTTYPE, GOC_EVENTDATA))
{
	int (*initModule)(void *) = NULL;
	GOC_StITerm *iterm = malloc( sizeof(GOC_StITerm) );
	memset(iterm, 0, sizeof(GOC_StITerm));
	iterm->event = event;
	iterm->handler = dlopen(libname, RTLD_NOW);
	if ( !iterm->handler )
	{
		GOC_ERROR(dlerror());
		return -1;
	}
	dlerror();
	initModule = dlsym(iterm->handler, "initModule");
	if ( !initModule )
	{
		GOC_ERROR(dlerror());
		return -1;
	}
	if ( (*initModule)( iterm ) )
	{
		GOC_ERROR("Error initializing module");
		return -1;
	}
	if ( !(iterm->init) )
	{
		GOC_ERROR("NULL init function");
		return -1;
	}
	if ( iterm->init() )
	{
		GOC_ERROR("Wrong initializing terminal");
		return -1;
	}
	pSystemGenerators = goc_termtableAdd(pSystemGenerators, iterm);
	return GOC_ERR_OK;
}

int goc_systemRegisterElement(GOC_IDENTIFIER id, GOC_FUN_LISTENER* func)
{
	GOC_StDefListen* defListen = (GOC_StDefListen*)malloc(sizeof(GOC_StDefListen));
	defListen->id = id;
	defListen->func = func;
	pDefListens = goc_listenAdd( pDefListens, defListen );
	return GOC_ERR_OK;
}

void goc_systemSetListenerFunc(GOC_FUN_LISTENER *pFun)
{
	systemInit();
	_pFuncListen = pFun;
}

static void eventKeyboard(GOC_EVENTTYPE type, GOC_EVENTDATA data)
{
	if ( type == GOC_ITERMEVENT_RELEASEKEY )
	{
		goc_systemBroadcastMsg(GOC_MSG_CHARFREE, 0, data);
	}
	else if ( type == GOC_ITERMEVENT_PRESSKEY )
	{
		goc_systemBroadcastMsg(GOC_MSG_CHAR, 0, data);
	}
	else if ( type == GOC_ITERMEVENT_KBDSTATE )
	{
		keyboardFlags = data;
		goc_systemBroadcastMsg(GOC_MSG_KBDSTATECHANGE, 0, data);
	}
}

/*
 * zainicjowanie systemu pod k±tem utworzenia jego struktur
 */
static void systemInit()
{
	if ( !( _flag & GOC_SFLAG_INIT ) )
	{
		_flag |= GOC_SFLAG_INIT;

		pSystemGenerators = goc_termtableAlloc();;
		pDefListens = goc_listenAlloc();
		pElements = goc_handlerAlloc();
		_focusElement = GOC_HANDLER_SYSTEM;
		_pFuncListen = NULL;

		goc_hkAdd(GOC_HANDLER_SYSTEM, 27, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeyESC);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 9, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeyTab);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 0x100, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeySwitchTerm);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 0x101, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeySwitchTerm);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 0x102, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeySwitchTerm);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 0x103, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeySwitchTerm);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 0x104, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeySwitchTerm);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 0x105, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			systemHotKeySwitchTerm);
		
		goc_systemRegisterElement(GOC_ELEMENT_FRAME, &goc_frameListener );
		goc_systemRegisterElement(GOC_ELEMENT_LABEL, &goc_labelListener );
		goc_systemRegisterElement(GOC_ELEMENT_EDIT, &goc_editListener );
		goc_systemRegisterElement(GOC_ELEMENT_BUTTON, &goc_buttonListener );
		goc_systemRegisterElement(GOC_ELEMENT_LIST, &goc_listListener );
		goc_systemRegisterElement(GOC_ELEMENT_CHOICE, &goc_choiceListener );
		goc_systemRegisterElement(GOC_ELEMENT_BAR, &goc_barListener );
		goc_systemRegisterElement(GOC_ELEMENT_PRECENT, &goc_precentListener );
		goc_systemRegisterElement(GOC_ELEMENT_MASK, &goc_maskListener );
		goc_systemRegisterElement(GOC_ELEMENT_FORM, &goc_formListener );
		goc_systemRegisterElement(GOC_ELEMENT_GROUP, &goc_groupListener );
		goc_systemRegisterElement(GOC_ELEMENT_SELLIST, &goc_sellistListener );
		goc_systemRegisterElement(GOC_ELEMENT_FILELIST, &goc_filelistListener );
		goc_systemRegisterElement(GOC_ELEMENT_RAWMAP, &goc_maparawListener );
		goc_systemRegisterElement(GOC_ELEMENT_POSMAP, &goc_mapaposListener );
		goc_systemRegisterElement(GOC_ELEMENT_SLIDEBAR, &goc_slidebarListener );
	}
	return;
}

/*
 * Uruchomienie systemu pod k±tem rozpoczêcia mechanizmu jego aktywno¶ci
 */
static void systemRunWork()
{
	if ( _flag & GOC_SFLAG_WORK )
		return;
	_flag |= GOC_SFLAG_WORK;
	// przechwytywanie sygna³ów - póki terminal jeszcze normalny
	goc_systemCatchSignals();
	// ustawienie terminala jako generatora
	{
		const char *termname = getenv("TERM");
		char *modname = NULL;
		// TODO: Configured path
		modname = goc_stringCopy(modname,
			"/usr/local/share/okienkoc/libterm-");
		modname = goc_stringAdd(modname, termname);
		modname = goc_stringAdd(modname, ".so");
		if ( ! goc_isFileExists(modname) )
		{
			GOC_ERROR("Unsupported terminal type");
			goc_stringFree(modname);
			systemClose(GOC_TRUE);
			return;
		}
		else
		{
			GOC_BDEBUG("Using module [%s] for terminal", modname);
		}
		if ( goc_systemRegisterMsgGenerator( modname, eventKeyboard ) )
		{
			GOC_ERROR("Cannot initialize keyboard");
			goc_stringFree(modname);
			systemClose(GOC_TRUE);
			return;
		}
		goc_stringFree(modname);
	}
	goc_textallcolor(GOC_BLACK | GOC_BBLACK);
	goc_clearscreen();
//	goc_systemSendMsg(GOC_HANDLER_SYSTEM, GOC_MSG_SYSTEMINITIATED, 0, 0);
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, GOC_MSG_PAINT, 0, 0);
	fflush(stdout);
	pthread_mutex_init( &mutex, NULL );
	pthread_mutex_lock( &mutex );
}

GOC_HANDLER goc_elementCreate(
		GOC_IDENTIFIER id,
		GOC_POSITION x, GOC_POSITION y, GOC_POSITION d, GOC_POSITION w,
		GOC_FLAGS f,	GOC_COLOR k, GOC_HANDLER ojciec)
{
	GOC_StElement elem;
	GOC_HANDLER uchwyt = 0;
	GOC_DEBUG("-> goc_elementCreate");
	systemInit();
	memset( &elem, 0, sizeof(GOC_StElement) );
	elem.id = id;
	elem.x = x;
	elem.y = y;
	elem.width = d;
	elem.height = w;
	elem.flag = f;
	elem.color = k;
	elem.ojciec = ojciec;
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, GOC_MSG_CREATE, &elem, (GOC_HANDLER)(&uchwyt));
	if ( uchwyt )
	{
		pElements = goc_handlerAdd( pElements, uchwyt );
		GOC_BDEBUG("Wykreowano element %s(%d)", uchwyt->id, uchwyt );
		GOC_DEBUG("<- goc_elementCreate");
	}
	else
	{
		GOC_DEBUG("Element nie zostal wykreowany");
		GOC_DEBUG("<- goc_elementCreate");
	}
	return uchwyt;
}

int goc_systemFindChildPrev(GOC_HANDLER u, GOC_HANDLER *c, GOC_BOOL czyStart)
{
	static int i;
	if ( !c )
		return GOC_ERR_WRONGARGUMENT;
	if ( czyStart == GOC_TRUE )
		i = pElements->nElement-1;
	else
		i--;
	for (; i>=0; i--)
	{
		if ( ((GOC_StElement*)(pElements->pElement[i]))->ojciec == u )
		{
			*c = pElements->pElement[i];
			return GOC_ERR_OK;
		}
	}
	return GOC_ERR_END;
}

int goc_systemFindChildNext(GOC_HANDLER u, GOC_HANDLER *c, GOC_BOOL czyStart)
{
	int i;
	
	if ( !c )
		return GOC_ERR_WRONGARGUMENT;
	i ^= i;
	if ( czyStart == GOC_FALSE )
	{ // odszukaj ostatnio znaleziony uchwyt
		for ( ; i<pElements->nElement; i++ )
		{
			if ( pElements->pElement[i] == *c )
				break;
		}
		if ( pElements->nElement == i )
			// nie znaleziono ostatnio wyszukanego dziecka
			return GOC_ERR_WRONGARGUMENT;
		i++;
	}

	for ( ; i<pElements->nElement; i++ )
	{
		if ( ((GOC_StElement*)(pElements->pElement[i]))->ojciec == u )
		{
			*c = pElements->pElement[i];
			return GOC_ERR_OK;
		}
	}
	*c = 0;
	return GOC_ERR_END;
}

int goc_systemIsChildExists(GOC_HANDLER u)
{
	int i;
	for (i=0; i<pElements->nElement; i++ )
	{
		if ( pElements->pElement[i] == u )
			return GOC_ERR_OK;
	}
	return GOC_ERR_END;
}

int goc_systemFocusOn(GOC_HANDLER uchwyt)
{
	int retCode;
	if ( _focusElement != 0 )
	{
		retCode = goc_systemSendMsg( _focusElement, GOC_MSG_FOCUSFREE, 0, 0 );
		if ( retCode != GOC_ERR_OK )
			return retCode;
	}
	_focusElement = uchwyt;
	retCode = goc_systemSendMsg( uchwyt, GOC_MSG_FOCUS, 0, 0 );
	if ( retCode != GOC_ERR_OK )
		return retCode;
	return GOC_ERR_OK;
}

GOC_HANDLER goc_systemGetFocusedElement()
{
	return _focusElement;
}
