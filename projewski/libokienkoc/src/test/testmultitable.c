#include <stdio.h>
#include <string.h>
#include <okienkoc/typetable.h>
#include <okienkoc/mystr.h>
#include <malloc.h>

static int intFree(int x) {return 0;}
static int intCopy(int x) {return x;}

GOC_TypeTable(GOC_StringTable, goc_stringtable, char*, goc_stringFree, strdup);
GOC_TypeTable(GOC_IntTable, goc_inttable, int, intFree, intCopy);

int main()
{
	int i;
	GOC_StringTable* tableOne;
	GOC_IntTable *tableTwo;

	tableOne = goc_stringtableAlloc();
	goc_stringtableAddCopy(tableOne, "Alfa");
	goc_stringtableAddCopy(tableOne, "Beta");
	goc_stringtableAddCopy(tableOne, "Gamma");
	goc_stringtableAdd(tableOne, strdup("IXI"));
	for (i=0; i<tableOne->nElement; i++)
		printf("%d = {%s}\n", i, tableOne->pElement[i]);
	tableOne = goc_stringtableFree(tableOne);

	tableTwo = goc_inttableAlloc();
	goc_inttableAdd(tableTwo, 1);
	goc_inttableAdd(tableTwo, 2);
	goc_inttableAdd(tableTwo, 3);
	goc_inttableAdd(tableTwo, 5);
	goc_inttableAdd(tableTwo, 8);
	goc_inttableAdd(tableTwo, 13);
	for (i=0; i<tableTwo->nElement; i++)
		printf("%d = {%d}\n", i, tableTwo->pElement[i]);
	tableTwo = goc_inttableFree(tableTwo);
	return 0;
}
