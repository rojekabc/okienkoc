#ifndef _GOC_MAPRAW_H_
#define _GOC_MAPRAW_H_

#include "element.h"
#include <tools/nbitpola.h>
#include "znak.h"

extern const char* GOC_ELEMENT_RAWMAP;

#define GOC_STRUCT_MAPRAW \
	GOC_STRUCT_ELEMENT; \
	GOC_StNBitField *dane; \
	GOC_StChar *pZnak; \
	_GOC_TABEL_SIZETYPE_ nZnak;

typedef struct GOC_StMapRaw
{
	GOC_STRUCT_MAPRAW;
} GOC_StMapRaw;

int goc_maparawListener(
	GOC_HANDLER uchwyt, GOC_StMessage* msg);
int goc_maparawAddChar(GOC_HANDLER uchwyt, char znak, GOC_COLOR color, int pozycja);
int goc_maparawSetBPV(GOC_HANDLER uchwyt, int bpv);
int goc_maparawAddChar(GOC_HANDLER uchwyt, char znak, GOC_COLOR color, int pozycja);
int goc_maparaw_SetSize(GOC_HANDLER uchwyt, int d, int w);
int goc_maparawSetPoint(GOC_HANDLER uchwyt, int x, int y, int value);
int goc_maparawGetPoint(GOC_HANDLER uchwyt, int x, int y);

#endif
