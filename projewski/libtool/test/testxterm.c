#include <stdio.h>
#include <X11/Xlib.h>
#define XK_XKB_KEYS
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#define GOC_PRINTINFO
#include <tools/log.h>

Window listChildStructures(Display *dpy, Window win)
{
		Window rootwin = 0;
		Window parentwin = 0;
		Window *pChildrenswin = NULL;
		unsigned int nChildrenswin = 0;

		if ( XQueryTree(dpy, win, &rootwin, &parentwin, &pChildrenswin, &nChildrenswin) )
		{
			int i = 0;
			GOC_BINFO("Listed win id is %u", win);
			GOC_BINFO("   ROOT win id is %u", rootwin);
			GOC_BINFO("   Parent win id is %u", parentwin);
			GOC_BINFO("   Number of childs is %u", nChildrenswin);
			for ( i=0; i<nChildrenswin; i++ )
			{
				GOC_BINFO("      Id of the chilren %u is %u", i, pChildrenswin[i]);
				listChildStructures(dpy, pChildrenswin[i]);
			}
			if ( pChildrenswin )
			{
				XFree(pChildrenswin);
				pChildrenswin = NULL;
				nChildrenswin = 0;
			}
			return rootwin;
		}
		else
		{
			GOC_ERROR("XQueryTree fail");
			return NULL;
		}

}

int catchEvent(Display* dpy, Window win) {
	XEvent event;

	Window rootwin = 0;
	Window parentwin = 0;
	Window *pChildrenswin = NULL;
	int nChildrenswin = 0;
	int i;

	// catch keyboard for this window and all childrens
	XSelectInput(dpy, win, KeyPressMask | KeyReleaseMask );
	if ( XQueryTree(dpy, win, &rootwin, &parentwin, &pChildrenswin, &nChildrenswin) ) {
		for ( i=0; i<nChildrenswin; i++ )
		{
			XSelectInput(dpy, pChildrenswin[i], KeyPressMask | KeyReleaseMask );
		}
		if ( pChildrenswin )
		{
			XFree(pChildrenswin);
			pChildrenswin = NULL;
			nChildrenswin = 0;
		}
	}

	// next keyboard event
	XNextEvent(dpy, &event);
	switch (event.type)
	{
	case KeyPress:
		GOC_INFO("Key pressed");
		break;
	case KeyRelease:
		GOC_INFO("Key released");
		break;
	default:
		GOC_INFO("Unknown key event");
		break;
	}
}

int main(int argc, char **argv)
{
	Display *dpy = NULL;
	Window win;
	XEvent event;
	int bar;
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
	// wylistowanie dzieciakow okna z focusem
	listChildStructures(dpy, win);
	// wylitowanie od root-a
	// listChildStructures(dpy, listChildStructures(dpy, win));
	// catchEvent(dpy, win);
	if ( dpy )
	{
		XCloseDisplay(dpy);
	}
	return 0;
}
