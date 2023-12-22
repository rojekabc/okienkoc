#ifndef _GOC_SCREEN_H_
#define _GOC_SCREEN_H_

#define goc_gotoxy(x, y) printf("\033[%d;%df", y, x)
#define goc_clearscreen() printf("\033[2J")
#define goc_saveXY() printf("\033[s")
#define goc_loadXY() printf("\033[u")
void goc_textcolor(int color);
void goc_textallcolor(unsigned char color);
#define goc_scroll(y1, y2) printf("\033[%d;%dr", y1, y2)
#define goc_delline() printf("\033[K")
void goc_getnum(long *c, long old, long min, long max);
#define goc_clearline() printf("\033[K")
#define goc_moveright(n) printf("\033[%dC", n)
#define goc_moveleft(n) printf("\033[%dD", n)
#define goc_movedown(n) printf("\033[%dB", n)
#define goc_moveup(n) printf("\033[%dA", n)
int goc_stringToColor(const char* str);
#define goc_hideCursor() printf("\033[?25l")
#define goc_showCursor() printf("\033[?25h")

#define GOC_BLACK           0x00
#define GOC_RED             0x01
#define GOC_GREEN           0x02
#define GOC_YELLOW          0x03
#define GOC_BLUE            0x04
#define GOC_MAGNETA         0x05
#define GOC_CYAN            0x06
#define GOC_WHITE           0x07

#define GOC_BBLACK           0x00
#define GOC_BRED             0x08
#define GOC_BGREEN           0x10
#define GOC_BYELLOW          0x18
#define GOC_BBLUE            0x20
#define GOC_BMAGNETA         0x28
#define GOC_BCYAN            0x30
#define GOC_BWHITE           0x38

#define GOC_FNORMAL          0x00
#define GOC_FBLINK           0x80
#define GOC_FBOLD            0x40

#define GOC_STRING_BLACK	"\033[0;30m"
#define GOC_STRING_RED		"\033[0;31m"
#define GOC_STRING_GREEN	"\033[0;32m"
#define GOC_STRING_YELLOW	"\033[0;33m"
#define GOC_STRING_BLUE		"\033[0;34m"
#define GOC_STRING_MAGNETA	"\033[0;35m"
#define GOC_STRING_CYAN		"\033[0;36m"
#define GOC_STRING_WHITE	"\033[0;37m"

#define GOC_STRING_BBLACK	"\033[1;30m"
#define GOC_STRING_BRED		"\033[1;31m"
#define GOC_STRING_BGREEN	"\033[1;32m"
#define GOC_STRING_BYELLOW	"\033[1;33m"
#define GOC_STRING_BBLUE	"\033[1;34m"
#define GOC_STRING_BMAGNETA	"\033[1;35m"
#define GOC_STRING_BCYAN	"\033[1;36m"
#define GOC_STRING_BWHITE	"\033[1;37m"

#endif
