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
#include <tools/mystr.h>
#include <tools/typetable.h>
#include <tools/screen.h>
#include <tools/plik.h>
#include <tools/term.h>

#include "global-inc.h"

#define GOC_SFLAG_WORK 1
#define GOC_SFLAG_INIT 2

const char* GOC_ELEMENT_SYSTEM = "GOC_System";
const char* GOC_MSG_NULL_ID = "GOC_MsgNull";
const char* GOC_MSG_FOCUS_ID = "GOC_MsgFocus";
const char* GOC_MSG_FOCUSFREE_ID = "GOC_MsgFocusFree";
const char* GOC_MSG_CHAR_ID = "GOC_MsgChar";
const char* GOC_MSG_REMOVE_ID = "GOC_MsgRemove";
const char* GOC_MSG_PAINT_ID = "GOC_MsgPaint";
const char* GOC_MSG_FINISH_ID = "GOC_MsgFinish";
const char* GOC_MSG_CREATE_ID = "GOC_MsgCreate";
const char* GOC_MSG_DESTROY_ID = "GOC_MsgDestroy";
const char* GOC_MSG_FOCUSNEXT_ID = "GOC_MsgFocusNext";
const char* GOC_MSG_FOCUSPREV_ID = "GOC_MsgForcusPrev";
const char* GOC_MSG_ACTION_ID = "GOC_MsgAction";
const char* GOC_MSG_HOTKEY_ID = "GOC_MsgHotKey";
const char* GOC_MSG_SYSTEMINITIATED_ID = "GOC_MsgSystemInitiated";
const char* GOC_MSG_INIT_ID = "GOC_MsgInit";
const char* GOC_MSG_CHARFREE_ID = "GOC_MsgCharFree";
const char* GOC_MSG_KBDSTATECHANGE_ID = "GOC_MsgKeyboardStateChange";
const char* GOC_MSG_CATCHSIGNAL_ID = "GOC_MsgCatchSignal";

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
static int systemListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);

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
	GOC_MSG_PAINT(msg);
	goc_systemSendMsg(element->ojciec, msg);
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
			GOC_MSG_DESTROY(msg);
			goc_systemSendMsg(uchwyt, msg);
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
			GOC_MSG_FOCUSFREE(msg);
			retCode = goc_systemSendMsg( _focusElement, msg);
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
		GOC_MSG_FOCUS(msg);
		start = GOC_FALSE;
		if ( goc_systemSendMsg(c, msg) == GOC_ERR_OK )
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
			GOC_MSG_FOCUS(msg);
			start = GOC_FALSE;
			if ( goc_systemSendMsg(c, msg) == GOC_ERR_OK )
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
			GOC_MSG_FOCUSFREE(msg);
			retCode = goc_systemSendMsg( _focusElement, msg );
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
		GOC_MSG_FOCUS(msg);
		if ( goc_systemSendMsg(c, msg) == GOC_ERR_OK )
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
			GOC_MSG_FOCUS(msg);
			if ( goc_systemSendMsg(c, msg) == GOC_ERR_OK )
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
// TODO TODO TODO (adopt to changes)
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

static const char *interpretMsgFull(GOC_HANDLER uchwyt, GOC_StMessage *msg)
{
	char *result = NULL;
	result = goc_stringAdd(result, "GOC_HANDLER=");
	result = goc_stringAdd(result, uchwyt->id);
	result = goc_stringAdd(result, "(");
	result = goc_stringAddInt(result, uchwyt);
	result = goc_stringAdd(result, ") GOC_MSG=");
	result = goc_stringAdd(result, msg->id);
	switch (msg->id)
	{
		case GOC_MSG_NULL_ID:
			break;
		case GOC_MSG_FOCUS_ID:
			break;
		case GOC_MSG_FOCUSFREE_ID:
			break;
		case GOC_MSG_CHAR_ID:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_REMOVE_ID:
			break;
		case GOC_MSG_CREATE_ID:
			result = goc_stringAdd(result, " PBUF=GOC_StElement{TODO}");
			break;
		case GOC_MSG_FINISH_ID:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_DESTROY_ID:
			break;
		case GOC_MSG_PAINT_ID:
			break;
		case GOC_MSG_FOCUSNEXT_ID:
			break;
		case GOC_MSG_FOCUSPREV_ID:
			break;
		case GOC_MSG_ACTION_ID:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_INIT_ID:
			break;
		case GOC_MSG_HOTKEY_ID:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_SYSTEMINITIATED_ID:
			break;
		case GOC_MSG_LISTCHANGE_ID:
			result = goc_stringAdd(result, " PBUF=");
			result = goc_stringAdd(result, pBuf);
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_LISTSETCOLOR_ID:
			break;
		case GOC_MSG_LISTADDTEXT_ID:
			break;
		case GOC_MSG_LISTCLEAR_ID:
			break;
		case GOC_MSG_LISTSETEXTERNAL_ID:
			break;
		case GOC_MSG_FILELISTISFOLDER_ID:
			break;
		case GOC_MSG_FILELISTADDFILE_ID:
			break;
		case GOC_MSG_FILELISTLISTFOLDER_ID:
			break;
		case GOC_MSG_FILELISTSETFOLDER_ID:
			break;
		case GOC_MSG_LISTADDROW_ID:
			break;
		case GOC_MSG_TIMERTICK_ID:
			break;
		case GOC_MSG_FILELISTISUPFOLDER_ID:
			break;
		case GOC_MSG_FILELISTGETFOLDERNAME_ID:
			break;
		case GOC_MSG_MAPSETSIZE_ID:
			break;
		case GOC_MSG_MAPGETCHARAT_ID:
			break;
		case GOC_MSG_MAPADDCHAR_ID:
			break;
		case GOC_MSG_MAPSETPOINT_ID:
			break;
		case GOC_MSG_MAPPAINT_ID:
			break;
		case GOC_MSG_CHARFREE_ID:
			result = goc_stringAdd(result, " NBUF=");
			result = goc_stringAddInt(result, nBuf);
			break;
		case GOC_MSG_KBDSTATECHANGE_ID:
			break;
		case GOC_MSG_CATCHSIGNAL_ID:
			break;
		case GOC_MSG_MAPGETPOINT_ID:
			break;
		case GOC_MSG_MAPGETCHAR_ID:
			break;
		case GOC_MSG_POSMAPFREEPOINT_ID:
			break;
		default:
			break;
	}
}
#endif // ifdef GOC_PRINTDEBUG

