#include <pthread.h>
#include <unistd.h>
#include <linux/kd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include "term.h"
#include "iterm.h"
#include "conflog.h"
#include <tools/term.h>

struct GOC_StITerm *pITerm = NULL;
pthread_t thread = 0;

static int keyboardFlags = 0;

/*
 * Funkcja:
 * Funkcja generuj±ca wie¶æ po otrzymaniu z klawiatury naci¶niêtego przycisku
 *
 * Argumenty:
 * Brak.
 *
 * Zwraca:
 * 1 na otrzymanie, 0 na brak.
 *
 */
static int buildKeyEntryCode(int scan)
{
	int key = goc_scanToKey( scan );
	int entry;
	int ktable = K_NORMTAB;
	if ( keyboardFlags & GOC_KBD_ANYSHIFT )
		ktable |= K_SHIFTTAB;
	if ( keyboardFlags & GOC_KBD_ALTR )
		ktable |= K_ALTTAB;
	if ( keyboardFlags & GOC_KBD_CAPSL )
	{
		if (( scan > 0xd ) && ( scan != 0x29 ))
		{
			if ( ktable & K_SHIFTTAB )
				ktable &= ~K_SHIFTTAB;
			else
				ktable |= K_SHIFTTAB;
		}
	}
	entry = goc_keyToEntry(key, ktable);
	if ( ( entry & 0xb00 ) == 0xb00 )
		entry &= 0xff;
	return entry;
}

static void keyboardGenerator()
{
	int scan = goc_getch();

	if ( scan & 0x0080 )
	{
		switch ( scan & 0xff7f )
		{
			case 0x2a:
				keyboardFlags &= ~GOC_KBD_SHIFTL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x36:
				keyboardFlags &= ~GOC_KBD_SHIFTR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x1c:
				pITerm->event(GOC_ITERMEVENT_RELEASEKEY, 0x0D);
				break;
			case 0x38:
				keyboardFlags &= ~GOC_KBD_ALTL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0xe038:
				keyboardFlags &= ~GOC_KBD_ALTR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x1d:
				keyboardFlags &= ~GOC_KBDCTRLL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0xe01d:
				keyboardFlags &= ~GOC_KBD_CRLR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x3a:
				// Ostateczna zmiana flagi dla Caps Lock
				if ( goc_isLedFlag(K_CAPSLOCK) )
					goc_clrLedFlag(K_CAPSLOCK);
				else
					goc_ledSetFlag(K_CAPSLOCK);
				break;
			case 0xe05b:
				keyboardFlags &= ~GOC_KBD_WINL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0xe05d:
				keyboardFlags &= ~GOC_KBD_WINR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			default:
				pITerm->event(GOC_ITERMEVENT_RELEASEKEY, buildKeyEntryCode(scan));
		}
	}
	else
	{
		switch ( scan )
		{
			case 0x2a:
				keyboardFlags |= GOC_KBD_SHIFTL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x36:
				keyboardFlags |= GOC_KBD_SHIFTR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x1c: // Enter
				pITerm->event(GOC_ITERMEVENT_PRESSKEY, 0x0D);
				break;
			case 0x38:
				keyboardFlags |= GOC_KBD_ALTL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0xe038:
				keyboardFlags |= GOC_KBD_ALTR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x1d:
				keyboardFlags |= GOC_KBDCTRLL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0xe01d:
				keyboardFlags |= GOC_KBD_CRLR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0x3a:
				// zapalenie swiatelek i ustalenie flagi
				// wewnatrz biblioteki
				if ( goc_isLedFlag(K_CAPSLOCK) == goc_isLedOn(LED_CAP) )
				{
					if ( keyboardFlags & GOC_KBD_CAPSL )
					{
						keyboardFlags &= ~GOC_KBD_CAPSL;
						goc_ledTurnOff(LED_CAP);
					}
					else
					{
						keyboardFlags |= GOC_KBD_CAPSL;
						goc_ledTurnOn(LED_CAP);
					}
					pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				}
				break;
			case 0xe05b:
				keyboardFlags |= GOC_KBD_WINL;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			case 0xe05d:
				keyboardFlags |= GOC_KBD_WINR;
				pITerm->event(GOC_ITERMEVENT_KBDSTATE, keyboardFlags);
				break;
			default:
			{
				pITerm->event(GOC_ITERMEVENT_PRESSKEY, buildKeyEntryCode(scan));
			}
		}
	}
}

// Konsola z pe³n± kontrol± klawiatury
static void *linuxTermEventGenerator(void *arg)
{
	fd_set fds;
	int n = 0;
	int fdmax = 0;
	
	FD_ZERO( &fds );
	FD_SET( STDIN_FILENO, &fds );
	fdmax = STDIN_FILENO;

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
				keyboardGenerator();
			}
		}
	}
	return 0;
}

// tablice konwersji kodów znaków dla systemu ze standardow± konsol±
#define GOC_REDEFSIZE 10
int xkey[GOC_REDEFSIZE] =
{
	0x0a,// Enter
	0x1b5b337e, // Del
	0x1b5b317e, // Home
	0x1b5b44, // Up
	0x1b5b41, // Left
	0x1b5b43, // Right
	0x1b5b42, // Down
	0x1b5b357e, // PgUp
	0x1b5b367e, // PgDown
	0x1b5b347e // End
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
				pITerm->event(GOC_ITERMEVENT_PRESSKEY, buildStdKeyEntryCode());
			}
		}
	}
	return 0;
}

static int linuxTermInit()
{
	if ( goc_kbdCanReadKbdMode() )
	{
		if ( goc_kbdScanMode() < 0 )
		{
			GOC_ERROR("Cannot initialize keyboard raw mode");
			return -1;
		}
		// start registering events - if no listener do nothing
		if ( pITerm->event )
		{
			pthread_create(&thread, NULL, &linuxTermEventGenerator, NULL);
		}
	}
	else
	{
		if ( goc_termInit() < 0 )
		{
			GOC_ERROR("Cannot initialize terminal");
			return -1;
		}
		// start registering events - if no listener do nothing
		if ( pITerm->event )
		{
			pthread_create(&thread, NULL, &linuxStdTermEventGenerator, NULL);
		}
	}

	return 0;
}

static int linuxTermClose()
{
	// stop event generator
	if ( thread )
	{
		thread = 0;
//		pthread_cancel( thread );
	}

	if ( goc_kbdCanReadKbdMode() )
	{
		return goc_kbdRestoreMode();
	}
	else
	{
		return goc_termRestore();
	}
}

int initModule(void *pModuleStruct)
{
	if ( pModuleStruct == NULL )
		return -1;
	if ( pITerm != NULL )
		return -1;
	pITerm = (GOC_StITerm *)pModuleStruct;
	pITerm->init = linuxTermInit;
	pITerm->close = linuxTermClose;
	return 0;
}
