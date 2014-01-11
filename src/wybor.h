#ifndef _GOC_CHOICE_H_
#define _GOC_CHOICE_H_

#include "element.h"

extern const char* GOC_ELEMENT_CHOICE;

#define GOC_STRUCT_CHOICE \
	GOC_STRUCT_ELEMENT; \
	GOC_BOOL stan; \
	char typ[4]; \
	GOC_COLOR kolorPoleAktywny; \
	GOC_COLOR kolorPoleNieaktywny; \
	GOC_COLOR kolorOdpAktywny; \
	GOC_COLOR kolorOdpNieaktywny; \
	GOC_COLOR kolorKlamryAktywny; \
	GOC_COLOR kolorKlamryNieaktywny; \
	GOC_COLOR kolorKlamry; \
	GOC_COLOR kolorOdp; \
	char *tekst

typedef struct GOC_StChoice
{
	GOC_STRUCT_CHOICE;
} GOC_StChoice;

#define GOC_TYPE_CHOICEROUND "()"
#define GOC_TYPE_CHOICEBAR "[]"
#define GOC_TYPE_CHOICEX "x "
#define GOC_TYPE_CHOICEASTERIX "* "
#define GOC_TYPE_CHOICEPLUSMINUS "+-"
#define GOC_TYPE_CHOICEV "v "

int goc_choiceListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf);
int goc_choiceSetText(GOC_HANDLER uchwyt, const char *tekst);
#define goc_choiceSetState(uchwyt, _stan) ((GOC_StChoice*)uchwyt)->stan=_stan
#define goc_choiceGetState(uchwyt) ((GOC_StChoice*)uchwyt)->stan

#endif // ifndef _GOC_CHOICE_H_
