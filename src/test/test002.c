/*
 * Program testujacy odczytywanie klawiszy na ró¿ne sposoby
 */
#include <okienkoc/term.h>
#include <stdio.h>
#include <okienkoc/mystr.h>
#include <signal.h>
#include <linux/keyboard.h>
#include <linux/kd.h>
//
//
int is_scan = 0;

static void die(int x)
{
	printf("Catch signal\n");
	if ( is_scan )
	{
		if ( goc_kbdRestoreMode() < 0 )
		{
			printf("False keyboard restore from scan mode\n");
			exit(-1);
		}
	}
	else
	{
		goc_termRestore();
	}
	exit(1);
}

int main(int argc, char **argv)
{
	int znak;
	if ( argc > 1 )
	{
		signal(SIGHUP, die);
		signal(SIGINT, die);
		signal(SIGQUIT, die);
		signal(SIGILL, die);
		signal(SIGTRAP, die);
		signal(SIGABRT, die);
		signal(SIGIOT, die);
		signal(SIGFPE, die);
		signal(SIGKILL, die);
		signal(SIGUSR1, die);
		signal(SIGUSR2, die);
		signal(SIGSEGV, die);
		signal(SIGPIPE, die);
		signal(SIGTERM, die);
		signal(SIGCHLD, die);
		signal(SIGCONT, die);
		signal(SIGSTOP, die);
		signal(SIGTSTP, die);
		signal(SIGTTIN, die);
		signal(SIGTTOU, die);
		if ( goc_stringEquals(argv[1], "goc_getch") )
		{
			printf("Przerywam po nacisnieciu Ctrl-C\n");
			goc_termInit();
			while ( znak = goc_getch() )
			{
				printf("Otrzymano %d (%x)\n", znak, znak);
				fflush(stdout);
			}
		}
		else if ( goc_stringEquals(argv[1], "wait") )
		{
			printf("Przerywam po nacisnieciu Ctrl-C\n");
			goc_termInit();
			while ( znak = goc_charWait() )
			{
				printf("Otrzymano %d\n", znak);
				fflush(stdout);
			}
		}
		else if ( goc_stringEquals(argv[1], "scan") )
		{
			printf("Przerywam po nacisnieciu lewego Ctrl\n");
			if ( goc_kbdScanMode() < 0 )
			{
				printf("False initialization\n");
				exit(-1);
			}
			is_scan = 1;
			while ( znak = goc_getch() )
			{
				int keycode = goc_scanToKey(znak);
				int entry = goc_keyToEntry( keycode, K_NORMTAB );
				int ascii = goc_keyToAscii( keycode );
				printf("Otrzymano scan(%x) keycode(%x) entry(%x) ascii(%x '%c' +shift %x '%c' +alt %x '%c' +altshift %x '%c'\n",
					znak, keycode, entry,
					goc_keyToAscii( 0x0000 | keycode ),
					goc_keyToAscii( 0x0000 | keycode ),
					goc_keyToAscii( 0x0100 | keycode ),
					goc_keyToAscii( 0x0100 | keycode ),
					goc_keyToAscii( 0x0200 | keycode ),
					goc_keyToAscii( 0x0200 | keycode ),
					goc_keyToAscii( 0x0300 | keycode ),
					goc_keyToAscii( 0x0300 | keycode )
						);

//				printf("Otrzymano sc=%x, kc=%x en=%x as=%x'%c'\n",
//					znak, keycode, entry, ascii, (char)ascii);
				fflush(stdout);
				if ((znak & 0xff) == 0x1d)
					die(0);
			}
		}
		else if ( goc_stringEquals(argv[1], "iscan") )
		{
			// stan klawiszy specjalnych
			int flag = 0;
			unsigned char LSHIFT = 0x10;
			unsigned char RSHIFT = 0x01;
			unsigned char LALT = 0x20;
			unsigned char RALT = 0x02;
			unsigned char LCTRL = 0x40;
			unsigned char RCTRL = 0x04;
			// TODO: Funkcja z mozliwoscia odczytania kodu scan
			// lub kodu znakowego. Mozliwosc ustawienia zglaszania
			// kiedy zostanie zwolniony klawisz oraz mozliwosc
			// ustawienia nacisniec klawiszy przelaczen
			printf("Przerywam po nacisnieciu Ctrl + c\n");
			if ( goc_kbdScanMode() < 0 )
			{
				printf("False initialization\n");
				exit(-1);
			}
			is_scan = 1;
			// znak to code scan
			while ( znak = goc_getch() )
			{
				// czy zwolniono przycisk
				int isreleased = znak & 0x80;
				// code scan bez informacji o zwolnieniu
				int freescan = znak & 0xFF7F;
				if ( isreleased )
				{
					switch ( freescan )
					{
					case 0x2a: flag &= (~LSHIFT); break;
					case 0x36: flag &= (~RSHIFT); break;
					case 0x1d: flag &= (~LCTRL); break;
					case 0xe01d: flag &= (~RCTRL); break;
					case 0x38: flag &= (~LALT); break;
					case 0xe038: flag &= (~RALT); break;
					}
					continue;
				}
				else
				{
					switch ( freescan )
					{
					case 0x2a: flag |= (LSHIFT); continue;
					case 0x36: flag |= (RSHIFT); continue;
					case 0x1d: flag |= (LCTRL); continue;
					case 0xe01d: flag |= (RCTRL); continue;
					case 0x38: flag |= (LALT); continue;
					case 0xe038: flag |= (RALT); continue;
					}
				}
				{
				// keycode to code keycode
				int keycode = goc_scanToKey(znak);
				int ascii;
				// entry to code entry
				// int entry = goc_keyToEntry( keycode );
				// ascii to code ascii z odpowiednim mappingiem
				if ( flag & (LSHIFT | RSHIFT) )
					keycode |= 0x0100;
				if ( flag & RALT )
					keycode |= 0x0200;
				// przy standardowym zachowaniu powinno byc
				// tak, ze keycode jest jak przy samym
				// naci¶niêciu SHIFT i dodawany jest kontrol
				// Tu, wyj±tkowo rozró¿niam wielko¶æ litery
				// oraz rodzaj
				if ( flag & (LCTRL | RCTRL) )
					putchar('^');
				ascii = goc_keyToAscii( keycode);
				if ( freescan^0x1c )
					putchar( ascii );
				else
					putchar('\n');
				if ((flag & (LCTRL | RCTRL)) && (ascii == 'c'))
					die(0);
				fflush(stdout);
				}
			}
		}
	}

	printf("argument goc_getch - Testuj funkcje goc_getch\n");
	printf("argument wait - Testuj funkcje goc_charWait\n");
	printf("argument scan - Testuj funkcje readScan\n");
	printf("argument iscan - Testuj funkcje readScan ale interpretuj\n");
	fflush(stdout);
	return 0;
}
