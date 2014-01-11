#ifndef _GOC_TABLE_ITERATOR_H_
#	define _GOC_TABLE_ITERATOR_H_

#	include "tablica.h"
#	include "iterator.h"

#	define GOC_STRUCT_TABLEITERATOR \
		GOC_STRUCT_ITERATOR; \
		void **pPtr; \
		_GOC_TABEL_SIZETYPE_ nPtr; \
		int pos

typedef struct GOC_TableIterator {
	GOC_STRUCT_TABLEITERATOR;
} GOC_TableIterator;

GOC_Iterator* goc_tableIteratorAlloc(void *pPtr, int nPtr);
GOC_Iterator* goc_tableIteratorFree(GOC_Iterator *);
		
#endif // ifndef _GOC_TABLE_ITERATOR_H_
