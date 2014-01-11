#include <pthread.h>
#include <stdio.h>
#include <X11/Xlib.h>
#define XK_XKB_KEYS
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include <linux/input.h>
#include "iterm.h"
#include "term.h"
#include "conflog.h"

struct GOC_StITerm *pITerm = NULL;
pthread_t thread = 0;

static int keyboardFlags = 0;

Display *dpy = NULL;
Window win;

#define GOC_REDEFSIZE 17
int xkey[GOC_REDEFSIZE] =
{
	XK_BackSpace, XK_Tab, XK_Return, XK_Linefeed, XK_Delete, XK_Escape,
	XK_Home, XK_Left, XK_Up, XK_Right, XK_Down, XK_Prior, XK_Page_Up, XK_Next, XK_Page_Down, XK_End, XK_Begin
};
int rkey[GOC_REDEFSIZE] =
{
	0x7f,         0x09,   0x0d,      0x0a,        0x116,      0x1b,
	0x114,   0x601,   0x603, 0x602,   0x600,    0x118,    0x118,      0x119,   0x119,        0x117,  0x114
};

static int buildKeyEntryCode(XKeyEvent *kev)
{
	int x;
	int i;
    	XLookupString(kev, NULL, 0, &x, NULL);
	// x = XLookupKeysym(kev, index);
//	GOC_FDEBUG(goc_stringAddInt(goc_stringCopy(NULL, "KeySym is "), x));
	for (i=0; i<GOC_REDEFSIZE; i++)
	{
		if ( x == xkey[i] )
		{
			x = rkey[i];
			break;
		}
	}
//	GOC_FDEBUG(goc_stringAddInt(goc_stringCopy(NULL, "Remapped KeySym is "), x));
	return x;
}

static void *xTermEventGenerator(void *arg)
{
	XEvent event;
	// Do until thread alive
	while (thread)
	{
		XNextEvent(dpy, &event);
		switch (event.type)
		{
			case KeyPress:
				{
				XKeyEvent *kev = (XKeyEvent*)&event;
				int x;
			    	XLookupString(kev, NULL, 0, &x, NULL);
				GOC_FDEBUG(goc_stringAddHex(goc_stringCopy(NULL, "Press KeySym is "), x));
				switch ( x )
				{
					case XK_Shift_L:
						keyboardFlags |= GOC_KBD_SHIFTL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Shift_R:
						keyboardFlags |= GOC_KBD_SHIFTR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Alt_L:
						keyboardFlags |= GOC_KBD_ALTL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Alt_R:
					case XK_ISO_Level3_Shift:
						keyboardFlags |= GOC_KBD_ALTR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Control_L:
						keyboardFlags |= GOC_KBDCTRLL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Control_R:
						keyboardFlags |= GOC_KBD_CRLR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Caps_Lock:
						if ( keyboardFlags & GOC_KBD_CAPSL )
							keyboardFlags &= ~GOC_KBD_CAPSL;
						else
							keyboardFlags |= GOC_KBD_CAPSL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Super_L:
						keyboardFlags |= GOC_KBD_WINL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Super_R:
						keyboardFlags |= GOC_KBD_WINR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					default:
					{
						pITerm->event(GOC_ITERMEVENT_PRESSKEY, buildKeyEntryCode(kev));
					}
				}
				break;
				}
			case KeyRelease:
				{
				XKeyEvent *kev = (XKeyEvent*)&event;
				int x;
			    	XLookupString(kev, NULL, 0, &x, NULL);
				GOC_FDEBUG(goc_stringAddHex(goc_stringCopy(NULL, "Release KeySym is "), x));
				switch ( x )
				{
					case XK_Shift_L:
						keyboardFlags &= ~GOC_KBD_SHIFTL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Shift_R:
						keyboardFlags &= ~GOC_KBD_SHIFTR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Alt_L:
						keyboardFlags &= ~GOC_KBD_ALTL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Alt_R:
					case XK_ISO_Level3_Shift:
						keyboardFlags &= ~GOC_KBD_ALTR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Control_L:
						keyboardFlags &= ~GOC_KBDCTRLL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Control_R:
						keyboardFlags &= ~GOC_KBD_CRLR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Super_L:
						keyboardFlags &= ~GOC_KBD_WINL;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					case XK_Super_R:
						keyboardFlags &= ~GOC_KBD_WINR;
						pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
						break;
					default:
						pITerm->event(GOC_ITERMEVENT_RELEASEKEY, buildKeyEntryCode(kev));
				}
				break;
				}
			default:
				GOC_ERROR("Unknown event");
				break;
		}
	}
	return 0;
}

static int xTermInit()
{
	int bar;
	XEvent event;

	// open display connection
	dpy = XOpenDisplay( NULL );
	if ( !dpy )
	{
		GOC_ERROR("Cannot open X display");
		return -1;
	}
	// Get actual window focused as kyboard input
	// Should be terminal window :/
	XGetInputFocus(dpy, &win, &bar);
	// capture keyboard
	XSelectInput(dpy, win, KeyPressMask | KeyReleaseMask );
	// capture keyboard in window childrens
	{
		Window rootwin = 0;
		Window parentwin = 0;
		Window *pChildrenswin = NULL;
		int nChildrenswin = 0;
		if ( XQueryTree(dpy, win, &rootwin, &parentwin, &pChildrenswin, &nChildrenswin) )
		{
			int i = 0;
			for ( i=0; i<nChildrenswin; i++ )
			{
				XSelectInput(dpy, pChildrenswin[i], KeyPressMask | KeyReleaseMask );
			}
			if ( pChildrenswin )
				XFree(pChildrenswin);
		}
		else
		{
			GOC_ERROR("XQueryTree fail");
		}

	}
	// set terminal canon mode (but don't read from terminal)
	// it's beacuse it don't let write terminal by ECHO
	goc_termInit();

	/*
	// clear events before real system start
	while ( XPending( dpy ) )
	{
		GOC_DEBUG("Removing event on init");
		XNextEvent( dpy, &event );
	}
	*/

	// start registering events - if no listener do nothing
	if ( pITerm->event )
	{
		int errcode = pthread_create(&thread, NULL, &xTermEventGenerator, NULL);
	}
	return 0;
}

static int xTermClose()
{
	// stop event generator
	if ( thread )
	{
		thread = 0;
	}

	// restore user terminal settings
	goc_termRestore();

	// close display connection
	if ( dpy )
	{
		/*
		// clear events
		XEvent event;
		while ( XPending( dpy ) )
		{
			GOC_DEBUG("Removing event on finish");
			XNextEvent( dpy, &event );
		}
		*/
		XCloseDisplay(dpy);
	}

	return 0;
}

int initModule(void *pModuleStruct)
{
	if ( pModuleStruct == NULL )
		return -1;
	if ( pITerm != NULL )
		return -1;
	pITerm = (GOC_StITerm *)pModuleStruct;
	pITerm->init = xTermInit;
	pITerm->close = xTermClose;
	return 0;
}
