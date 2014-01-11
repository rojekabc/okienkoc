#ifndef _GOC_BASEELEMENT_H_
#define _GOC_BASEELEMENT_H_

#include "global.h"

#define GOC_STRUCT_BASEELEMENT \
	GOC_IDENTIFIER id; \
	const char **pFunId; \
	void **pFunkcja; \
	_GOC_TABEL_SIZETYPE_ nFunkcja

#endif // ifndef _BASELEMENT_H_
