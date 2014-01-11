#include "global-inc.h"
#include "element.h"
#include "system.h"
#include "hotkey.h"
#include "term.h"
#include "conflog.h"
#include <stdio.h>

// Pobieranie polozenia uwzglêdnia równie¿ to,
// w jakim po³o¿eniu znajduje siê rodzic danego elementu
//
// Rozpatrywane s± równie¿ warto¶ci ujemne, które mówi± o
// miejscu zakotwiczenia elementu. Warto¶æ dodatnia, to
// góra oraz lewy bok. Warto¶æ ujemna to dó³ oraz prawy bok.
//
// Zakotwiczenie odbywa siê dla lewego górnego naroznika elementu
// dla warto¶ci dodatnich. Dla ujemnych zakotwiczany jest
// prawy dolny róg (a wiêc po³o¿enie okre¶la pozycjê najbli¿szej
// krawêdzi elementu w stosunku do zakotwiczanej krawêdzi rodzica)
// 
// UWAGA ! Polozenia s± liczone od 1 w górê ! Nie od 0 !
// St±d wymagane s± wyrównania
GOC_POSITION goc_elementGetScreenX(GOC_HANDLER u)
{
	GOC_POSITION x = 1;
	GOC_StElement *element = (GOC_StElement*)u;
	while ( element != GOC_HANDLER_SYSTEM )
	{
		if ( element->x > 0 )
		{
			x += element->x;
			x--;
		}
		else
		{
			if ( element->ojciec == GOC_HANDLER_SYSTEM )
				x += goc_screenGetWidth();
			else
				x += goc_elementGetWidth(element->ojciec);
			x += element->x;
//			x -= element->width;
			x++;
		}
		element = (GOC_StElement*)element->ojciec;
	}
	return x;
}

GOC_POSITION goc_elementGetScreenY(GOC_HANDLER u)
{
	GOC_POSITION y = 1;
	GOC_StElement *element = (GOC_StElement*)u;
	while ( element != GOC_HANDLER_SYSTEM )
	{
		if ( element->y > 0 )
		{
			y += element->y;
			y--;
		}
		else
		{
			if ( element->ojciec == GOC_HANDLER_SYSTEM )
				y += goc_screenGetHeight();
			else
				y += goc_elementGetHeight(element->ojciec);
			y += element->y;
//			y -= element->height;
			y++;
		}
		element = (GOC_StElement*)element->ojciec;
	}
	return y;
}

GOC_POSITION goc_elementGetWidth(GOC_HANDLER u)
{
	GOC_StElement *element = (GOC_StElement*)u;
	GOC_POSITION width = 0;
	// je¿eli zapytanie dotyczy okna systemu
	if ( u == GOC_HANDLER_SYSTEM )
		return goc_screenGetWidth();
	// wyznaczenie obszaru do dyspozycji elementu
	width = goc_elementGetWidth(element->ojciec);
	width -= element->x;
	width++; // korekta
	// okreslenie rozmiaru od punktu polozenia elementu
	if ( element->width > 0 )
	{
		// element wykracza poza obszar rodzica
		if ( width <= 0 )
			return 0;
		if ( width > element->width )
			return element->width;
		else
			return width;
	}
	// okre¶lenie rozmiaru od prawej krawedzi rodzica
	else
	{
		width += element->width;
		return width;
	}
}

GOC_POSITION goc_elementGetHeight(GOC_HANDLER u)
{
	GOC_StElement *element = (GOC_StElement*)u;
	GOC_POSITION height = 0;
	// je¿eli zapytanie dotyczy okna systemu
	if ( u == GOC_HANDLER_SYSTEM )
		return goc_screenGetHeight();
	// wyznaczenie obszaru do dyspozycji elementu
	height = goc_elementGetHeight(element->ojciec);
	height -= element->y;
	height++; // korekta na liczenie od 1
	// okreslenie rozmiaru od punktu polozenia elementu
	if ( element->height > 0 )
	{
		// element wykracza poza obszar rodzica
		if ( height <= 0 )
			return 0;
		if ( height > element->height )
			return element->height;
		else
			return height;
	}
	// okre¶lenie rozmiaru od prawej krawedzi rodzica
	else
	{
		height += element->height;
		return height;
	}
}

