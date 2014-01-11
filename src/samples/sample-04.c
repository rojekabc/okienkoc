/*
 * W próbie tej pokazane s± trzy przyciski. W zale¿no¶ci od tego, który
 * zostanie naci¶niêty pojawia siê odpowiedni napis z informacj±, jaki color
 * zosta³ wci¶niêty.
 */
#include <okienkoc/okienkoc.h>
#include <stdio.h>

GOC_HANDLER przyciskGreen, przyciskRed, przyciskCyan;
GOC_HANDLER napis;

static int nasluch(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, unsigned int nBuf)
{
	if ( wiesc == GOC_MSG_ACTION )
	{
		if ( uchwyt == przyciskGreen )
			goc_labelSetText(napis, "Naci¶niêto Green", GOC_FALSE);
		else if ( uchwyt == przyciskRed )
			goc_labelSetText(napis, "Naci¶niêto Red", GOC_FALSE);
		else if ( uchwyt == przyciskCyan )
			goc_labelSetText(napis, "Naci¶niêto Cyan", GOC_FALSE);
	}
	return goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
}

int main()
{
	GOC_MSG wiesc;
	goc_systemSetListenerFunc(&nasluch);
	napis = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 1, 80, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
			GOC_WHITE | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_labelSetText(napis, "Nie naci¶niêto nic", GOC_FALSE);
	przyciskGreen = goc_elementCreate(GOC_ELEMENT_BUTTON, 5, 5, 10, 3, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED,
			GOC_GREEN | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_buttonSetText(przyciskGreen, "Ok");
	przyciskRed = goc_elementCreate(GOC_ELEMENT_BUTTON, 5, 9, 10, 3, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
			GOC_RED | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_buttonSetText(przyciskRed, "Ok");
	przyciskCyan = goc_elementCreate(GOC_ELEMENT_BUTTON, 5, 13, 10, 3, GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED | GOC_EFLAGA_RIGHT,
			GOC_CYAN | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_buttonSetText(przyciskCyan, "Ok");
	
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
