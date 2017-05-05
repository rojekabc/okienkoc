#include "global-inc.h"
#include "conflog.h"
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#ifndef __CYGWIN__
#	include <linux/kd.h>
#	include <linux/vt.h>
#	include <linux/keyboard.h>
#endif
#include <errno.h>
#ifndef _DEBUG
#	include <string.h>
#endif

struct termios old_tio;
static int kbdmode;
// static int consolefd;
/*static int term_scancode = 0;*/

int goc_kbdCanReadKbdMode()
{
	int kbdm;
	int fd = STDIN_FILENO;
	if ( ioctl(fd, KDGKBMODE, &kbdm) )
		return 0;
	return 1;
}

// Przelaczenie sie na tryb czytania scan code klawiatury
// 0 func zadziala bez problemu
// -1 wystapil problem
// NIE UZYWAC W POLACZENIU Z FUNKCJA goc_termInit / goc_termRestore
int goc_kbdScanMode(void)
{
	struct termios tio;
	int fd = STDIN_FILENO;
	/*
	fd = open("/dev/tty", O_RDONLY);
	if ( fd == -1 )
	{
		GOC_ERROR("Cannot open tty");
		return -1;
	}
	*/

//	consolefd = open("/dev/tty", O_RDONLY);
//	if ( consolefd < 0 )
//	{
//		printf("Console\n");
//		return -1;
//	}

	// Read terminal settings
	if ( tcgetattr(fd, &old_tio) < 0 )
	{
		GOC_ERROR("Cannot read terminal settings");
		return -1;
	}
	memcpy(&tio, &old_tio, sizeof(struct termios));

	// Read keyboard mode
	if ( ioctl(fd, KDGKBMODE, &kbdmode) )
	{
		GOC_BERROR("Cannot read keyboard mode [%s] for that terminal type [%s]", strerror(errno), ttyname(fd));
		return -1;
	}

	// Set canon terminal settings
	tio.c_lflag &= ~(ICANON | ECHO | ISIG);
	tio.c_iflag = 0;
	tio.c_cc[VMIN] = 1; /* bufor pobierania znakow */
	tio.c_cc[VTIME] = 0; /* 0.1 sec intercharacter timeout */
 	if (tcsetattr(fd, TCSAFLUSH, &tio) < 0)
	{
		GOC_ERROR("Cannot set terminal setting");
		return -1;
	}

	// Set raw keyboard mode
	if ( ioctl(fd, KDSKBMODE, K_RAW) )
	{
		GOC_ERROR("Cannot set keyboard mode");
		return -1;
	}
	return 0;
}

int goc_screenGetWidth()
{
	struct winsize wsize;
	if ( ioctl(STDIN_FILENO, TIOCGWINSZ, &wsize) )
	{
		return -1;
	}
	return wsize.ws_col;
}

int goc_screenGetHeight()
{
	struct winsize wsize;
	if ( ioctl(STDIN_FILENO, TIOCGWINSZ, &wsize) )
	{
		return -1;
	}
	return wsize.ws_row;
}

// przelacz na oryginalny tryb klawiatury
int goc_kbdRestoreMode(void)
{
	// zachowaj obecny tryb
	if ( ioctl(STDIN_FILENO, KDSKBMODE, kbdmode) )
		return -1;
  	if (tcsetattr(STDIN_FILENO, 0, &old_tio) < 0)
		return -1;
	return 0;
}

// Zamiana kodu scan na code key
// Kod key to wskazanie samego klawisza, nie tego co przesz³o przez system
// i okre¶la jaki znak ma byæ wypisany
// Lista klawiszy znajduje siê w linux/input.h
int goc_scanToKey(int scan)
{
	struct kbkeycode a;
	// wyczysc bit informuj±cy o zdarzeniu puszczenia
	scan &= 0xff7f;
	// jesli code specjalny to szukaj w tablicy 0x80 keycode'ow
	if ( scan & 0xf000 )
	{
		scan &= 0xff;
		scan |= 0x80;
	}
	a.scancode = scan;
	a.keycode &= 0;
	if ( ioctl(STDIN_FILENO, KDGETKEYCODE, &a) < 0 )
		GOC_ERROR("KDGETKEYCODE error");
	return a.keycode;
}

