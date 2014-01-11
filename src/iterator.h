#ifndef _GOC_ITERATOR_H_
#	define _GOC_ITERATOR_H_

#	define GOC_STRUCT_ITERATOR \
		int (*hasNext)(struct GOC_Iterator*); \
		void* (*next)(struct GOC_Iterator*)


typedef struct GOC_Iterator {
	GOC_STRUCT_ITERATOR;
} GOC_Iterator;

int goc_iteratorHasNext(GOC_Iterator *);
void *goc_iteratorNext(GOC_Iterator *);

#endif // ifndef _GOC_ITERATOR_H_
