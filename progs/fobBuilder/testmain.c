#include "test.h"
#include <okienkoc/log.h>

int main(int argc, char **argv)
{
	if ( argc == 2 )
	{
		const char *filename = argv[1];
		xmlDoc *doc = NULL;
		xmlNode *root = NULL;
		// zaladowanie pliku
		if ( !filename )
		{
			GOC_ERROR("Podano wartosn NULL");
			return -1;
		}
		if ( !goc_isFileExists(filename) )
		{
			GOC_BERROR("Plik [%s] nie istnieje", filename);
			return -1;
		}

		doc = xmlReadFile( filename, NULL, 0 );
		if ( !doc )
		{
			GOC_BERROR("Nieudane parsowanie pliku [%s]", filename);
			return -1;
		}
		root = xmlDocGetRootElement( doc );
		if ( !root )
		{
			GOC_ERROR("Nieudane pozyskanie galezi root");
			return -1;
		}


		{
			StKlasa* k = fobDeserialize( (fobElement*)root );
			fobSerialize( (fobElement*)k, stdout );
		}
	}
	else
	{
		StKlasa* k = fobAlloc(cn_Klasa);
		k->name = goc_stringCopy(k->name, "Nazwa");
		k->plist = goc_tableAdd(k->plist, &k->nlist, sizeof(char*));
		k->plist[k->nlist-1] = goc_stringCopy(NULL, "alfa");
		k->plist = goc_tableAdd(k->plist, &k->nlist, sizeof(char*));
		k->plist[k->nlist-1] = goc_stringCopy(NULL, "beta");
		fobSerialize( (fobElement*)k, stdout );
	}
	return 0;
}
