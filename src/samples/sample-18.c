#include <okienkoc/okienkoc.h>
#include <stdio.h>
#include <okienkoc/random.h>

/**
 * Testowanie ró¿nych mask mapowych w po³±czeniu na jedn± maskê
 * docelow±
 */

void maskaPrzypisz(GOC_HANDLER maska, int n)
{
	int i;
	int x, y;
	for (i=0; i<n; i++)
	{
		x = goc_random(10);
		y = goc_random(10);
		goc_maskSet(maska, x, y, 1);
	}
}

// G³ówna czê¶æ programu
int main()
{
	GOC_MSG wiesc;
	GOC_HANDLER maska = 0;
	GOC_HANDLER mapa1 = 0;
	GOC_HANDLER mapa2 = 0;
	GOC_HANDLER mapa3 = 0;

	srand(time(NULL));
	maska = goc_elementCreate(GOC_ELEMENT_MASK, 1, 1, 10, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_maskCharSpace(maska, 2, 2);
	goc_maskSetRealArea(maska, 10, 10);
	mapa1 = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, 10, 10,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE | GOC_FBOLD, maska);
	mapa2 = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, 10, 10,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE | GOC_FBOLD, maska);
	mapa3 = goc_elementCreate(GOC_ELEMENT_POSMAP, 1, 1, 10, 10,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE | GOC_FBOLD, maska);
	goc_maskAddMap(maska, mapa1);
	goc_maskSetValue(maska, 0, ' ', GOC_WHITE);
	goc_maskSetValue(maska, 1, 'T', GOC_GREEN);
	maskaPrzypisz(maska, 80);
	goc_maskAddMap(maska, mapa2);
	goc_maskSetValue(maska, 0, 0, GOC_WHITE);
	goc_maskSetValue(maska, 1, 'o', GOC_RED);
	maskaPrzypisz(maska, 20);
	goc_maskAddMap(maska, mapa3);
	goc_maskSetValue(maska, 0, 0, GOC_WHITE);
	goc_maskSetValue(maska, 1, '<', GOC_WHITE);
	maskaPrzypisz(maska, 3);

	maska = goc_elementCreate(GOC_ELEMENT_MASK, 24, 1, 10, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_maskSetRealArea(maska, 10, 10);
	goc_maskAddMap(maska, mapa1);

	maska = goc_elementCreate(GOC_ELEMENT_MASK, 36, 1, 10, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_maskSetRealArea(maska, 10, 10);
	goc_maskAddMap(maska, mapa2);

	maska = goc_elementCreate(GOC_ELEMENT_MASK, 48, 1, 10, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_maskSetRealArea(maska, 10, 10);
	goc_maskAddMap(maska, mapa3);

	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
