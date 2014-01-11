/*
 * W probie tej zostanie pokazana ramka.
 */
#include <okienkoc/okienkoc.h>
#define NUM 8

int main()
{
	GOC_MSG wiesc;
	GOC_HANDLER ramka;
	int i;
	for (i=0; i<NUM; i++)
	{
		ramka = goc_elementCreate(GOC_ELEMENT_FRAME, 1+i, 1+i, 0-i, 0-i, GOC_EFLAGA_PAINTED,
				GOC_GREEN | GOC_FBOLD, GOC_HANDLER_SYSTEM );
		goc_elementSetColor( ramka, i | GOC_FBOLD );
	}
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
