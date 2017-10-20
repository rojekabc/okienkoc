#include <stdio.h>
#include <string.h>
#include "ramka.h"
#include "system.h"
#include "global.h"
#include "element.h"
#include <tools/screen.h>
#include "global-inc.h"

const char *GOC_ELEMENT_FRAME = "GOC_Frame";;

// Zrezygnowano z ramkaRozwijaj
int goc_frameDrawer(GOC_StElement *ramka, const char *znak)
{
	register GOC_POSITION i;
	register GOC_POSITION x, y, w, d;
	
	x = goc_elementGetScreenX( (GOC_HANDLER)ramka );
	y = goc_elementGetScreenY( (GOC_HANDLER)ramka );
	d = goc_elementGetWidth( (GOC_HANDLER)ramka );
	w = goc_elementGetHeight( (GOC_HANDLER)ramka );
	goc_textallcolor(ramka->color);
	// rysuj linie pozioma
	goc_gotoxy(x, y++);
	putchar(znak[0]);
	for (i=2; i<d; i++)
		putchar(znak[1]);
	if ( d^1 )
		putchar(znak[2]);
	w--;
	if ( !w )
	  	return GOC_ERR_OK;
	w--;
	// rysuj kolejne linie
	while ( w ) {
		goc_gotoxy(x, y++);
		putchar(znak[3]);
		for (i=2; i<d; i++)
			putchar(znak[4]);
		if ( d^1 )
			putchar(znak[5]);
		w--;
	}
	// rysuj ostatnia linie
	w++;
	goc_gotoxy(x, y++);
	putchar(znak[6]);
	for (i=2; i<d; i++)
		putchar(znak[7]);
	if ( d^1 )
		putchar(znak[8]);
	return GOC_ERR_OK;
}

// zwróæ ustawion± funkcjê rysuj±c± napis, a je¶li nie ma to domy¶lnie
// stosowan± do rysowania napisów
GOC_FUN_FRAMEDRAWER *goc_elementGetFrameDrawer(GOC_HANDLER uchwyt)
{
	void *tmp = goc_elementGetFunc(uchwyt, GOC_FUNID_FRAMEDRAWER);
	if ( tmp )
		return tmp;
	return &goc_frameDrawer;
}

static int framePaint(GOC_StFrame *ramka)
{
	ramka->flag |= GOC_EFLAGA_SHOWN;
	goc_elementGetFrameDrawer((GOC_HANDLER)ramka)((GOC_StElement*)ramka, ramka->znak);
	fflush(stdout);
	return GOC_ERR_OK;
}

int goc_frameListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT(GOC_StFrame, ramka, msg);
		memcpy(ramka->znak, "*-*| |*-*", 9);
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_PAINT_ID )
	{
		return framePaint((GOC_StFrame*)uchwyt);
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		return goc_elementDestroy(uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}

int goc_frameSetPoints(GOC_HANDLER u, char *p)
{
	memcpy(((GOC_StFrame*)u)->znak, p, 9);
	return GOC_ERR_OK;
}
