#include "array.h"

#include <malloc.h>
#include <string.h>

static void internalRemove(GOC_Array* array, int pos) {
	array->pElement = goc_tableRemove(
		array->pElement, &(array->nElement),
		sizeof(void*), pos);
}

GOC_Array* goc_arrayAlloc() {
	struct GOC_Array* result = malloc(sizeof(struct GOC_Array));
	memset(result, 0, sizeof(struct GOC_Array));
	result->freeElement = &free;
	return result;
}

GOC_Array* goc_arrayFree(GOC_Array* array) {
	if ( array ) {
		goc_arrayClear( array );
		free( array );
		array = NULL;
	}
	return array;
}

GOC_Array* goc_arrayAdd(GOC_Array* array, void* pElement) {
	if ( array ) {
		array->pElement = goc_tableAdd(
			array->pElement, &(array->nElement),
			sizeof(void*));
		array->pElement[array->nElement-1] = pElement;
	}
	return array;
}

GOC_Array* goc_arrayRemove(GOC_Array* array, void* pElement) {
	if ( array ) {
		for ( int i=0; i<array->nElement; i++ ) {
			if ( array->pElement[i] == pElement ) {
				internalRemove(array, i);
				break;
			}
		}
	}
	return array;
}

GOC_Array* goc_arrayRemoveAt(GOC_Array* array, int pos) {
	if ( array ) {
		internalRemove(array, pos);
	}
	return array;
}

GOC_Array* goc_arrayInsert(GOC_Array* array, int pos, void* pElement) {
	if ( array && pos >=0 ) {
		array->pElement = goc_tableInsert(array->pElement, &array->nElement,
			sizeof(void*), pos);
		array->pElement[pos] = pElement;
	}
	return array;
}

GOC_Array* goc_arrayClear(GOC_Array* array) {
	if ( array ) {
		int i;
		if ( array->freeElement ) {
			for(i=0; i<array->nElement; i++) {
				if ( array->pElement ) {
					array->freeElement(array->pElement[i]);
				}
			}
		}
		array->pElement = goc_tableClear( array->pElement, &array->nElement );
	}
	return array;
}

