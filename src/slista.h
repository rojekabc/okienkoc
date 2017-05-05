#ifndef _SLISTA_H
#define _GOC_SELLIST_H_

#include "lista.h"
#include "nbitpola.h"

extern const char* GOC_ELEMENT_SELLIST;

#define GOC_STRUCT_SELLIST \
	GOC_STRUCT_LIST; \
	GOC_COLOR kolorPoleZaznaczony; \
	GOC_COLOR kolorPoleKursorZaznaczony; \
	GOC_StNBitField *pZaznaczony;

typedef struct GOC_StSelList
{
	GOC_STRUCT_SELLIST;
} GOC_StSelList;

int slistaUstKolorZaznaczony(GOC_HANDLER uchwyt);
int slistaUstKolorKursorZaznaczony(GOC_HANDLER uchwyt);
GOC_COLOR slistaDajKolorZaznaczony(GOC_HANDLER uchwyt);
GOC_COLOR slistaDajKolorKursorZaznaczony(GOC_HANDLER uchwyt);
int slistaDajZaznaczonyLiczba(GOC_HANDLER uchwyt);
int goc_sellistGetSelectPos(GOC_HANDLER uchwyt, int numer);
char *slistaDajZaznaczony(GOC_HANDLER uchwyt, int numer);
int goc_sellistListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf);
int goc_sellistSelect(GOC_HANDLER uchwyt, int pos);
int goc_sellistUnselect(GOC_HANDLER uchwyt, int pos);

#endif // ifndef _GOC_SELLIST_H_