// Wykonywanie wie¶ci dopóty code powrotu nie bêdzie inny ni¿ GOC_ERR_UNKNOWNMSG
int goc_systemBroadcastMsg(GOC_StMessage* msg)
{
	GOC_FUN_LISTENER *pFun = systemListenerFunc(_focusElement);
	int ret = GOC_ERR_UNKNOWNMSG;
	//GOC_HANDLER uchwyt = GOC_HANDLER_SYSTEM;
	GOC_HANDLER uchwyt = _focusElement;
	GOC_BDEBUG( "-> goc_systemBroadcastMsg %s", interpretMsgFull(uchwyt, msg) );
	if ( pFun != NULL )
	{
		ret = pFun(_focusElement, msg);
	}
	uchwyt = _focusElement;
	while ( ret == GOC_ERR_UNKNOWNMSG )
	{
		ret = goc_systemDefaultAction(uchwyt, msg);
		if ( uchwyt == GOC_HANDLER_SYSTEM )
			break;
		uchwyt = ((GOC_StElement*)uchwyt)->ojciec;
	}
	GOC_BDEBUG( "<- goc_systemBroadcastMsg RET=%s(%d)", interpretReturn(ret), ret );
	return ret;
}

int goc_systemDefaultAction(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	int ret = GOC_ERR_OK;
	GOC_FUN_LISTENER *defFun = NULL;
	GOC_BDEBUG( "-> goc_systemDefaultAction %s", interpretMsgFull(uchwyt, msg) );
	if ( msg->id == GOC_MSG_CHAR_ID )
	{
		GOC_StMsgChar* msgchar = (GOC_StMsgChar*)msg;
		GOC_HOTKEY last = NULL;
		while ( (last = goc_hkListKey(uchwyt, last, msgchar->charcode)) )
		{
			GOC_DEBUG("Znaleziono hotkey");
			if ( !(goc_hkGetFlag(last) & GOC_EFLAGA_ENABLE) )
				continue;
			GOC_DEBUG("Hotkey jest aktywny");
			defFun = goc_hkGetFunc(last);
			GOC_BDEBUG("Funkcja dla hotkey: %x", defFun);
			GOC_MSG_HOTKEY(msghotkey, msgchar->charcode);
			ret = defFun(uchwyt, msghotkey);
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
		ret = systemListener(uchwyt, msg);
		GOC_BDEBUG("<- goc_systemDefaultAction RET=%s(%d)", interpretReturn(ret), ret);
		return ret;
	}

	defFun = goc_systemFindDefaultFunc( ((GOC_StElement*)uchwyt)->id );
	if ( defFun )
	{
		GOC_DEBUG("Wykonanie domyslnej funkcji");
		ret = defFun(uchwyt, msg);
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
		ret = goc_systemSendMsg(((GOC_StElement*)uchwyt)->ojciec, msg);
	}
	GOC_BDEBUG("<- goc_systemDefaultAction RET=%s(%d)", interpretReturn(ret), ret);
	return ret;
}

static int systemHotKeyESC(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_MSG_FINISH(msgfinish, GOC_ERR_OK);
	return goc_systemBroadcastMsg( msgfinish );
}

static int systemHotKeyTab(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( keyboardFlags & GOC_KBD_ANYSHIFT ) {
		GOC_MSG_FOCUSPREV( msgfocus );
		return goc_systemBroadcastMsg( msgfocus );
	} else {
		GOC_MSG_FOCUSNEXT( msgfocus );
		return goc_systemBroadcastMsg( msgfocus );
	}
}

static int systemHotKeySwitchTerm(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( keyboardFlags & GOC_KBD_ALTL ) {
		GOC_StMsgChar* msgchar = (GOC_StMsgChar*)msg;
		goc_termSwitchTo(msgchar->charcode-0xFF);
	}
	return GOC_ERR_OK;
}

static int systemListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_FINISH_ID ) {
		return systemClose(GOC_FALSE);
	} else if ( msg->id == GOC_MSG_CREATE_ID ) {
		GOC_FUN_LISTENER *defFun = NULL;
		GOC_StMsgCreate* msgcreate = (GOC_StMsgCreate*)msg;
		defFun = goc_systemFindDefaultFunc( msgcreate->element.id );
		if ( defFun ) {
			return defFun(GOC_HANDLER_SYSTEM, msg );
		} else {
			return GOC_ERR_UNKNOWNID;
		}
	} else if ( msg->id == GOC_MSG_FOCUSNEXT_ID ) {
		return goc_systemFocusNext(GOC_HANDLER_SYSTEM);
	} else if ( msg->id == GOC_MSG_FOCUSPREV_ID ) {
		return goc_systemFocusPrev(GOC_HANDLER_SYSTEM);
	} else if ( msg->id == GOC_MSG_PAINT_ID ) {
		GOC_HANDLER c;
		GOC_BOOL start = GOC_TRUE;
		while ( goc_systemFindChildNext(GOC_HANDLER_SYSTEM, &c, start ) == GOC_ERR_OK )	{
			goc_systemSendMsg(c, msg);
			start = GOC_FALSE;
		}
		fflush(stdout);
		return GOC_ERR_OK;
	} else if ( msg->id == GOC_MSG_CATCHSIGNAL_ID ) {
		// fprintf(stderr, "Zamykanie systemu\n");
		GOC_StMsgCatchSignal* msgsignal = (GOC_StMsgCatchSignal*)msg;
		if (msgsignal->signalid == SIGWINCH) {
			goc_clearscreen();
			GOC_MSG_PAINT( msgpaint );
			return goc_systemSendMsg(GOC_HANDLER_SYSTEM, msgpaint);
		}
		return systemClose(GOC_FALSE);
	}
	return GOC_ERR_UNKNOWNMSG;
}

