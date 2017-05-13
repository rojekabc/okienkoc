/*
 * Listowanie katalogu
 */
#include <okienkoc/okienkoc.h>
#include <stdio.h>

int main()
{
	GOC_StMessage wiesc;
	goc_elementCreate(GOC_ELEMENT_SLIDEBAR, 5, 5, 10, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_elementCreate(GOC_ELEMENT_SLIDEBAR, 4, 6, 1, 10,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
