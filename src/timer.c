#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include "global.h"
#include "system.h"
#include "tablica.h"
#include "mystr.h"
#include "global-inc.h"

const char* GOC_MSG_TIMERTICK_ID = "GOC_MsgTimerTick";

static GOC_HANDLER *pHandler = NULL;
static _GOC_TABEL_SIZETYPE_ nHandler = 0;
static char **pId = NULL;
static int milisec = 100;
//static int tickCounter = 0;

static void alarmTick(int a)
{
	int i;
	for (i=0; i<nHandler; i++) {
		GOC_MSG_TIMERTICK(msg, pId[i]);
		goc_systemSendMsg(pHandler[i], msg);
	}
}

int goc_systemSetTimer(GOC_HANDLER uchwyt, const char *id)
{
	if ( nHandler == 0 )
	{
		// wystartuj czasomierz
		struct itimerval tv;
		struct sigaction sa;
		memset( &sa, 0, sizeof( struct sigaction ) );
		sa.sa_handler = &alarmTick;
		sigaction(SIGALRM, &sa, 0);
		memset( &tv, 0, sizeof( struct itimerval ) );
		tv.it_value.tv_usec = 100;
		tv.it_interval.tv_usec = milisec * 1000;
		setitimer(ITIMER_REAL, &tv, 0);
	}
	pHandler = goc_tableAdd(pHandler, &nHandler, sizeof(GOC_HANDLER));
	pHandler[nHandler-1] = uchwyt;
	nHandler--;
	pId = goc_tableAdd(pId, &nHandler, sizeof(char *));
	pId[nHandler-1] = goc_stringCopy(NULL, id);
	return GOC_ERR_OK;
}

int goc_systemFreeTimer(GOC_HANDLER uchwyt)
{
	int i;
	for ( i=0; i<nHandler; i++ )
	{
		if ( pHandler[i] == uchwyt )
		{
			pHandler = goc_tableRemove(
				pHandler, &nHandler, sizeof(GOC_HANDLER), i);
			nHandler++;
			pId[i] = goc_stringFree(pId[i]);
			pId = goc_tableRemove(
				pId, &nHandler, sizeof(char *), i);
			return GOC_ERR_OK;
		}
	}
	return GOC_ERR_END;
}

int goc_systemFreeTimers()
{
	int i;
	for ( i=0; i<nHandler; i++ )
		pId[i] = goc_stringFree(pId[i]);
	pHandler = goc_tableClear( pHandler, &nHandler );
	nHandler = i;
	pId = goc_tableClear( pId, &nHandler);
	return GOC_ERR_OK;
}
