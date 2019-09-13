#include <pthread.h>
#include <unistd.h>
#include <linux/kd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include "iterm.h"
#include "conflog.h"
#include <tools/term.h>
#include <tools/malloc.h>
#include "term-std.h"

pthread_t thread = 0;

// tablice konwersji kodów znaków dla systemu ze standardow± konsol±
#define GOC_REDEFSIZE 10
int xkey[GOC_REDEFSIZE] =
{
	0x0a,// Enter
	0x1b5b337e, // Del
	0x1b5b48, // Home
	0x1b5b44, // Up
	0x1b5b41, // Left
	0x1b5b43, // Right
	0x1b5b42, // Down
	0x1b5b357e, // PgUp
	0x1b5b367e, // PgDown
	0x1b5b46 // End
};
int rkey[GOC_REDEFSIZE] =
{
	0x0d, 0x116, 0x114, 0x601, 0x603, 0x602, 0x600, 0x118, 0x119, 0x117
};

static int buildStdKeyEntryCode()
{
	int key = goc_getch();
	int i;
	for (i=0; i<GOC_REDEFSIZE; i++)
	{
		if ( key == xkey[i] )
		{
			key = rkey[i];
			break;
		}
	}
	return key;
}

// Konsola, na której nie mo¿na zainicjowaæ pe³nej kontroli klawiatury
static void *linuxStdTermEventGenerator(void *arg)
{
	fd_set fds;
	int n = 0;
	int fdmax = 0;
	
	FD_ZERO( &fds );
	FD_SET( STDIN_FILENO, &fds );
	fdmax = STDIN_FILENO;

	GOC_Terminal* terminal = arg;
	GOC_CHEOP( terminal, return 0 );
	// Do until thread alive
	while (thread)
	{
		// That point wait for generation
		n = select( fdmax+1, &fds, NULL, NULL, NULL );
		if ( n > 0 )
		{
			if ( FD_ISSET( STDIN_FILENO, &fds ) )
			{
				// read key and send event
				terminal->event(GOC_ITERMEVENT_PRESSKEY, buildStdKeyEntryCode());
			}
		}
	}
	return 0;
}

static int linuxTermInit(struct GOC_Terminal* terminal)
{
	if ( goc_termInit() < 0 )
	{
		GOC_ERROR("Cannot initialize terminal");
		return -1;
	}
	// start registering events - if no listener do nothing
	if ( terminal->event )
	{
		pthread_create(&thread, NULL, &linuxStdTermEventGenerator, terminal);
	}

	return 0;
}

static int linuxTermClose(struct GOC_Terminal* term)
{
	// stop event generator
	if ( thread )
	{
		thread = 0;
//		pthread_cancel( thread );
	}

	goc_termRestore();
	free( term );
	return 0;
}

struct GOC_Terminal* goc_termStdAlloc(void (*event)(GOC_EVENTTYPE, GOC_EVENTDATA)) {
	ALLOC(GOC_Terminal, pTerm);

	pTerm->init = linuxTermInit;
	pTerm->close = linuxTermClose;
	pTerm->event = event;

	return pTerm;
}
