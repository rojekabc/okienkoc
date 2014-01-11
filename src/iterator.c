#include "iterator.h"

int goc_iteratorHasNext(GOC_Iterator *iterator)
{
	if ( iterator->hasNext )
		return iterator->hasNext(iterator);
	return 0;
}

void *goc_iteratorNext(GOC_Iterator *iterator)
{
	if ( iterator->next )
		return iterator->next(iterator);
	return 0;
}
