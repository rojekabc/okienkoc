/*
 * Próbra, która ma na celu sprawdzenie funkcjonowania podawania
 * warto¶ci ujemnych dla po³o¿enia elementów
 */
#include <okienkoc/okienkoc.h>
#include <stdio.h>
#ifndef _DEBUG
#	include <string.h>
#endif

int main()
{
	GOC_MSG wiesc;
	GOC_HANDLER napis;
	int i, w, h;
	char left[11] = "- Left A -";
	char right[11] = "- Right A-";
	char *napisStr = NULL;

	w = goc_screenGetWidth();
	h = goc_screenGetHeight();

	napisStr = malloc(w + 1);
	memset(napisStr, '-', w);
	napisStr[w] = 0;
	for (i=10; i<=w; i+=10)
		napisStr[i-1] = ((i/10)%10) + '0';

	napis = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 4, w, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	goc_labelAddLine(napis, napisStr);
	napis = goc_elementCreate(GOC_ELEMENT_LABEL, 1, -3, w, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	goc_labelAddLine(napis, napisStr);
	free(napisStr);

	h -= 10;

	for (i=0; i<h; i++)
	{
		napis = goc_elementCreate(GOC_ELEMENT_LABEL, 1 + i*5, 6 + i, 10, 1,
				GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
				GOC_RED, GOC_HANDLER_SYSTEM );
		goc_labelAddLine(napis, left);
		napis = goc_elementCreate(GOC_ELEMENT_LABEL, 0 - i*5, 6 + i, 10, 1,
				GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
				GOC_CYAN, GOC_HANDLER_SYSTEM );
		goc_labelAddLine(napis, right);
		left[7]++;
		right[8]++;
		if ( 11 + i*5 > w )
			break;
	}

	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
