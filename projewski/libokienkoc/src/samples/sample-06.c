#include <okienkoc/okienkoc.h>

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER wybor;
	wybor = goc_elementCreate(GOC_ELEMENT_CHOICE, 1, 1, 25, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_choiceSetText(wybor, "Opcja 1");
	wybor = goc_elementCreate(GOC_ELEMENT_CHOICE, 1, 2, 25, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_choiceSetText(wybor, "Opcja 2");
	wybor = goc_elementCreate(GOC_ELEMENT_CHOICE, 1, 3, 25, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_choiceSetText(wybor, "Opcja 3");
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
