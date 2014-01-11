#include <stdio.h>
#include <X11/Xlib.h>
#define XK_XKB_KEYS
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#define GOC_PRINTINFO
#include <okienkoc/log.h>

Window listChildStructures(Display *dpy, Window win)
{
		Window rootwin = 0;
		Window parentwin = 0;
		Window *pChildrenswin = NULL;
		int nChildrenswin = 0;
		if ( XQueryTree(dpy, win, &rootwin, &parentwin, &pChildrenswin, &nChildrenswin) )
		{
			int i = 0;
			GOC_BINFO("Listed win id is %d", win);
			GOC_BINFO("   ROOT win id is %d", rootwin);
			GOC_BINFO("   Parent win id is %d", parentwin);
			GOC_BINFO("   Number of childs is %d", nChildrenswin);
			for ( i=0; i<nChildrenswin; i++ )
			{
				GOC_BINFO("      Id of the chilren %d is %d", i, pChildrenswin[i]);
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
	if ( dpy )
	{
		XCloseDisplay(dpy);
	}
	return 0;
}
