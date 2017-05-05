#ifndef _GOC_MAPPOS_H_
#define _GOC_MAPPOS_H_

#include "element.h"
#include "znak.h"

extern const char* GOC_ELEMENT_POSMAP;

#define GOC_STRUCT_VALUEPUNKT \
	GOC_STRUCT_POINT; \
	int value


typedef struct GOC_StValuePoint {
	GOC_STRUCT_VALUEPUNKT;
} GOC_StValuePoint;

#define GOC_STRUCT_MAPPOS \
	GOC_STRUCT_ELEMENT; \
	GOC_StValuePoint **pPunkt; \
	_GOC_TABEL_SIZETYPE_ nPunkt; \
	GOC_StChar *pZnak; \
	_GOC_TABEL_SIZETYPE_ nZnak

typedef struct GOC_StMapPos
{
	GOC_STRUCT_MAPPOS;
} GOC_StMapPos;

int goc_mapaposListener(
	GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf);
int goc_mapaposAddChar(GOC_HANDLER uchwyt, char znak, GOC_COLOR color, int pozycja);
int goc_mapaposRemPoint(GOC_HANDLER uchwyt, int x, int y);
int goc_mapposAddPoint(GOC_HANDLER uchwyt, int x, int y);
int goc_mapaposSetPoint(GOC_HANDLER uchwyt, int x, int y, int value);
int goc_mapaposChgPoint(GOC_HANDLER uchwyt, int x, int y, int value);
int goc_mapaposAddPoint(GOC_HANDLER uchwyt, GOC_StValuePoint *vpunkt);
GOC_StValuePoint* goc_mapaposReadPoint(GOC_HANDLER uchwyt, int x, int y);

#endif
