#include <stdio.h>
#include <string.h>
#include "ramka.h"
#include "global.h"
#include "element.h"
#include "screen.h"
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

int goc_frameListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StFrame* ramka = (GOC_StFrame*)malloc(sizeof(GOC_StFrame));
		memcpy(ramka, elem, sizeof(GOC_StElement));
		memcpy(ramka->znak, "*-*| |*-*", 9);
		*retuchwyt = (GOC_HANDLER)ramka;
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_PAINT )
	{
		return framePaint((GOC_StFrame*)uchwyt);
	}
	else if ( wiesc == GOC_MSG_DESTROY )
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