// Zamiana kodu key na wartosc entry
// kbdTable to:
// K_NORMTAB; // K_SHIFTTAB / K_ALTTAB / K_ALTSHIFTTAB
//
// Zwracane rodzaje warto¶ci:
// 0XX - cyfra, TAB, Spacja, Backspace
// bXX - znak normalny
// 1xx - F?, Home itd
// 6xx - strzalki
// 7xx - ALT/ALTGR/SHIFT/CTRL
// 200 - to raczej nie zinterpretowany (Windowsowe nie s± odró¿niane)
// 201 - Enter
int goc_keyToEntry(int kc, int kbdTable)
{
	struct kbentry ke;
	ke.kb_index = kc;
	ke.kb_table = kbdTable;
	ke.kb_value &= 0;
	if ( ioctl(STDIN_FILENO, KDGKBENT, &ke) )
		GOC_ERROR("KDGKBENT error");
	return ke.kb_value;
}

// Zamiana kodu key na wartosc ascii
// UWAGA ! To jest tak naprawde goc_keyToEntry z dodatkowa interpretacja
// entry mniej wiêcej (chyba) - mówi o ¼ródle znaku i jego interpretacji
// w systemie.
int goc_keyToAscii(int kc)
{
	struct kbentry ke;
	int ascii = 0;
	ke.kb_index = kc;
	ke.kb_table = (kc & 0xFF00) >> 8;
	ke.kb_value = 0;
	ioctl(STDIN_FILENO, KDGKBENT, &ke);
	ascii = ke.kb_value; // cyfry sa bez niczego
	if ( ascii & 0xff00 )
	{
		// B = 1011
		if ( (ascii & 0x0b00) == 0x0b00 ) //znaki normalne
			ascii = ascii & 0x00FF; // zostaw tylko code znaku
		/*
		else if ( (ascii & 0x0100) == 0x0100 ) // F?.. Home..
		else if ( (ascii & 0x0600) == 0x0600 ) // strzalki
		else if ( (ascii & 0x0700) == 0x0700 ) // alt/ctrl/shift
		*/
		else
			ascii = 0; // jakies inne
	}
	return ascii;
}

void goc_ledTurnOn(int led)
{
	unsigned int leds;
	ioctl(STDIN_FILENO, KDGETLED, &leds);
	leds |= led;
	ioctl(STDIN_FILENO, KDSETLED, leds);
}

void goc_ledTurnOff(int led)
{
	unsigned int leds;
	ioctl(STDIN_FILENO, KDGETLED, &leds);
	leds &= ~led;
	ioctl(STDIN_FILENO, KDSETLED, leds);
}

int goc_isLedOn(int led)
{
	unsigned int leds;
	ioctl(STDIN_FILENO, KDGETLED, &leds);
	return leds & led;
}

void goc_ledSetFlag(int led)
{
	unsigned int leds;
	ioctl(STDIN_FILENO, KDGKBLED, &leds);
	leds |= led;
	ioctl(STDIN_FILENO, KDSKBLED, leds);
}

void goc_clrLedFlag(int led)
{
	unsigned int leds;
	ioctl(STDIN_FILENO, KDGKBLED, &leds);
	leds &= ~led;
	ioctl(STDIN_FILENO, KDSKBLED, leds);
}

int goc_isLedFlag(int led)
{
	unsigned int leds;
	ioctl(STDIN_FILENO, KDGKBLED, &leds);
	return leds & led;
}

void goc_termSwitchTo(int num)
{
	ioctl(STDIN_FILENO, VT_ACTIVATE, num);
	ioctl(STDIN_FILENO, VT_WAITACTIVE, num);
}

/* initialze terminal */
int goc_termInit(void)
{
  struct termios tio;

  if(tcgetattr(STDIN_FILENO, &tio) < 0) return -1; /* blad przy czytaniu ustawien terminala */

  old_tio=tio;
  // cfmakeraw
/*  tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tio.c_oflag &= ~OPOST;
  tio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tio.c_cflag &= ~(CSIZE | PARENB);
  tio.c_cflag |= CS8;*/

  tio.c_lflag &= ~(ICANON|ECHO); 
  tio.c_cc[VMIN] = 1;
  tio.c_cc[VTIME] = 0;

  if(tcsetattr(STDIN_FILENO, TCSANOW, &tio) < 0) return -1; /* blad przy zapisie ustawien terminala */
  return 0;
}

int goc_termRestore(void)
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_tio) < 0) return -1;
  return 0;
}

int goc_getch()
{
	unsigned char buf[5] = {0,0,0,0,0};
	register unsigned int ret = 0;
	register unsigned char *ptr;

	read(STDIN_FILENO, buf, 4);
	ptr = buf;
	while ( *ptr )
	{
		ret <<= 8;
		ret |= *ptr;
		ptr++;
	}
	return ret;
}

/*
 * Sprawdz czy pojawil sie znak i zwroc != 0 jesli tak i wypelnij znak
 * Jesli sie nie pojawil zwroc 0 i wypelnij znak na 0
 * DZIALA POPRAWNIE PO ZAINICJOWANIU TERMINALA !!!
 */
