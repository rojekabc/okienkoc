#ifndef _GOC_GROUP_H_
#define _GOC_GROUP_H_

#include "element.h"

extern const char* GOC_ELEMENT_GROUP;

#define GOC_STRUCT_GROUP \
	GOC_STRUCT_ELEMENT

typedef struct GOC_StGroup
{
	GOC_STRUCT_GROUP;
} GOC_StGroup;

int goc_groupListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf);

#endif // ifndef _GOC_FORM_H_
