#ifndef _GOC_EDIT_H_
#define _GOC_EDIT_H_

#include "element.h"
#include "napis.h"
#include "ramka.h"

extern const char* GOC_ELEMENT_EDIT;

#define GOC_STRUCT_EDIT \
	GOC_STRUCT_ELEMENT; \
	int kursor; \
	int start; \
	GOC_COLOR kolorRamkiAktywny; \
	GOC_COLOR kolorRamkiNieaktywny; \
	GOC_COLOR kolorPolaAktywny; \
	GOC_COLOR kolorPolaNieaktywny; \
	GOC_COLOR kolorPolaZaznaczony; \
	GOC_COLOR kolorRamki; \
	char *tekst



typedef struct GOC_StEdit
{
	GOC_STRUCT_EDIT;
} GOC_StEdit;

int goc_editListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf);
int goc_editSetText( GOC_HANDLER u, const char *tekst );
const char *goc_editGetText( GOC_HANDLER u );

#endif // ifndef _GOC_EDIT_H_
