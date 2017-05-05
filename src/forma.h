#ifndef _GOC_FORM_H_
#define _GOC_FORM_H_

#include "element.h"

extern const char* GOC_ELEMENT_FORM;

#define GOC_STRUCT_FORM \
	GOC_STRUCT_ELEMENT; \
	GOC_HANDLER focus;

typedef struct GOC_StForm
{
	GOC_STRUCT_FORM;
} GOC_StForm;

int goc_formListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf);
int goc_formShow(GOC_HANDLER uchwyt);
int goc_formHide(GOC_HANDLER uchwyt);
int goc_formShowLock(GOC_HANDLER uchwyt, int flags);

#endif // ifndef _GOC_FORM_H_
