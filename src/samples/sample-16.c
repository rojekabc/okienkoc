/*
 * Wykorzystanie timer'a do zbudowania prostego zegarka
 */
#include <okienkoc/okienkoc.h>
#include <okienkoc/mystr.h>
#include <stdio.h>
#include <time.h>

GOC_HANDLER zegar = 0;
int counter = 0;

static int nasluch(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	if (( wiesc == GOC_MSG_TIMERTICK ) && ( uchwyt == zegar ))
	{
		time_t ct;
		struct tm *lt;
		char buf[20];
		if ( !goc_stringEquals( pBuf, "Zegar" ) )
			return GOC_ERR_REFUSE;
		ct = time(NULL);
		lt = localtime(&ct);
		sprintf(buf, "%02d:%02d:%02d",
			lt->tm_hour, lt->tm_min, lt->tm_sec);
		goc_labelRemLines( zegar );
		goc_labelAddLine(zegar, buf);
		goc_systemSendMsg(zegar, GOC_MSG_PAINT, 0, 0);
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
}

int main()
{
	GOC_MSG wiesc;
	zegar = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 1, 40, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_labelAddLine(zegar, "Zegar");
	goc_systemSetListenerFunc( &nasluch );
	goc_systemSetTimer( zegar, "Zegar" );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
