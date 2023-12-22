#include <stdio.h>
#include <stdlib.h>

#include "global-inc.h"
#include "screen.h"
#include "mystr.h"

void goc_textcolor(int color) {
    /* a + b */
    /* a - co ustawiasz b - jaki color */
    printf("\033[%dm", color);
}

void goc_textallcolor(unsigned char color) {
    printf("\033[");
    if (color & GOC_FBLINK)
	printf("5;");
    if (color & GOC_FBOLD)
	printf("1;");
    if (!(color & 0xC0))
	printf("0;");
    printf("%d;%dm", 30+(color&7), 40+((color>>3)&7));
}

void goc_getnum(long *c, long old, long min, long max)
{
    char buf[20];
    goc_saveXY();
    do
    {
        goc_loadXY();
	printf("                   ");
	goc_loadXY();
	fgets(buf, 20, stdin);
        if (buf[0]=='\n') *c = old;
        else *c = atol(buf);
    } while (*c<min || *c>max);
}

int goc_stringToColor(const char* str)
{
	int color = GOC_BLACK;
	if ( goc_stringEndsWithCase(str, "black") )
		color = GOC_BLACK;
	if ( goc_stringEndsWithCase(str, "red") )
		color = GOC_RED;
	if ( goc_stringEndsWithCase(str, "green") )
		color = GOC_GREEN;
	if ( goc_stringEndsWithCase(str, "yellow") )
		color = GOC_YELLOW;
	if ( goc_stringEndsWithCase(str, "blue") )
		color = GOC_BLUE;
	if ( goc_stringEndsWithCase(str, "magneta") )
		color = GOC_MAGNETA;
	if ( goc_stringEndsWithCase(str, "cyan") )
		color = GOC_CYAN;
	if ( goc_stringEndsWithCase(str, "white") )
		color = GOC_WHITE;
	if ( goc_stringStartsWithCase(str, "bold") )
		color |= GOC_FBOLD;
	if ( goc_stringStartsWithCase(str, "blink") )
		color |= GOC_FBLINK;
	return color;
}
