/*
 * W pr�bie tej zostan� umieszczone trzy elementy pozwalaj�ce na wpisywanie
 * informacji.
 */
#include <okienkoc/okienkoc.h>

int main()
{
	GOC_MSG wiesc;
	GOC_HANDLER napis;
	napis = goc_elementCreate(GOC_ELEMENT_EDIT, 5, 5, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	napis = goc_elementCreate(GOC_ELEMENT_EDIT, 5, 7, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	napis = goc_elementCreate(GOC_ELEMENT_EDIT, 5, 9, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}