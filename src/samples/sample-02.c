/*
 * W próbie tej zostanie pokazany napis w trzech liniach. Kazda line jest
 * inaczej pozycjonowana.
 */
#include <okienkoc/okienkoc.h>

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER napis;
	napis = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 5, 0, 3, GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	goc_labelAddLine(napis, "Opisik");
	goc_labelAddLine(napis, "Opisik");
	goc_labelAddLine(napis, "Opisik");
	goc_labelSetFlag(napis, 0, GOC_EFLAGA_LEFT);
	goc_labelSetFlag(napis, 1, GOC_EFLAGA_CENTER);
	goc_labelSetFlag(napis, 2, GOC_EFLAGA_RIGHT);
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