int goc_systemSendMsg(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_FUN_LISTENER *pFun = NULL;
	int ret;

	GOC_BDEBUG( "-> goc_systemSendMsg %s", interpretMsgFull(uchwyt, msg) );

	pFun = systemListenerFunc(uchwyt);
	if ( pFun != NULL ) {
		ret = pFun(uchwyt, msg);
	} else {
		ret = goc_systemDefaultAction(uchwyt, msg);
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
int goc_systemCheckMsg(GOC_StMessage* msg)
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
	if ( type == GOC_ITERMEVENT_RELEASEKEY ) {
		GOC_MSG_CHARFREE( msg, data );
		goc_systemBroadcastMsg( msg );
	} else if ( type == GOC_ITERMEVENT_PRESSKEY ) {
		GOC_MSG_CHAR( msg, data );
		goc_systemBroadcastMsg( msg );
	} else if ( type == GOC_ITERMEVENT_KBDSTATE ) {
		keyboardFlags = data;
		GOC_MSG_KBDSTATECHANGE( msg, data );
		goc_systemBroadcastMsg( msg );
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
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, msgpaint);
	fflush(stdout);
	pthread_mutex_init( &mutex, NULL );
	pthread_mutex_lock( &mutex );
}

GOC_HANDLER goc_elementCreate(
		GOC_IDENTIFIER id,
		GOC_POSITION x, GOC_POSITION y, GOC_POSITION d, GOC_POSITION w,
		GOC_FLAGS f,	GOC_COLOR k, GOC_HANDLER ojciec)
{
	GOC_MSG_CREATE( msg );
	GOC_DEBUG("-> goc_elementCreate");
	systemInit();
	msgFull.element.id = id;
	msgFull.element.x = x;
	msgFull.element.y = y;
	msgFull.element.width = d;
	msgFull.element.height = w;
	msgFull.element.flag = f;
	msgFull.element.color = k;
	msgFull.element.ojciec = ojciec;
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, msg);
	if ( msgFull.created )
	{
		pElements = goc_handlerAdd( pElements, msgFull.created );
		GOC_BDEBUG("Wykreowano element %s(%d)", uchwyt->id, msgFull.created );
		GOC_DEBUG("<- goc_elementCreate");
	}
	else
	{
		GOC_DEBUG("Element nie zostal wykreowany");
		GOC_DEBUG("<- goc_elementCreate");
	}
	return msgFull.created;
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
	if ( _focusElement != 0 ) {
		GOC_MSG_FOCUSFREE( msg );
		retCode = goc_systemSendMsg( _focusElement, msg );
		if ( retCode != GOC_ERR_OK )
			return retCode;
	}
	_focusElement = uchwyt;
	GOC_MSG_FOCUS( msg );
	retCode = goc_systemSendMsg( uchwyt, msg );
	if ( retCode != GOC_ERR_OK )
		return retCode;
	return GOC_ERR_OK;
}

GOC_HANDLER goc_systemGetFocusedElement()
{
	return _focusElement;
}
