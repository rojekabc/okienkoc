#include <tools/tableiterator.h>
#include <tools/tablica.h>
#include <stdio.h>

int main()
{
	GOC_Iterator *iterator = NULL;
	int *pTable = NULL;
	_GOC_TABEL_SIZETYPE_ nTable = 0;

	pTable = goc_tableAdd(pTable, &nTable, sizeof(int));
	pTable[nTable-1] = 1;
	pTable = goc_tableAdd(pTable, &nTable, sizeof(int));
	pTable[nTable-1] = 15;
	pTable = goc_tableAdd(pTable, &nTable, sizeof(int));
	pTable[nTable-1] = 21;
	pTable = goc_tableAdd(pTable, &nTable, sizeof(int));
	pTable[nTable-1] = 148;
	pTable = goc_tableAdd(pTable, &nTable, sizeof(int));
	pTable[nTable-1] = 419;
	pTable = goc_tableAdd(pTable, &nTable, sizeof(int));
	pTable[nTable-1] = 1439;

	iterator = goc_tableIteratorAlloc( (void*)pTable, nTable );
	while ( goc_iteratorHasNext( iterator ) )
	{
		int e = (int)goc_iteratorNext( iterator );
		printf("%d\n", e);
	}
	goc_tableIteratorFree( iterator );
	return 0;
}
