#include <okienkoc/okienkoc.h>

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER pasek;
	pasek = goc_elementCreate(GOC_ELEMENT_BAR, 1, 1, 40, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_RED, GOC_HANDLER_SYSTEM );
	goc_barAddText(pasek, "Punkt A");
	goc_barAddText(pasek, "Punkt B");
	goc_barAddText(pasek, "Punkt C");
	pasek = goc_elementCreate(GOC_ELEMENT_BAR, 1, 2, 40, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	goc_barAddText(pasek, "Punkt A");
	goc_barAddText(pasek, "Punkt B");
	goc_barAddText(pasek, "Punkt C");
	pasek = goc_elementCreate(GOC_ELEMENT_BAR, 1, 3, 40, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_CYAN, GOC_HANDLER_SYSTEM );
	goc_barAddText(pasek, "Punkt A");
	goc_barAddText(pasek, "Punkt B");
	goc_barAddText(pasek, "Punkt C");

	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
