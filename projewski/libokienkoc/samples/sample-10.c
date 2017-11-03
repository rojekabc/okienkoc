#include <okienkoc/okienkoc.h>
#include <tools/screen.h>

void maskaPrzypisz(GOC_HANDLER maska)
{
	goc_maskSet(maska, 0, 0, 1);
	goc_maskSet(maska, 1, 1, 1);
	goc_maskSet(maska, 2, 2, 1);
	goc_maskSet(maska, 3, 3, 1);
	goc_maskSet(maska, 4, 4, 1);
}

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER maska;
	GOC_HANDLER maparaw = 0;

	maska = goc_elementCreate(GOC_ELEMENT_MASK, 1, 1, 50, 20, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_maskSetRealArea(maska, 100, 100);
	maparaw = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, 100, 100, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE | GOC_FBOLD, maska);
	goc_maskAddMap( maska, maparaw );
	goc_maskSetValue(maska, 0, '0', GOC_WHITE);
	goc_maskSetValue(maska, 1, '1', GOC_WHITE | GOC_FBOLD);
	maskaPrzypisz(maska);
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
