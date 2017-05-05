#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "slidebar.h"
#include "screen.h"

const char* GOC_ELEMENT_SLIDEBAR = "GOC_SlideBar";

static void slidebarMove(GOC_HANDLER h, GOC_POSITION newp)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	GOC_POSITION x = goc_elementGetScreenX( h );
	GOC_POSITION y = goc_elementGetScreenY( h );
	goc_textallcolor( slide->color );
	if ( slide->height == 1 )
		goc_gotoxy(x + slide->position, y);
	else if ( slide->width == 1 )
		goc_gotoxy(x, y + slide->position);
	putchar(slide->emptycharacter);
	slide->position = newp;
	if ( slide->height == 1 )
		x += slide->position;
	else if ( slide->width == 1 )
		y += slide->position;
	goc_gotoxy( x, y );
	putchar( slide->character );
	goc_gotoxy( x, y );
	fflush(stdout);
}

static int slidebarPaint(GOC_HANDLER h)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	GOC_POSITION x = goc_elementGetScreenX( h );
	GOC_POSITION y = goc_elementGetScreenY( h );
	GOC_POSITION width = goc_elementGetWidth(h);
	GOC_POSITION height = goc_elementGetHeight(h);
	goc_textallcolor( slide->color );
	if ( slide->height == 1 )
	{
		goc_gotoxy(x, y);
		while ( width-- )
		{
			putchar( slide->emptycharacter );
		}
		x += slide->position;
	}
	else if ( slide->width == 1 )
	{
		GOC_POSITION i = y;
		while ( height-- )
		{
			goc_gotoxy(x, i++);
			putchar( slide->emptycharacter );
		}
		y += slide->position;
	}
	goc_gotoxy(x, y);
	putchar( slide->character );
	goc_gotoxy(x, y);
	fflush(stdout);
	return GOC_ERR_OK;
}

static int slideHotKeyHome(GOC_HANDLER h, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	slidebarMove(h, 0);
	return GOC_ERR_OK;
}

static int slideHotKeyEnd(GOC_HANDLER h, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_POSITION width = goc_elementGetWidth(h);
	GOC_POSITION height = goc_elementGetHeight(h);
	if ( width == 1 )
		slidebarMove( h, height - 1 );
	else if ( height == 1 )
		slidebarMove( h, width - 1 );
	return GOC_ERR_OK;
}

static int slideHotKeyLeft(GOC_HANDLER h, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	GOC_POSITION height = goc_elementGetHeight(h);
	if (( height == 1 ) && ( slide->position > 0))
		slidebarMove( h, slide->position - 1 );
	return GOC_ERR_OK;
}

static int slideHotKeyRight(GOC_HANDLER h, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	GOC_POSITION width = goc_elementGetWidth(h);
	GOC_POSITION height = goc_elementGetHeight(h);
	if (( height == 1 ) && ( slide->position < width-1 ))
		slidebarMove( h, slide->position + 1 );
	return GOC_ERR_OK;
}
static int slideHotKeyUp(GOC_HANDLER h, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	GOC_POSITION width = goc_elementGetWidth(h);
	if (( width == 1 ) && ( slide->position > 0))
		slidebarMove( h, slide->position - 1 );
	return GOC_ERR_OK;
}
static int slideHotKeyDown(GOC_HANDLER h, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	GOC_POSITION width = goc_elementGetWidth(h);
	GOC_POSITION height = goc_elementGetHeight(h);
	if (( width == 1 ) && ( slide->position < height-1))
		slidebarMove( h, slide->position + 1 );
	return GOC_ERR_OK;
}

int goc_slidebarListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf)
{
	if ( wiesc == GOC_MSG_PAINT )
	{
		return slidebarPaint(uchwyt);
	}
	else if ( wiesc == GOC_MSG_FOCUS )
	{
		GOC_StSlideBar* slide = (GOC_StSlideBar*)uchwyt;
		if ( !(slide->flag & GOC_EFLAGA_PAINTED) )
			return GOC_ERR_REFUSE;
		if ( !(slide->flag & GOC_EFLAGA_ENABLE) )
			return GOC_ERR_REFUSE;
		slidebarMove(uchwyt, slide->position);
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_FOCUSFREE )
	{
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StSlideBar* slide = (GOC_StSlideBar*)malloc(sizeof(GOC_StSlideBar));
		memcpy(slide, elem, sizeof(GOC_StElement));
		if (( elem->height != 1 ) && ( elem->width != 1 ))
			elem->height = 1;
		slide->position = 0;
		slide->character = '*';
		*retuchwyt = (GOC_HANDLER)slide;

		goc_hkAdd(*retuchwyt, 0x114, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			slideHotKeyHome);
		goc_hkAdd(*retuchwyt, 0x117, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			slideHotKeyEnd);
		if ( elem->height == 1 )
		{
			goc_hkAdd(*retuchwyt, 0x602, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
				slideHotKeyRight);
			goc_hkAdd(*retuchwyt, 0x601, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
				slideHotKeyLeft);
			slide->emptycharacter = '-';
		}
		else if ( elem->width == 1 )
		{
			goc_hkAdd(*retuchwyt, 0x603, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
				slideHotKeyUp);
			goc_hkAdd(*retuchwyt, 0x600, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
				slideHotKeyDown);
			slide->emptycharacter = '|';
		}
	}
	return GOC_ERR_UNKNOWNMSG;
}

int goc_slidebarSetPosition(GOC_HANDLER h, GOC_POSITION position)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	if ( slide->height == 1 )
	{
		GOC_POSITION w = goc_elementGetWidth(h);
		if ( position >= w )
			slide->position = w - 1;
		else if ( position < 0 )
			slide->position = 0;
		else
			slide->position = position;
	}
	else if ( slide->width == 1 )
	{
		GOC_POSITION height = goc_elementGetHeight(h);
		if ( position >= height )
			slide->position = height - 1;
		else if ( position < 0 )
			slide->position = 0;
		else
			slide->position = position;
	}
	else
		return GOC_ERR_WRONGARGUMENT;
	if ( slide->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(h, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

int goc_slidebarSetChar(GOC_HANDLER h, char ch, char emptych)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	slide->character = ch;
	slide->emptycharacter = emptych;
	if ( slide->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(h, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

GOC_POSITION goc_slidebarGetPosition(GOC_HANDLER h)
{
	GOC_StSlideBar* slide = (GOC_StSlideBar*)h;
	return slide->position;
}
