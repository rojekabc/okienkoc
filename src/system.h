#ifndef _GOC_SYSTEM_H_
#define _GOC_SYSTEM_H_
#include "global.h"
#include "iterm.h"
#include "element.h"

extern const char* GOC_ELEMENT_SYSTEM;

extern const char* GOC_MSG_NULL_ID;
/**
 * Give focus to element.
 */
#define GOC_MSG_FOCUS(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_FOCUS_ID)
extern const char* GOC_MSG_FOCUS_ID;
/**
 * Free focus from element.
 */
#define GOC_MSG_FOCUSFREE(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_FOCUSFREE_ID)
extern const char* GOC_MSG_FOCUSFREE_ID;
/*
 * Character was pressed/
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	unsigned int charcode;
} GOC_StMsgChar;
#define GOC_MSG_CHAR(variable, _charcode_) GOC_MSG(GOC_StMsgChar, variable, GOC_MSG_CHAR_ID); variable##Full.charcode = _charcode_
extern const char* GOC_MSG_CHAR_ID;
extern const char* GOC_MSG_REMOVE_ID;

/**
 * Paint message. Inform element to paint its structure.
 */
#define GOC_MSG_PAINT(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_PAINT_ID)
extern const char* GOC_MSG_PAINT_ID;
/*
 * Send a message about system shutdown. Send an exitcode.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	int exitcode;
} GOC_StMsgFinish;
#define GOC_MSG_FINISH(variable, _exitcode_) GOC_MSG(GOC_StMsgFinish, variable, GOC_MSG_FINISH_ID); variable##Full.exitcode = _exitcode_
extern const char* GOC_MSG_FINISH_ID;
/*
 * Tworzenie elementu. W pBuf adres na strukture ELEMENT z ustawionymi warto¶ciami
 * W nBuf adres na GOC_HANDLER, jaki ma zostaæ wpisany po utworzeniu elementu
 */
/**
 * Create element message.
 * element contains values of created element.
 * created is output element, which will be created.
 *
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	GOC_StElement element;
	GOC_HANDLER created;
} GOC_StMsgCreate;
#define GOC_MSG_CREATE(variable) GOC_MSG(GOC_StMsgCreate, variable, GOC_MSG_CREATE_ID)
extern const char* GOC_MSG_CREATE_ID;
/**
 * Destroy element with free memory.
 */
#define GOC_MSG_DESTROY(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_DESTROY_ID)
extern const char* GOC_MSG_DESTROY_ID;
/**
 * Move focus to next element.
 */
#define GOC_MSG_FOCUSNEXT(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_FOCUSNEXT_ID)
extern const char* GOC_MSG_FOCUSNEXT_ID;
/**
 * Move focus to previous element.
 */
#define GOC_MSG_FOCUSPREV(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_FOCUSPREV_ID)
extern const char* GOC_MSG_FOCUSPREV_ID;
/**
 * Message send on do an action. For example after press button.
 */
#define GOC_MSG_ACTION(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_ACTION_ID)
extern const char* GOC_MSG_ACTION_ID;
/*
 * Character was pressed. Use as a hotkey.
 */
#define GOC_MSG_HOTKEY(variable, _charcode_) GOC_MSG(GOC_StMsgChar, variable, GOC_MSG_HOTKEY_ID); variable##Full.charcode = _charcode_
extern const char* GOC_MSG_HOTKEY_ID;
/*
 * System zostal zainicjowany i jest gotowy do pracy
 */
extern const char* GOC_MSG_SYSTEMINITIATED_ID;
/*
 * Initiate after creation. In most cases used to call directly 'parent' structure
 * by child element, when initialized.
 */
#define GOC_MSG_INIT(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_INIT_ID)
extern const char* GOC_MSG_INIT_ID;

/**
 * Send timer tick message.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	const char* timerid;
} GOC_StMsgTimerTick;
#define GOC_MSG_TIMERTICK(variable, _timerid_) GOC_MSG(GOC_StMsgTimerTick, variable, GOC_MSG_TIMERTICK_ID); variable##Full.timerid = _timerid_
extern const char* GOC_MSG_TIMERTICK_ID;
/*
 * Character was released.
 */
#define GOC_MSG_CHARFREE(variable, _charcode_) GOC_MSG(GOC_StMsgChar, variable, GOC_MSG_CHARFREE_ID); variable##Full.charcode = _charcode_
extern const char* GOC_MSG_CHARFREE_ID;
/*
 * Keyboard state change.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	int kbdstate;
} GOC_StMsgKbdStateChange;
#define GOC_MSG_KBDSTATECHANGE(variable, _kbdstate_) GOC_MSG(GOC_StMsgKbdStateChange, variable, GOC_MSG_KBDSTATECHANGE_ID); variable##Full.kbdstate = _kbdstate_
extern const char* GOC_MSG_KBDSTATECHANGE_ID;
/*
 * Catch system signal.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	int signalid;
} GOC_StMsgCatchSignal;
#define GOC_MSG_CATCHSIGNAL(variable, _signalid_) GOC_MSG(GOC_StMsgCatchSignal, variable, GOC_MSG_CATCHSIGNAL_ID); variable##Full.signalid = _signalid_
extern const char* GOC_MSG_CATCHSIGNAL_ID;

int goc_systemRegisterMsgGenerator(
	const char *libname, void (*event)(GOC_EVENTTYPE, GOC_EVENTDATA));
int goc_systemRegisterElement(GOC_IDENTIFIER id, GOC_FUN_LISTENER* func);
int goc_systemCheckMsg(GOC_StMessage* msg);
int goc_systemSendMsg(GOC_HANDLER uchwyt, GOC_StMessage* msg);
GOC_HANDLER goc_elementCreate(
		GOC_IDENTIFIER id,
		GOC_POSITION x, GOC_POSITION y, GOC_POSITION d, GOC_POSITION w,
		GOC_FLAGS f,	GOC_COLOR k, GOC_HANDLER ojciec);
int goc_systemFindChildNext(GOC_HANDLER u, GOC_HANDLER *d, GOC_BOOL czyStart);
int goc_systemFindChildPrev(GOC_HANDLER u, GOC_HANDLER *c, GOC_BOOL czyStart);
int goc_systemDefaultAction(GOC_HANDLER uchwyt, GOC_StMessage* msg);
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
