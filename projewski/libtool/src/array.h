#ifndef _GOC_ARRAY_H_
#	define _GOC_ARRAY_H_
#	include "tablica.h"

typedef struct GOC_Array {
	void** pElement;
	_GOC_TABEL_SIZETYPE_ nElement;
	void (*freeElement)(void*);
} GOC_Array;

#define array_add(_array_, _value_) _array_ = goc_arrayAdd(_array_, _value_)

GOC_Array* goc_arrayAlloc();
GOC_Array* goc_arrayFree(GOC_Array* array);
GOC_Array* goc_arrayAdd(GOC_Array* array, void* pElement);
GOC_Array* goc_arrayRemove(GOC_Array* array, void* pElement);
GOC_Array* goc_arrayRemoveAt(GOC_Array* array, int pos);
GOC_Array* goc_arrayClear(GOC_Array* array);
GOC_Array* goc_arrayInsert(GOC_Array* array, int pos, void* pElement);

#define goc_arrayGet( _array_, _pos_ ) (_array_)->pElement[_pos_]
#define goc_arraySize(_array_) (_array_)->nElement
#define goc_arrayIsEmpty(_array_) ((_array_)->pElement == NULL || (_array_)->nElement == 0)
#define goc_arraySet( _array_, _pos_, _element_ ) (_array_)->pElement[_pos_] = _element_

#endif // ifndef _GOC_ARRAY_H_
