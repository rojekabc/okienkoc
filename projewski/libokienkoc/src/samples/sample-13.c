#include <okienkoc/okienkoc.h>
#include <stdio.h>

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER grupa1, grupa2, napis1, napis2;
	grupa1 = goc_elementCreate(GOC_ELEMENT_GROUP, 1, 1, 80, 25,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM );
	grupa2 = goc_elementCreate(GOC_ELEMENT_GROUP, 1, 1, 80, 25,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM );
	napis1 = goc_elementCreate(GOC_ELEMENT_LABEL, 5, 5, 10, 1,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_GREEN, grupa1 );
	napis2 = goc_elementCreate(GOC_ELEMENT_LABEL, 25, 5, 10, 1,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_WHITE, grupa2 );
	goc_elementCreate(GOC_ELEMENT_EDIT, 25, 6, 10, 1,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_GREEN, grupa1 );
	goc_elementCreate(GOC_ELEMENT_EDIT, 5, 6, 10, 1,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_WHITE, grupa2 );
	goc_elementCreate(GOC_ELEMENT_EDIT, 25, 7, 10, 1,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_GREEN, grupa1 );
	goc_elementCreate(GOC_ELEMENT_EDIT, 5, 7, 10, 1,
		GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED, GOC_WHITE, grupa2 );
	goc_labelAddLine(napis1, "Group 1");
	goc_labelAddLine(napis2, "Froup 2");
	//goc_systemClearGroupArea(grupa2);
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
