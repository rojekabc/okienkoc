/*
 * W probie tej zostan± pokazane dwie listy. Jedna jest podzielona na dwie
 * kolumny. Obie posiadaj± te same elementy.
 */
#include <okienkoc/okienkoc.h>
#include <tools/screen.h>
#include <stdio.h>

void listaBuduj(GOC_HANDLER lista)
{
	goc_listAddText(lista, "alfa");
	goc_listAddText(lista, "beta");
	goc_listAddText(lista, "auto");
	goc_listAddText(lista, "blok");
	goc_listAddText(lista, "cesna");
	goc_listAddText(lista, "ciezar");
	goc_listAddText(lista, "drezno");
	goc_listAddText(lista, "dublin");
	goc_listAddText(lista, "ekran");
	goc_listAddText(lista, "ewolucja");
	goc_listAddText(lista, "fragment");
	goc_listAddText(lista, "fuzja");
	goc_listAddText(lista, "gra");
	goc_listAddText(lista, "gapik");
	goc_listAddText(lista, "hamak");
	goc_listAddText(lista, "hak");
	goc_listAddText(lista, "ig³a");
	goc_listAddText(lista, "iskra");
}

void listaBudujKolumna(GOC_HANDLER lista)
{
	goc_listAddText(lista, "alfa");
	goc_listSetTextToColumn(lista, "beta", 1);
	goc_listAddText(lista, "auto");
	goc_listSetTextToColumn(lista, "blok", 1);
	goc_listAddText(lista, "cesna");
	goc_listSetTextToColumn(lista, "ciezar", 1);
	goc_listAddText(lista, "drezno");
	goc_listSetTextToColumn(lista, "dublin", 1);
	goc_listAddText(lista, "ekran");
	goc_listSetTextToColumn(lista, "ewolucja", 1);
	goc_listAddText(lista, "fragment");
	goc_listSetTextToColumn(lista, "fuzja", 1);
	goc_listAddText(lista, "gra");
	goc_listSetTextToColumn(lista, "gapik", 1);
	goc_listAddText(lista, "hamak");
	goc_listSetTextToColumn(lista, "hak", 1);
	goc_listAddText(lista, "ig³a");
	goc_listSetTextToColumn(lista, "iskra", 1);
}

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER lista;
	GOC_HANDLER lista2;
	lista = goc_elementCreate(GOC_ELEMENT_LIST, 1, 1, 20, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_GREEN | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_listSetTitle(lista, "Jeden");
	listaBuduj(lista);
//	fprintf(stderr, "ROWS: %d\n", goc_listGetRowsAmmount(lista));
	lista2 = goc_elementCreate(GOC_ELEMENT_LIST, 21, 1, 20, 10, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_GREEN | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_listAddColumn(lista2, 10);
	goc_listAddColumn(lista2, 10);
	listaBudujKolumna(lista2);
	goc_listSetTitle(lista2, "Dwa");
//	goc_systemSendMsg(lista, GOC_MSG_PAINT, 0, 0);
//	goc_systemSendMsg(lista2, GOC_MSG_PAINT, 0, 0);
//	goc_systemClearArea(lista);
//	fflush(stdout);
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}
