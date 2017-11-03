#include <okienkoc/okienkoc.h>
#include <tools/screen.h>

int main()
{
	GOC_StMessage wiesc;
	goc_elementCreate(GOC_ELEMENT_EDIT, 5, 5, 10, 1, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
