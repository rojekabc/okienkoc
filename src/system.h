#ifndef _GOC_SYSTEM_H_
#define _GOC_SYSTEM_H_
#include "global.h"
#include "fobclass.h"
#include "iterm.h"

extern const char* GOC_ELEMENT_SYSTEM;

extern const char* GOC_MSG_NULL;
extern const char* GOC_MSG_FOCUS;
extern const char* GOC_MSG_FOCUSFREE;
/*
 * Otrzymano znak. W nBuf code znaku
 */
extern const char* GOC_MSG_CHAR;
extern const char* GOC_MSG_REMOVE;
/*
 * Rysuj element. W nBuf 0, w pBuf 0
 */
extern const char* GOC_MSG_PAINT;
/*
 * Zatrzymanie systemu. W nBuf code powrotu
 */
extern const char* GOC_MSG_FINISH;
/*
 * Tworzenie elementu. W pBuf adres na strukture ELEMENT z ustawionymi warto¶ciami
 * W nBuf adres na GOC_HANDLER, jaki ma zostaæ wpisany po utworzeniu elementu
 */
extern const char* GOC_MSG_CREATE;
/*
 * Niszczenie elementu, polegaj±ce na zwolnieniu zajmowanej przez niego
 * pamiêci. pBuf i nBuf równe 0
 */
extern const char* GOC_MSG_DESTROY;
extern const char* GOC_MSGFOCUSNEXT;
extern const char* GOC_MSG_FOCUSPREV;
/*
 * Wykonano akcje na danym elemencie (domyslna). W nBuf uchwyt do elementu,
 * ktory ja zainicjowal
 */
extern const char* GOC_MSG_ACTION;
/*
 * Otrzymano znak. W nBuf code znaku
 */
extern const char* GOC_MSG_HOTKEY;
/*
 * System zostal zainicjowany i jest gotowy do pracy
 */
extern const char* GOC_MSG_SYSTEMINITIATED;
/*
 * Przypisanie do obiektu wartosci poczatkowych, zainicjowanie.
 * Wywolywane najczesciej tuz po stworzeniu obiektu (zaallokowaniu pamieci).
 * Obiekt przekazywany jest w pBuf
 */
extern const char* GOC_MSG_INIT;
/*
 * Sygnal od timera.
 * W pBuf identyfikator timera
 */
extern const char* GOC_MSG_TIMERTICK;
/*
 * w nBuf code znaku, ktory zostal zwolniony
 */
extern const char* GOC_MSG_CHARFREE;
/*
 * W nBuf aktualny stan flag klawiatury
 */
extern const char* GOC_MSG_KBDSTATECHANGE;
/*
 * W nBuf code przechwyconego sygna³u systemowego
 */
extern const char* GOC_MSG_CATCHSIGNAL;

int goc_systemRegisterMsgGenerator(
	const char *libname, void (*event)(GOC_EVENTTYPE, GOC_EVENTDATA));
int goc_systemRegisterElement(GOC_IDENTIFIER id, GOC_FUN_LISTENER* func);
int goc_systemCheckMsg(GOC_MSG *wiesc);
int goc_systemSendMsg(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, unsigned int nBuf);
GOC_HANDLER goc_elementCreate(
		GOC_IDENTIFIER id,
		GOC_POSITION x, GOC_POSITION y, GOC_POSITION d, GOC_POSITION w,
		GOC_FLAGS f,	GOC_COLOR k, GOC_HANDLER ojciec);
int goc_systemFindChildNext(GOC_HANDLER u, GOC_HANDLER *d, GOC_BOOL czyStart);
int goc_systemFindChildPrev(GOC_HANDLER u, GOC_HANDLER *c, GOC_BOOL czyStart);
int goc_systemDefaultAction(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, unsigned int nBuf);
void goc_systemSetListenerFunc(GOC_FUN_LISTENER *pFun);
int goc_systemFocusNext(GOC_HANDLER ojciec);
int goc_systemFocusPrev(GOC_HANDLER ojciec);
int goc_systemFocusOn(GOC_HANDLER uchwyt);
int goc_systemDestroyElement(GOC_HANDLER uchwyt);
int goc_systemClearArea(GOC_HANDLER uchwyt);
int goc_systemClearGroupArea(GOC_HANDLER uchwyt);
int goc_systemRepaintUnder(GOC_HANDLER uchwyt);
int goc_systemFocusNextEnd(GOC_HANDLER ojciec);
int goc_systemSetTimer(GOC_HANDLER uchwyt, const char *id);
int goc_systemFreeTimer(GOC_HANDLER uchwyt);
int goc_systemFreeTimers();
GOC_HANDLER goc_systemGetFocusedElement();
int goc_systemIsChildExists(GOC_HANDLER u);

#endif // ifndef _GOC_SYSTEM_H_
