#include <okienkoc/okienkoc.h>

int main()
{
	GOC_MSG wiesc;
	GOC_HANDLER napis;
	napis = goc_elementCreate(GOC_ELEMENT_EDIT, 5, 5, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}