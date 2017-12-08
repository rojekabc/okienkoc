#ifndef _GOC_ITERM_H_
#define _GOC_ITERM_H_

#define GOC_EVENTTYPE unsigned int
#define GOC_EVENTDATA unsigned int

// event types
// 	
// 	Event: Press Key (Just Key Press)
// 	Data: unsigned int keyCode
#define GOC_ITERMEVENT_PRESSKEY		1
//	Event: Release Key (Just Key Release)
//	Data: unsigned int keyCode
#define GOC_ITERMEVENT_RELEASEKEY		2
//	Event: New state of keyboard
//	Data: unsigned int keyboard state flags
#define GOC_ITERMEVENT_KBDSTATE		4

// obsluga klawiatury - flagi stanu
#define GOC_KBD_SHIFTL	0x001
#define GOC_KBD_SHIFTR	0x002
#define GOC_KBD_ALTL	0x004
#define GOC_KBD_ALTR	0x008
#define GOC_KBDCTRLL	0x010
#define GOC_KBD_CRLR	0x020
#define GOC_KBD_WINL	0x040
#define GOC_KBD_WINR	0x080
#define GOC_KBD_CAPSL	0x100

#define GOC_KBD_ANYSHIFT	(GOC_KBD_SHIFTL | GOC_KBD_SHIFTR)
#define GOC_KBD_ANYCTRL	(GOC_KBDCTRLL | GOC_KBD_CRLR)
#define GOC_KBD_ABYALT	(GOC_KBD_ALTL | GOC_KBD_ALTR)


// init
// 	Description:
// 		Init interface.
// 		Set by imlementing module.
// 	Arguments:
// 	Return:
// 		 O OK
// 		-1 GOC_ERROR
//
// close
// 	Description:
// 		Finish working of module.
// 		Free resources.
// 	Arguments:
// 	Return:
// 		 0 OK
//		-1 GOC_ERROR
//
// event
// 	Description:
// 		Set event catched by module.
// 		Set by listener.
// 	Arguments:
// 		GOC_EVENTTYPE - type of event
// 		GOC_EVENTDATA - sending data of event
// 	Return:
typedef struct GOC_Terminal {
	void *handler;
	int (*init)(struct GOC_Terminal*);
	void (*event)(GOC_EVENTTYPE, GOC_EVENTDATA);
	int (*close)(struct GOC_Terminal*);
} GOC_Terminal;

#endif // ifndef _ITREM_H_
