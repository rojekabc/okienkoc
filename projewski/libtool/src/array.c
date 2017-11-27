#include "array.h"

#include <malloc.h>
#include <string.h>

static void internalClear(GOC_Array* array) {
	if ( array->freeElement ) {
		GOC_STABLEFREE(array, Element, array->freeElement);
	}
}

static void internalRemove(GOC_Array* array, int pos) {
	array->pElement = goc_tableRemove(
		array->pElement, &(array->nElement),
		sizeof(void*), pos);
}

GOC_Array* goc_arrayAlloc() {
	GOC_Array* result = malloc(sizeof(struct GOC_Array));
	memset(result, 0, sizeof(struct GOC_Array));
	result->freeElement = &free;
	return result;
}

GOC_Array* goc_arrayFree(GOC_Array* array) {
	if ( array ) {
		internalClear( array );
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

GOC_Array* goc_arrayClear(GOC_Array* array) {
	if ( array ) {
		internalClear(array);
	}
	return array;
}
