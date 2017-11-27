#ifndef _GOC_ARRAY_H_
#	define _GOC_ARRAY_H_
#	include "tablica.h"

typedef struct GOC_Array {
	void** pElement;
	_GOC_TABEL_SIZETYPE_ nElement;
	void (*freeElement)(void*);
} GOC_Array;

GOC_Array* goc_arrayAlloc();
GOC_Array* goc_arrayFree(GOC_Array* array);
GOC_Array* goc_arrayAdd(GOC_Array* array, void* pElement);
GOC_Array* goc_arrayRemove(GOC_Array* array, void* pElement);
GOC_Array* goc_arrayRemoveAt(GOC_Array* array, int pos);
GOC_Array* goc_arrayClear(GOC_Array* array);

#endif // ifndef _GOC_ARRAY_H_
