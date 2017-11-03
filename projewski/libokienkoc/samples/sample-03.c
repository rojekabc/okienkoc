/*
 * W próbie tej zostan± umieszczone trzy elementy pozwalaj±ce na wpisywanie
 * informacji.
 */
#include <okienkoc/okienkoc.h>
#include <tools/screen.h>

int main()
{
	GOC_StMessage wiesc;
	goc_elementCreate(GOC_ELEMENT_EDIT, 5, 5, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	goc_elementCreate(GOC_ELEMENT_EDIT, 5, 7, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	goc_elementCreate(GOC_ELEMENT_EDIT, 5, 9, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
