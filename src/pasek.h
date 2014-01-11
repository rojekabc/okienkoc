#ifndef _GOC_BAR_H_
#define _GOC_BAR_H_

#include "element.h"
#include "tablica.h"

extern const char* GOC_ELEMENT_BAR;

#define GOC_STRUCT_BAR \
	GOC_STRUCT_ELEMENT; \
	char **tekst; \
	_GOC_TABEL_SIZETYPE_ nText; \
	short start; \
	int kursor; \
	GOC_COLOR kolorZaznaczony; \
	GOC_COLOR kolorAktywny; \
	GOC_COLOR kolorNieaktywny; \
	GOC_POSITION rozmiar

#define GOC_BAR_COUNTEDSIZE 255

typedef struct GOC_StBar
{
	GOC_STRUCT_BAR;
} GOC_StBar;

int goc_barListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf);
int goc_barAddText(GOC_HANDLER uchwyt, const char *tekst);

#endif // ifndef _GOC_BAR_H_
