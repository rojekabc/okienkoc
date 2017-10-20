/*
 * Ruchomy napis z uzyciem Timer'a i funkcji zmienionego rysownika
 */
#include <okienkoc/okienkoc.h>
#include <okienkoc/mystr.h>
#include <stdio.h>
#ifndef _DEBUG
#	include <string.h>
#endif

int counter = 0;
char direction = 0;
char wait = 0;
int width = 0;

int labelMoveDrawer(GOC_StElement *e, const char *tekst)
{
	if ( wait )
	{
		wait--;
		return GOC_ERR_OK;
	}
	wait = 1;

	if ( width != goc_elementGetWidth( (GOC_HANDLER)e ) )
	{
		counter = 0;
		direction = 0;
		width = goc_elementGetWidth( (GOC_HANDLER)e );
	}

	goc_textallcolor(e->color);
	if ( width <= strlen( tekst ) )
	{
		char *tekstPisany = (char *)malloc(width+1);
		memset( tekstPisany, 0, width+1 );
		memcpy( tekstPisany, tekst, width );
		goc_saveXY();
		goc_gotoxy(
			goc_elementGetScreenX( (GOC_HANDLER)e ),
			goc_elementGetScreenY( (GOC_HANDLER)e ));
		puts(tekstPisany);
		goc_loadXY();
		return GOC_ERR_OK;
	}

	if ( direction )
	{
		if ( counter > 0 )
		{
			counter--;
			goc_saveXY();
			goc_gotoxy(
				goc_elementGetScreenX( (GOC_HANDLER)e ) + counter,
				goc_elementGetScreenY( (GOC_HANDLER)e ));
			printf("%s ", tekst);
			goc_loadXY();
		}
		else
		{
			direction = 0;
		}
	}
	else
	{
		if ( counter < width-strlen(tekst) )
		{
			counter++;
			goc_saveXY();
			goc_gotoxy(
				goc_elementGetScreenX( (GOC_HANDLER)e ) + counter - 1,
				goc_elementGetScreenY( (GOC_HANDLER)e ));
			printf(" %s", tekst);
			goc_loadXY();
		}
		else
		{
			direction = 1;
		}
	}
	return GOC_ERR_OK;
}

static int nasluch(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_TIMERTICK_ID )
	{
		GOC_MSG_PAINT( msgpaint );
		goc_systemSendMsg(uchwyt, msgpaint);
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER napis;
	napis = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 1, 0, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_labelAddLine(napis, "Ruchomy napis");
	goc_elementSetFunc( napis, GOC_FUNID_LISTENER, &nasluch );
	goc_elementSetFunc( napis, GOC_FUNID_LABELDRAWER,
		&labelMoveDrawer );
	goc_systemSetTimer( napis, "RuchNapis" );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