int goc_elementSetColor(GOC_HANDLER u, GOC_COLOR k)
{
	GOC_StElement *element = (GOC_StElement*)u;
	element->color = k;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}
int goc_elementSetX(GOC_HANDLER u, GOC_POSITION x)
{
	GOC_StElement *element = (GOC_StElement*)u;
	((GOC_StElement*)u)->x = x;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}
int goc_elementSetY(GOC_HANDLER u, GOC_POSITION y)
{
	GOC_StElement *element = (GOC_StElement*)u;
	((GOC_StElement*)u)->y = y;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}
int goc_elementSetWidth(GOC_HANDLER u, GOC_POSITION d)
{
	GOC_StElement *element = (GOC_StElement*)u;
	((GOC_StElement*)u)->width = d;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}
int goc_elementSetHeight(GOC_HANDLER u, GOC_POSITION w)
{
	GOC_StElement *element = (GOC_StElement*)u;
	((GOC_StElement*)u)->height = w;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

GOC_FLAGS goc_elementGetFlag(GOC_HANDLER u)
{
	return ((GOC_StElement*)u)->flag;
}

int goc_elementSetFlag(GOC_HANDLER u, GOC_FLAGS f)
{
	GOC_StElement *element = (GOC_StElement*)u;
	((GOC_StElement*)u)->flag = f;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

int goc_elementOrFlag(GOC_HANDLER u, GOC_FLAGS f)
{
	GOC_StElement *element = (GOC_StElement*)u;
	element->flag |= f;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

int goc_elementIsFlag(GOC_HANDLER u, GOC_FLAGS f)
{
	GOC_StElement *element = (GOC_StElement*)u;
	element->flag &= ~f;
	if ( element->flag & GOC_EFLAGA_SHOWN )
		goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

GOC_COLOR goc_elementGetColor(GOC_HANDLER u)
{
	return ((GOC_StElement*)u)->color;
}

// Obiekty, ktore rozszerzaja ten element powinny go wywo³ywaæ
// aby wykona³ on usuwanie swoich ¶mieci
int goc_elementDestroy(GOC_HANDLER u)
{
	GOC_StElement *element = (GOC_StElement*)u;
	_GOC_TABEL_SIZETYPE_ i = element->nFunkcja;
	element->pFunkcja = goc_tableClear(
		element->pFunkcja, &(element->nFunkcja));
	element->pFunId = goc_tableClear(element->pFunId, &i);
	goc_hkClear(u, GOC_HKFLAG_SYSTEM | GOC_HKFLAG_USER);
	free((void*)u);
	return GOC_ERR_OK;
}

int goc_elementSetFunc(GOC_HANDLER uchwyt, const char *funId, void *fun)
{
	GOC_StElement *element = (GOC_StElement*)uchwyt;
	element->pFunkcja = goc_tableAdd(element->pFunkcja,
		&(element->nFunkcja), sizeof(void*));
	element->nFunkcja--;
	element->pFunkcja[element->nFunkcja] = fun;
	element->pFunId = goc_tableAdd(element->pFunId,
		&(element->nFunkcja), sizeof(const char*));
	element->pFunId[element->nFunkcja-1] = funId;
	return GOC_ERR_OK;
}

void *goc_elementGetFunc(GOC_HANDLER uchwyt, const char *funkcjaId)
{
	GOC_StElement *element = (GOC_StElement*)uchwyt;
	int i;
	for ( i=0; i<element->nFunkcja; i++ )
	{
		if ( element->pFunId[i] == funkcjaId )
			return element->pFunkcja[i];
	}
	return NULL;
}
