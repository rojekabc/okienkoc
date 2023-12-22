#define GOC_PRINTINFO
#include <tools/log.h>
#include <tools/fileistream.h>
#include <tools/properties.h>
#ifdef __MACH__
#	include <stdlib.h>
#else
#	include <malloc.h>
#endif

int main(int argc, char **argv)
{
	GOC_IStream* is = NULL;
	GOC_Iterator* it = NULL;
	GOC_Properties* props = NULL;
	if ( argc != 2 )
	{
		GOC_INFO("Podaj jako argument plik typu properties");
		return -1;
	}
	is = goc_fileIStreamOpen(argv[1]);
	props = goc_propertiesAlloc();
	props = goc_propertiesLoad(props, is);

	it = goc_propertiesListCategories(props);
	while ( goc_iteratorHasNext(it) )
	{
		GOC_Category* cat = (GOC_Category*)goc_iteratorNext(it);
		printf("GOC_Category: %s\n", cat->name);
		GOC_Iterator* pit = goc_propertiesListCategory(props, cat->name);
		while ( goc_iteratorHasNext(pit) )
		{
			GOC_Property* p = (GOC_Property*)goc_iteratorNext(pit);
			printf("\t%s = %s\n", p->name, p->value);
		}
		free(pit);
		printf("\n");
	}
	free(it);


	props = goc_propertiesFree(props);
	goc_isClose(is);
	return 0;
}