int goc_isZnak()
{
	fd_set fds;
	struct timeval tv;
	int n = 0;
//	int n;
//	int get;
//	int add;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO( &fds );
	FD_SET( STDIN_FILENO, &fds );
	n = select( STDIN_FILENO+1, &fds, NULL, NULL, &tv );
	if ( n <= 0 ) // nic albo b³±d
		return 0;
//	{
//		if ( znak )
//			*znak = 0;
//	}
	return 1; // jest gotowy znak
	// na razie max zwrócony to 5 znaków
//	n = read( STDIN_FILENO, get, 4 );
//	n = read( STDIN_FILENO, add, 4 );

	/* znak jest normalny */
//	if (get < 0xFF) {
//		return get;
//	}
	/* pochodzi z naci¶niêcia Alt + co¶ */
//	if ((get & 0xFFFF00FF) == 0x0000001B) {
//		get &= 0x0000FF00;
//		get >>= 8;
//		return (0x00000100|get);
//	}
	/* pochodzi z klawiszy strza³ek */
//	if ((get & 0xFF00FFFF) == 0x00005B1B) {
//		get &= 0x00FF0000;
//		get >>= 16;
//		return (0x00000100|get);
//	}
	/* pochodzi z klawiszy PgUP itd. */
//	if ((get & 0xFF00FFFF) == 0x7E005B1B) {
//		get &= 0x00FF0000;
//		get >>= 16;
//		return (0x00000100|get);
//	}
	/* Jest miêdzy F1 a F5 */
//	if ((get & 0x00FFFFFF) == 0x005B5B1B) {
//		get &= 0xFF000000;
//		get >>= 24;
//		return (0x00001000|get);
//	}
	/* Jest miêdzy F6 a F8 */
//	if ((get & 0x00FFFFFF) == 0x00315B1B) {
//		get = add;
//		get &= 0xFF000000;
//		get >>= 24;
//		return (0x00001000|get);
//	}
	/* Jest miêdzy F9 a F12 */
//	if ((get & 0x00FFFFFF) == 0x00325B1B) {
//		get = add;
//		goc_getch();
//		get &= 0xFF000000;
//		get >>= 24;
//		return (0x00001000|get);
//	}
//	return 0; // unknown;
}

int goc_getchar() {
/*
    zwracane warto¶ci: 
	-1 nie rozpoznany rodzaj klawisza
0xABB:
Kod A:
	 0 tzn. normalny klawisz (code BB = 0x00 -> 0xFF)
	 1 tzn. specjalny lub z nacisniêcia z Alt'em (ale nie daj±cy jakiego¶ znaku normalnie)
	 2 tzn. F() (code BB=<numer>)
	
	w znak jest jego warto¶æ
*/
    int get = goc_getch();
    /* znak jest normalny */
    if (get < 0xFF) {
	return get;
    }
    /* pochodzi z naci¶niêcia Alt + co¶ */
    if ((get & 0xFFFF00FF) == 0x0000001B) {
	get &= 0x0000FF00;
	get >>= 8;
	return (0x00000100|get);
    }
    /* pochodzi z klawiszy strza³ek */
    if ((get & 0xFF00FFFF) == 0x00005B1B) {
	get &= 0x00FF0000;
	get >>= 16;
	return (0x00000100|get);
    }
    /* pochodzi z klawiszy PgUP itd. */
    if ((get & 0xFF00FFFF) == 0x7E005B1B) {
	get &= 0x00FF0000;
	get >>= 16;
	return (0x00000100|get);
    }
    /* Jest miêdzy F1 a F5 */
    if ((get & 0x00FFFFFF) == 0x005B5B1B) {
	get &= 0xFF000000;
	get >>= 24;
	return (0x00001000|get);
    }
    /* Jest miêdzy F6 a F8 */
    if ((get & 0x00FFFFFF) == 0x00315B1B) {
	goc_getch();
	get &= 0xFF000000;
	get >>= 24;
	return (0x00001000|get);
    }
    /* Jest miêdzy F9 a F12 */
    if ((get & 0x00FFFFFF) == 0x00325B1B) {
	goc_getch();
	get &= 0xFF000000;
	get >>= 24;
	return (0x00001000|get);
    }
    return -1;
}

int goc_charWait()
{
	fd_set fds;
	int n = 0;
	FD_ZERO( &fds );
	FD_SET( STDIN_FILENO, &fds );
	n = select( STDIN_FILENO + 1, &fds, NULL, NULL, NULL );
	if ( n <= 0 ) // nic albo b³±d
		return -1;
	return goc_getchar(); // jest gotowy znak
}

