#ifndef _GOC_PRECENT_H_
#define _GOC_PRECENT_H_

#include "element.h"

extern const char* GOC_ELEMENT_PRECENT;

#define GOC_STRUCT_PRECENT \
	GOC_STRUCT_ELEMENT; \
	char znakJasny; \
	char znakCiemny; \
	GOC_COLOR kolorJasny; \
	GOC_COLOR kolorCiemny; \
	GOC_COLOR kolorAktywny; \
	GOC_COLOR kolorNieaktywny; \
	GOC_COLOR kolorRamka; \
	int max; \
	int pozycja; \
	int min

typedef struct GOC_StPrecent
{
	GOC_STRUCT_PRECENT;
} GOC_StPrecent;

int goc_precentListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);
#define goc_precentPositionPlus(_uchwyt_) \
	if ( ((GOC_StPrecent*)_uchwyt_)->pozycja < ((GOC_StPrecent*)_uchwyt_)->max ) \
		(((GOC_StPrecent*)_uchwyt_)->pozycja)++;
#define goc_precentPositionMinus(_uchwyt_) \
	if ( ((GOC_StPrecent*)_uchwyt_)->pozycja > ((GOC_StPrecent*)_uchwyt_)->min ) \
		(((GOC_StPrecent*)_uchwyt_)->pozycja)--;
#define goc_precentSetPosition(_uchwyt_, _value_) \
	((GOC_StPrecent*)_uchwyt_)->pozycja = _value_;
#define goc_precentSetMin(_uchwyt_, _value_) \
	((GOC_StPrecent*)_uchwyt_)->min = _value_;
#define goc_precentSetMax(_uchwyt_, _value_) \
	((GOC_StPrecent*)_uchwyt_)->max = _value_;


#endif // ifndef _GOC_PRECENT_H_
