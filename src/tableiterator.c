#include "global-inc.h"
#include "tableiterator.h"
#ifndef _DEBUG
#	include <string.h>
#endif

static int tableIteratorHasNext(GOC_Iterator *iterator)
{
	GOC_TableIterator* titerator = (GOC_TableIterator*)iterator;
	if ( titerator->nPtr > titerator->pos )
		return 1;
	return 0;
}

static void* tableIteratorNext(GOC_Iterator *iterator)
{
	GOC_TableIterator* titerator = (GOC_TableIterator*)iterator;
	if ( titerator->nPtr > titerator->pos )
		return titerator->pPtr[titerator->pos++];
	return NULL;
}

GOC_Iterator* goc_tableIteratorAlloc(void *pPtr, int nPtr)
{
	GOC_TableIterator *tmp = malloc(sizeof(GOC_TableIterator));
	memset(tmp, 0, sizeof(GOC_TableIterator));
	tmp->hasNext = tableIteratorHasNext;
	tmp->next = tableIteratorNext;
	tmp->pPtr = (void**)pPtr;
	tmp->nPtr = nPtr;
	return (GOC_Iterator*)tmp;
}

GOC_Iterator* goc_tableIteratorFree(GOC_Iterator *tIterator)
{
	free( tIterator );
	return NULL;
}
