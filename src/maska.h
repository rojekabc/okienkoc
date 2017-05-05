#ifndef _GOC_MASK_H_
#define _GOC_MASK_H_

#include "element.h"
#include "nbitpola.h"
#include "tablica.h"
#include "znak.h"

extern const char* GOC_ELEMENT_MASK;
/*
 * Jako pBuf int dlugosc
 * Jak nBuf int wysokosc
 */
extern const char* GOC_MSG_MAPSETSIZE;
/*
 * Jako pBuf GOC_StPoint* punkt do pobrania
 * Jako nBuf GOC_StChar* wskazanie na znak do wypelnienia
 */
extern const char* GOC_MSG_MAPGETCHARAT;
/*
 * w pBuf GOC_StChar*
 * w nBuf pozycja (lub -1, gdy tylko dodac)
 */
extern const char* GOC_MSG_MAPADDCHAR;
/*
 * w pBuf GOC_StPoint*
 * w nBuf value
 *
 */
extern const char* GOC_MSG_MAPSETPOINT;
/*
 * W pBuf GOC_StArea* wskazanie wycinka obszaru rysowanego z mapy
 * W nBuf GOC_StPoint* wskazanie punktu na screenie, w którym zacznie rysowanie
 */
extern const char* GOC_MSG_MAPPAINT;
/*
 * w pBuf GOC_StPoint*
 * w nBuf int* na zwracan± warto¶æ
 */
extern const char* GOC_MSG_MAPGETPOINT;
/*
 * Funkcja zwraca wartosc znaku, jaki ma byc rysowany
 * dla okreslonej wartosci
 *
 * W pBuf GOC_StChar* do uzupe³nienia warto¶ciami
 * W nBuf struktura sk³adowana GOC_StValuePoint* z punktem do rysowania
 */
extern const char* GOC_MSG_MAPGETCHAR;
/*
 * Zwolnienie punktu z zasobów mapy pozycyjnej.
 * Je¿eli punkt ten wi±¿e siê z jakimi¶ dodatkowymi
 * zasobami - nale¿y je zwolniæ. Sam punkt bêdzie usuwany
 * przez funkcjê wywo³uj±c±.
 *
 * W pBuf wskazanie na GOC_StValuePoint
 */
extern const char* GOC_MSG_POSMAPFREEPOINT;
/*
 * Zmiana po³o¿enia kursora w elemencie
 *
 * W pBuf wskazanie na GOC_StPoint
 */
extern const char* GOC_MSG_CURSORCHANGE;
/*
 * Fill structure with char code and color for selected area
 *
 * pBuf - pointer to GOC_StFillArea
 */
extern const char* GOC_MSG_MAPFILL;

// d,w - rzeczywista dlugosc i wysokosc
// xp yp - x,y poczatku rysowania na obszarze rzeczywistym
// xk, yk - x,y kursora na obszarze rzeczywistym
// nZnakow - liczebnosc znakow
// liczebnosc wartosci = nZnakow
// tKolorow, tZnakow - tablica kolorow i znakow
// xs, ys - rozmiar przestrzeni na jeden znak
#define GOC_STRUCT_MASK \
	GOC_STRUCT_ELEMENT; \
	unsigned int d; \
	unsigned int w; \
	int xp, yp; \
	unsigned char xs, ys; \
	GOC_StPoint kursor; \
	GOC_HANDLER *pMapa; \
	_GOC_TABEL_SIZETYPE_ nMapa

//	GOC_StNBitField *dane
	
typedef struct GOC_StMask
{
	GOC_STRUCT_MASK;
} GOC_StMask;

int goc_maskListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf);
int goc_maskSetRealArea(GOC_HANDLER uchwyt, int d, int w);
int goc_maskSet(GOC_HANDLER uchwyt, unsigned char x, unsigned char y, int val);
int goc_maskGet(GOC_HANDLER uchwyt, int x, int y);

#define goc_maskGetUnder(uchwyt) \
	goc_nbitFieldGet( ((GOC_StMask*)uchwyt)->dane, \
			((GOC_StMask*)uchwyt)->xp+((GOC_StMask*)uchwyt)->kursor.x + \
			((GOC_StMask*)uchwyt)->d*(((GOC_StMask*)uchwyt)->yp + \
			((GOC_StMask*)uchwyt)->kursor.y) )
#define goc_maskSetUnder(uchwyt, val) \
	goc_nbitFieldSet( ((GOC_StMask*)uchwyt)->dane, \
			((GOC_StMask*)uchwyt)->xp + ((GOC_StMask*)uchwyt)->kurso.x + \
			((GOC_StMask*)uchwyt)->d*(((GOC_StMask*)uchwyt)->yp + \
			((GOC_StMask*)uchwyt)->kursor.y), val )
int goc_maskPaintPoint(GOC_HANDLER uchwyt, int x, int y);
int goc_maskSetValue(GOC_HANDLER uchwyt, short pozycja, char znak, GOC_COLOR color);
#define goc_maskGetBuffer(uchwyt) ((GOC_StMask*)uchwyt)->dane->dane
// w bajtach
#define goc_maskGetBufferSize(uchwyt) (((GOC_StMask*)uchwyt)->dane->lbnw * \
		((GOC_StMask*)uchwyt)->dane->ld-1)/8+1
void goc_maskRemMaps(GOC_HANDLER uchwyt);
int goc_maskAddMap(GOC_HANDLER uchwyt, GOC_HANDLER mapa);
#define goc_maskCharSpace(uchwyt, xspace, yspace) \
	((GOC_StMask*)uchwyt)->xs = xspace;\
	((GOC_StMask*)uchwyt)->ys = yspace;

#endif // ifndef _GOC_MASK_H_
