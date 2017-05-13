/*
 * Listowanie katalogu
 */
#include <okienkoc/okienkoc.h>
#include <stdio.h>

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER lista;
	lista = goc_elementCreate(GOC_ELEMENT_FILELIST, 1, 1, 40, 20,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_filelistSetFolder(lista, ".");
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
