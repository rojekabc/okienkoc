/*
 * Wykorzystanie timer'a do zbudowania prostego zegarka
 */
#include <okienkoc/okienkoc.h>
#include <okienkoc/mystr.h>
#include <stdio.h>
#include <time.h>

GOC_HANDLER zegar = 0;
int counter = 0;

static int nasluch(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if (( msg->id == GOC_MSG_TIMERTICK_ID ) && ( uchwyt == zegar ))
	{
		GOC_StMsgTimerTick* msgtimer = (GOC_StMsgTimerTick*)msg;
		time_t ct;
		struct tm *lt;
		char buf[20];
		if ( !goc_stringEquals( msgtimer->timerid, "Zegar" ) )
			return GOC_ERR_REFUSE;
		ct = time(NULL);
		lt = localtime(&ct);
		sprintf(buf, "%02d:%02d:%02d",
			lt->tm_hour, lt->tm_min, lt->tm_sec);
		goc_labelRemLines( zegar );
		goc_labelAddLine(zegar, buf);
		GOC_MSG_PAINT( msgpaint );
		goc_systemSendMsg(zegar, msgpaint);
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

int main()
{
	GOC_StMessage wiesc;
	zegar = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 1, 40, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_labelAddLine(zegar, "Zegar");
	goc_systemSetListenerFunc( &nasluch );
	goc_systemSetTimer( zegar, "Zegar" );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
