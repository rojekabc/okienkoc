#include "global-inc.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include <stdio.h>
#include "lista.h"
#include "screen.h"
#include "mystr.h"
#include "tablica.h"
#include "napis.h"
#include "ramka.h"
#include "hotkey.h"
#include "system.h"

// element id
const char* GOC_ELEMENT_LIST = "GOC_List";

// message ids
const char* GOC_MSG_LISTCHANGE_ID = "GOC_MsgListChange";
const char* GOC_MSG_LISTSETCOLOR_ID = "GOC_MsgListSetColor";
const char* GOC_MSG_LISTADDTEXT_ID = "GOC_MsgListAddText";
const char* GOC_MSG_LISTCLEAR_ID = "GOC_MsgListClear";
const char* GOC_MSG_LISTSETEXTERNAL_ID = "GOC_MsgListSetExternal";
const char* GOC_MSG_LISTADDROW_ID = "GOC_MsgListAddRow";

int goc_listAddColumn(GOC_HANDLER u, GOC_POSITION width)
{
	GOC_POSITION free;
	GOC_COLUMN *kolumna;
	int i;
	GOC_StList *lista = (GOC_StList*)u;
	free = goc_elementGetWidth(u);
	free -= 2; /* ramka */
	for ( i=0; i<lista->nKolumna; i++ )
		free -= lista->pKolumna[i]->position;
	lista->pKolumna = goc_tableAdd( lista->pKolumna, &(lista->nKolumna), sizeof(GOC_COLUMN*) );
	kolumna = malloc(sizeof(GOC_COLUMN));
	memset(kolumna, 0, sizeof(GOC_COLUMN));
	lista->pKolumna[lista->nKolumna-1] = kolumna;
	kolumna->position = width <= free ? width : free;
	// Wyrowanie liczby wierszow w stosunku do istenijacych kolumn
	if ( lista->nKolumna > 1 )
	{
		kolumna->pText = goc_tableResize(kolumna->pText,
			&(kolumna->nText), sizeof(char*),
			lista->pKolumna[0]->nText);
		// Wyzerowanie wierszy
		if ( kolumna->pText )
			memset(kolumna->pText, 0,
				sizeof(char*)*kolumna->nText);
	}
//	lista->kursor = -1;
	return GOC_ERR_OK;
}

// ustawienie w kolumnie tekstu w pierwszej wolnej pozycji, jesli nie znajdzie
// zostanie dodana nowa kolumna
int goc_listSetColumnText(GOC_HANDLER u, const char *tekst, int kolumna)
{
	GOC_StList* lista = (GOC_StList*)u;
	GOC_COLUMN *k = NULL;
	int i;

	if ( lista->flag & GOC_LISTFLAG_EXTERNAL )
		return GOC_ERR_REFUSE;
	if ( kolumna >= lista->nKolumna )
		return GOC_ERR_REFUSE;
	k = lista->pKolumna[kolumna];
	for ( i=0; i<k->nText; i++ )
	{
		if ( k->pText[i] == NULL )
		{
			k->pText[i] = goc_stringCopy(k->pText[i], tekst);
			return GOC_ERR_OK;
		}
	}
	return goc_listAddColumnText(u, tekst, kolumna);
}

// Dodanie nowego wierszu i ustawienie tekstu dla podanej kolumny
int goc_listAddColumnText(GOC_HANDLER u, const char *tekst, int kolumna)
{
	GOC_LISTROW wiersz;
	int ret;
	
	memset( &wiersz, 0, sizeof(GOC_LISTROW));
	wiersz.pText = goc_tableResize( wiersz.pText, &(wiersz.nText),
		sizeof(char*), kolumna+1);
	wiersz.pText[kolumna] = tekst;
	wiersz.nRow = -1;
	GOC_MSG_LISTADDROW( msg, &wiersz );
	ret = goc_systemSendMsg(u, msg);
	wiersz.pText = goc_tableClear( wiersz.pText, &(wiersz.nText) );
	return ret;
	/*
	GOC_StList* lista = (GOC_StList*)u;
	char *pBuf = NULL;
	GOC_COLUMN *k = NULL;
	int i;
	
	if ( lista->flag & GOC_LISTFLAG_EXTERNAL )
		return GOC_ERR_REFUSE;
	// jesli nie ma to stworz nowa kolumne
	if ( lista->nKolumna == 0 )
		goc_listAddColumn(u, lista->width);
	if ( kolumna >= lista->nKolumna )
		return GOC_ERR_REFUSE;
	// zrob kopie dodawanego tekstu
	pBuf = goc_stringCopy(pBuf, tekst);
	// dodaj do wszystkich kolumn wiersz, ustawiajac ten do ktorego
	// ma zostac wpisany tekst
	for ( i=0; i<lista->nKolumna; i++ )
	{
		k = lista->pKolumna[i];
		k->pText = goc_tableAdd(k->pText, &(k->nText),sizeof(char*));
		if ( i != kolumna )
			k->pText[k->nText-1] = NULL;
		else
			k->pText[k->nText-1] = pBuf;
	}
	return GOC_ERR_OK;
	*/
}
/*
 * Odmowa wykonania polecenia, jezeli ustawione dane sa zewnetrzne.
 */
static int listAddRow(GOC_HANDLER u, GOC_LISTROW *wiersz)
{
	GOC_StList* lista = (GOC_StList*)u;
	GOC_COLUMN *k = NULL;
	int i;
	
	if ( lista->flag & GOC_LISTFLAG_EXTERNAL )
		return GOC_ERR_REFUSE;
	// jesli nie ma to stworz nowa kolumne
	if ( lista->nKolumna == 0 )
		goc_listAddColumn(u, goc_elementGetWidth(u));
	// ustal pozcje na podstawie sortowania lub jako ostatni element
	if ( wiersz->nRow == -1 )
	{
		if ( lista->flag & GOC_LISTFLAG_SORT )
		{
			k = lista->pKolumna[0];
			for (i=0; i<k->nText; i++)
			{
				if ( strcmp(wiersz->pText[0], k->pText[i])<0 )
					break;
			}
			wiersz->nRow = i;
		}
		else
		{
			wiersz->nRow = lista->pKolumna[0]->nText;
		}
	}
	// czy nie wykracza ponad dodanie
	if ( wiersz->nRow > lista->pKolumna[0]->nText )
		return GOC_ERR_REFUSE;
	// dodawanie danych wiersza do kolumn
	for ( i=0; i<lista->nKolumna; i++ )
	{
		k = lista->pKolumna[i];
		k->pText = goc_tableInsert(k->pText, &(k->nText), sizeof(char*),
				wiersz->nRow);

		if ( i < wiersz->nText )
		{
			k->pText[wiersz->nRow] = goc_stringCopy(NULL,
				wiersz->pText[i]);
		}
		else
		{
			k->pText[wiersz->nRow] = NULL;
		}
	}
	return GOC_ERR_OK;

//	return goc_listAddColumnText(u, tekst, 0);
/*	GOC_StList* lista = (GOC_StList*)u;
	char *pBuf = NULL;
	GOC_COLUMN *kolumna = NULL;
	if ( lista->flag & GOC_LISTFLAG_EXTERNAL )
		return GOC_ERR_REFUSE;
	if ( lista->nKolumna == 0 )
		goc_listAddColumn(u, lista->width);
	kolumna = lista->pKolumna[0];
	pBuf = goc_stringCopy(pBuf, tekst);
	kolumna->pText = goc_tableAdd( kolumna->pText, &(kolumna->nText),
		sizeof(char*) );
	kolumna->pText[kolumna->nText-1] = pBuf;
//	goc_systemSendMsg(u, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;*/
}

static int listAddText(GOC_HANDLER u, char *buf)
{
	GOC_LISTROW wiersz;
	int retCode;
	memset( &wiersz, 0, sizeof(GOC_LISTROW) );
	wiersz.nRow = -1;
	wiersz.pText = goc_tableAdd(wiersz.pText, &wiersz.nText,
		sizeof(char *));
	wiersz.pText[wiersz.nText-1] = buf;
	GOC_MSG_LISTADDROW( msg, &wiersz );
	retCode = goc_systemSendMsg(u, msg);
	wiersz.pText = goc_tableClear(wiersz.pText, &wiersz.nText);
	return retCode;
}

const char *goc_listGetColumnText(GOC_HANDLER u, int pos, int kol)
{
	GOC_StList *lista = (GOC_StList*)u;
	if ( lista == NULL )
		return NULL;
	if ( kol < 0 )
		return NULL;
	if ( kol >= lista->nKolumna )
		return NULL;
	if ( pos < 0 )
		return NULL;
	if ( pos >= lista->pKolumna[kol]->nText )
		return NULL;
	return lista->pKolumna[kol]->pText[pos];
}
/*
 * Odmowa wykonania polecenia, jezeli ustawione dane sa zewnetrzne.
 */
int goc_listAddText(GOC_HANDLER u, char *tekst)
{
	GOC_MSG_LISTADDTEXT( msg, tekst );
	return goc_systemSendMsg(u, msg);
}

const char *goc_listGetText(GOC_HANDLER u, int pos)
{
	GOC_StList *lista = (GOC_StList*)u;
	if ( lista == NULL )
		return NULL;
	return goc_listGetColumnText(u, pos, lista->nKolumna-1);
}

int goc_listSetTitle(GOC_HANDLER u, const char *tekst)
{
	GOC_StList *lista = (GOC_StList*)u;
	lista->pTytul = goc_stringCopy(lista->pTytul, tekst);
	return GOC_ERR_OK;
}

static GOC_COLOR listSetColor(GOC_HANDLER uchwyt, int pos)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	if ( lista->nKolumna == 0 )
		return lista->color;
	if ( pos != lista->kursor )
		return lista->color;
	else
		return lista->kolorPoleKursor;
}

int goc_listGetRowsAmmount(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int n = 0;
	int j;
	for ( j=0; j<lista->nKolumna; j++ )
	{
		if ( n < lista->pKolumna[j]->nText )
			n = lista->pKolumna[j]->nText;
	}
	return n;
}

// bez konrtoli - odrysuj wskazana linie (pozycja na liscie)
static void listRedrawLine(GOC_HANDLER u, int pos)
{
	GOC_StList *lista = (GOC_StList*)u;
	GOC_StElement e;
	GOC_COLUMN *k;
	GOC_FUN_LABELDRAWER *drawer;
	int j; // licznik kolumn

	drawer = goc_elementGetLabelDrawer(u);
	memcpy(&e, lista, sizeof(GOC_StElement));
	GOC_MSG_LISTSETCOLOR( msg, pos );
	goc_systemSendMsg(u, msg);
	e.color = msgFull.color;
	e.y += pos - lista->start + 1;
	(e.x)++;
	for ( j = 0; j < lista->nKolumna; j++ )
	{
		k = lista->pKolumna[j];
		e.width = k->position;
		if ( pos<k->nText )
			drawer(&e, k->pText[pos]);
		else
			drawer(&e, "");
		e.x += k->position;
	}
}

static int goc_listPaint(GOC_HANDLER u)
{
	GOC_StList *lista = (GOC_StList*)u;
	int i; // licznik zdañ
	int j; // licznik kolumn
	int n; // maksymalna liczba wierszy
	GOC_COLUMN *k;
	GOC_StElement e;
	GOC_FUN_LABELDRAWER *drawer;

	if ( !(lista->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_OK;

	drawer = goc_elementGetLabelDrawer(u);

	memcpy(&e, lista, sizeof(GOC_StElement));
	e.color = lista->kolorRamka;
	goc_elementGetFrameDrawer(u)(&e, "*-*| |*-*");
	// rysowanie tytulu, jesli obecny
	if ( lista->pTytul )
	{
		GOC_StElement elTytul;
		memcpy( &elTytul, lista, sizeof(GOC_StElement));
		elTytul.color = lista->kolorRamka;
		elTytul.height = 1;
		elTytul.x += 2;
		elTytul.flag = GOC_EFLAGA_CENTER;
		if ( elTytul.width > 2 )
		{
			elTytul.width -= 4;
			if ( strlen(lista->pTytul)+2 < elTytul.width )
				elTytul.width = strlen(lista->pTytul)+2;
		}
		else
			elTytul.width = 0;
		drawer(&elTytul, lista->pTytul);

	}
	
	n = goc_listGetRowsAmmount(u);
	j=0;
	for ( i=lista->start; i<n; i++ )
	{
		if ( i - lista->start > goc_elementGetHeight(u)-3 )
			break;
		GOC_MSG_LISTSETCOLOR( msg, i);
		goc_systemSendMsg(u, msg);
		e.color = msgFull.color;
//		j %= lista->nKolumna;
		e.y++;
		e.x = lista->x;
		(e.x)++;
		for ( j = 0; j < lista->nKolumna; j++ )
		{
			k = lista->pKolumna[j];
			e.width = k->position;
			if ( i<k->nText )
				drawer(&e, k->pText[i]);
			else
				drawer(&e, "");
			e.x += k->position;
		}

//		if (!j)
//		{
//		}
//		e.width = lista->pKolumna[j];
//		j++;
	}
	if (lista->kursor != -1)
		goc_gotoxy(
			goc_elementGetScreenX(u)+1,
			goc_elementGetScreenY(u)+(lista->kursor-lista->start) + 1);
	fflush(stdout);
	lista->flag |= GOC_EFLAGA_SHOWN;
	return GOC_ERR_OK;
}

static int listFocus(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	if ( !(lista->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_REFUSE;
	if ( !(lista->flag & GOC_EFLAGA_ENABLE) )
		return GOC_ERR_REFUSE;
	lista->kolorRamka = lista->kolorRamkaAktywny;
	lista->color = lista->kolorPoleAktywny;
	if ( lista->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msg );
		goc_systemSendMsg(uchwyt, msg);
	}
	return GOC_ERR_OK;
}

static int listFocusFree(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	lista->kolorRamka = lista->kolorRamkaNieaktywny;
	lista->color = lista->kolorPoleNieaktywny;
	if ( lista->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msg );
		goc_systemSendMsg(uchwyt, msg);
	}
	return GOC_ERR_OK;
}

char *goc_listGetUnderCursor(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	if ( lista->nKolumna < 0 )
		return NULL;
	if ( lista->kursor < 0 )
		return NULL;
	return lista->pKolumna[0]->pText[lista->kursor];
//	return lista->pText[lista->kursor];
}

// Obliczenie polozenia start po zmianie kursora
static int listCountStart(int cursorPos, GOC_POSITION listHeight, int rowsCnt)
{
	if ( cursorPos < 0 )
		// od poczatku
		return 0;
	if ( cursorPos < listHeight - 2 )
		// pokazuj od poczatku
		return 0;
	else if ( cursorPos >= rowsCnt - listHeight + 2 )
		// pokazuj od konca
		return rowsCnt - listHeight + 2;
	else
		// pokazuj w polowie
		return cursorPos - ((listHeight-2)>>1);
}

int goc_listSetCursor(GOC_HANDLER uchwyt, int pos)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int cnt = goc_listGetRowsAmmount( uchwyt );
	GOC_POSITION height = goc_elementGetHeight(uchwyt);
	if ( lista->nKolumna == 0 )
		return GOC_ERR_REFUSE;
	// jezeli pozycja sie nie zmienila nic nie rob
	if ( lista->kursor == pos )
		return GOC_ERR_OK;
	if ( pos < 0 )
	{
		lista->kursor = -1;
		GOC_MSG_LISTCHANGE( msg, NULL, -1 );
		goc_systemSendMsg(uchwyt, msg);
	}
	else if ( pos < cnt )
	{
		lista->kursor = pos;
		GOC_MSG_LISTCHANGE( msg, lista->pKolumna[0]->pText[lista->kursor],
			lista->kursor );
		goc_systemSendMsg(uchwyt, msg );
	}
	else
		return GOC_ERR_REFUSE;
	lista->start = listCountStart(lista->kursor, height, cnt);
	return GOC_ERR_OK;
}

static int listCursorNext(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int n = goc_listGetRowsAmmount(uchwyt);
	if ( n == 0 )
		return GOC_ERR_OK;
	if ( lista->kursor < n-1 )
	{
		GOC_POSITION height = goc_elementGetHeight(uchwyt);
		lista->kursor++;
		GOC_MSG_LISTCHANGE( msg, lista->pKolumna[0]->pText[lista->kursor],
			lista->kursor );
		goc_systemSendMsg(uchwyt, msg);
		if ( lista->kursor - lista->start >= height-2 )
		{
			// przemaluj calosc, nastapilo przewiniecie strony
			lista->start = listCountStart(lista->kursor, height, n);
			GOC_MSG_PAINT( msgpaint );
			goc_systemSendMsg(uchwyt, msgpaint);
		}
		if ( lista->flag & GOC_EFLAGA_SHOWN )
		{
			// przemaluj tylko linie poprzednia i tam gdzie poszedl kursor
			if ( lista->kursor > 0 )
				listRedrawLine(uchwyt, lista->kursor-1);
			listRedrawLine(uchwyt, lista->kursor);
			goc_gotoxy(
				goc_elementGetScreenX(uchwyt)+1,
				goc_elementGetScreenY(uchwyt)+(lista->kursor-lista->start) + 1);
			fflush(stdout);
		}
	}
	return GOC_ERR_OK;
}

static int listCursorPrev(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	if ( lista->kursor > 0 )
	{
		lista->kursor--;
		GOC_MSG_LISTCHANGE( msg, lista->pKolumna[0]->pText[lista->kursor],
			lista->kursor );
		goc_systemSendMsg(uchwyt, msg );
		if ( lista->start > lista->kursor )
		{
			// przemaluj calosc, nastapilo przewiniecie strony
			int n = goc_listGetRowsAmmount(uchwyt);
			GOC_POSITION height = goc_elementGetHeight(uchwyt);
			lista->start = listCountStart(lista->kursor, height, n);
			GOC_MSG_PAINT( msgpaint );
			goc_systemSendMsg(uchwyt, msgpaint);
		}
		if ( lista->flag & GOC_EFLAGA_SHOWN )
		{
			// przemaluj tylko linie nastepna i tam gdzie poszedl kursor
			listRedrawLine(uchwyt, lista->kursor);
			listRedrawLine(uchwyt, lista->kursor+1);
			goc_gotoxy(
				goc_elementGetScreenX(uchwyt)+1,
				goc_elementGetScreenY(uchwyt)+(lista->kursor-lista->start) + 1);
			fflush(stdout);
		}
	}
	return GOC_ERR_OK;
}

static int lisyHotKeyHome(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int retCode = GOC_ERR_OK;
	retCode = goc_listSetCursor(uchwyt, 0);
	if ( retCode != GOC_ERR_OK )
		return retCode;
	if ( lista->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msgpaint );
		goc_systemSendMsg(uchwyt, msgpaint);
	}
	return GOC_ERR_OK;
}

static int listHotKeyEnd(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int retCode = GOC_ERR_OK;
	retCode = goc_listSetCursor(uchwyt, goc_listGetRowsAmmount( uchwyt )-1);
	if ( retCode != GOC_ERR_OK )
		return retCode;
	if ( lista->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msgpaint );
		goc_systemSendMsg(uchwyt, msgpaint);
	}
	return GOC_ERR_OK;
}

static int listHotKeyPgUp(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int n = goc_listGetRowsAmmount(uchwyt);
	int retCode = GOC_ERR_OK;
	// brak wpisów
	if ( n <= 0 )
		return retCode;
	// kursor jest ju¿ na pocz±tku
	if ( lista->kursor == 0 )
		return retCode;
	// kursor jeszcze nie ustawiony
	if ( lista->kursor < 0 )
	{
		lista->kursor = 0;
		return retCode;
	}
	// wykonaj przesuniecie wszystkiego o lista->height-2
	lista->start -= goc_elementGetHeight(uchwyt)-2;
	lista->kursor -= goc_elementGetHeight(uchwyt)-2;
	// wykonaj wyrównanie
	if ( lista->start < 0 )
		lista->start = 0;
	if ( lista->kursor < 0 )
		lista->kursor = 0;
	// zgloszenie zmiany
	GOC_MSG_LISTCHANGE( msgchange, lista->pKolumna[0]->pText[lista->kursor],
		lista->kursor );
	goc_systemSendMsg(uchwyt, msgchange);
	// wy¶wietlenie
	if ( lista->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msgpaint );
		goc_systemSendMsg(uchwyt, msgpaint);
	}
	return retCode;
}

static int listHotKeyPgDown(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int n = goc_listGetRowsAmmount(uchwyt);
	int retCode = GOC_ERR_OK;
	GOC_POSITION heightOffset;
	// brak wpisów
	if ( n <= 0 )
		return retCode;
	// kursor jest ju¿ na koñcu
	if ( lista->kursor == n-1 )
		return retCode;
	// kursor jeszcze nie ustawiony
	if ( lista->kursor < 0 )
		lista->kursor = 0;
	// wykonaj przesuniecie wszystkiego o lista->height-2
	heightOffset = goc_elementGetHeight(uchwyt)-2;
	lista->start += heightOffset;
	lista->kursor += heightOffset;
	// wykonaj wyrównanie
	if ( lista->start + heightOffset >= n )
		lista->start = n - heightOffset;
	if ( lista->kursor >= n )
		lista->kursor = n-1;
	// zgloszenie zmiany
	GOC_MSG_LISTCHANGE( msgchange, lista->pKolumna[0]->pText[lista->kursor],
		lista->kursor );
	goc_systemSendMsg(uchwyt, msgchange);
	// wy¶wietlenie
	if ( lista->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msgpaint );
		goc_systemSendMsg(uchwyt, msgpaint);
	}
	return retCode;
}

static int listHotKeyNext(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	return listCursorNext(uchwyt);
}

static int listHotKeyPrev(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	return listCursorPrev(uchwyt);
}

static int listInit(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
//	lista->pText = NULL;
//	lista->nText = 0;
	lista->nKolumna = 0;
	lista->pKolumna = NULL;

	lista->pTytul = NULL;

	lista->start = 0;
	lista->kursor = -1;
	lista->kolorRamkaAktywny = GOC_WHITE | GOC_FBOLD;
	lista->kolorRamkaNieaktywny = GOC_WHITE;
	lista->kolorPoleAktywny = lista->color;
	lista->kolorPoleNieaktywny = GOC_WHITE;
	lista->kolorPoleKursor = GOC_WHITE | GOC_FBOLD;

	lista->color = lista->kolorPoleNieaktywny;
	lista->kolorRamka = lista->kolorRamkaNieaktywny;
	goc_hkAdd( uchwyt, 0x600, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			listHotKeyNext );
	goc_hkAdd( uchwyt, 0x603, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			listHotKeyPrev );
	goc_hkAdd( uchwyt, 0x114, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			lisyHotKeyHome );
	goc_hkAdd( uchwyt, 0x117, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			listHotKeyEnd );
	goc_hkAdd( uchwyt, 0x118, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			listHotKeyPgUp );
	goc_hkAdd( uchwyt, 0x119, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			listHotKeyPgDown );
	return GOC_ERR_OK;
}

// teksty wywala, zostaja kolumny 
static int listClear(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	GOC_COLUMN *k;
	int j;
	lista->kursor = -1;
	lista->start = 0;
	
	if ( lista->flag & GOC_LISTFLAG_EXTERNAL )
	{
		for ( j=0; j<lista->nKolumna; j++ )
		{
			k = lista->pKolumna[j];
			k->pText = NULL;
			k->nText = 0;
			lista->flag &= ~GOC_LISTFLAG_EXTERNAL;
		}
	}
	else
	{
		int i;
		for ( j=0; j<lista->nKolumna; j++ )
		{
			k = lista->pKolumna[j];
			for ( i=0; i<k->nText; i++ )
				goc_stringFree(k->pText[i]);
			k->pText = goc_tableClear(k->pText, &k->nText);
		}
	}
	return GOC_ERR_OK;
}

/*
 * Usuniecie elementow znajdujacych sie w liscie.
 * Kolumny pozostaja nie naruszone.
 */
int goc_listClear(GOC_HANDLER uchwyt)
{
	GOC_MSG_LISTCLEAR( msg );
	return goc_systemSendMsg(uchwyt, msg);
}

int listSetExt(
	GOC_HANDLER uchwyt, const char **pTable, unsigned int size)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	
	goc_listClear(uchwyt);
	/* Jesli nie ma zadnej kolumny, to zbuduj */
	if ( lista->nKolumna == 0 )
		goc_listAddColumn(uchwyt, goc_elementGetWidth(uchwyt));
	lista->pKolumna[0]->pText = (char **)pTable;
	lista->pKolumna[0]->nText = size;
	lista->flag |= GOC_LISTFLAG_EXTERNAL;
	return GOC_ERR_OK;
}

int goc_listSetExtTable(
	GOC_HANDLER uchwyt, const char **pTable, unsigned int size)
{
	GOC_MSG_LISTSETEXTERNAL( msg, pTable, size);
	return goc_systemSendMsg(uchwyt, msg);
}

/*
 * Przeszukiwanie pierwszej kolumny za podanym tekstem, jezeli
 * zostanie odnaleziony zostanie zwrocony numer wiersza, w ktorej sie znajduje,
 * w przeciwnym razie zwraca -1
 */
int goc_listFindText(GOC_HANDLER u, const char *tekst)
{
	GOC_StList *lista = (GOC_StList*)u;
	int i;
	GOC_COLUMN *k;

	if ( lista->nKolumna == 0 )
		return -1;
	k = lista->pKolumna[0];
	for ( i=0; i < k->nText; i++ )
	{
		if ( strcmp( k->pText[i], tekst) == 0 )
			return i;
	}
	return -1;
}

static int goc_listDestroy(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	int j;
	goc_listClear(uchwyt);
	goc_stringFree(lista->pTytul);
	for ( j=0; j<lista->nKolumna; j++ )
		free(lista->pKolumna[j]);
	lista->pKolumna = goc_tableClear(
		lista->pKolumna, &lista->nKolumna);
	return goc_elementDestroy(uchwyt);
}

int goc_listListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT(GOC_StList, lista, msg);
		listInit((GOC_HANDLER)lista);
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_INIT_ID )
	{
		return listInit(uchwyt);
	}
	else if ( msg->id == GOC_MSG_PAINT_ID )
	{
		return goc_listPaint(uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUS_ID )
	{
		return listFocus(uchwyt);
	}
	else if ( msg->id == GOC_MSG_FOCUSFREE_ID )
	{
		return listFocusFree(uchwyt);
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		return goc_listDestroy(uchwyt);
	}
	else if ( msg->id == GOC_MSG_LISTSETCOLOR_ID )
	{
		GOC_StMsgListSetColor* msgcolor = (GOC_StMsgListSetColor*)msg;
		msgcolor->color = listSetColor( uchwyt, msgcolor->position );
	}
	else if ( msg->id == GOC_MSG_LISTADDTEXT_ID )
	{
		GOC_StMsgListAddText* msgadd = (GOC_StMsgListAddText*)msg;
		return listAddText(uchwyt, msgadd->pText);
	}
	else if ( msg->id == GOC_MSG_LISTADDROW_ID )
	{
		GOC_StMsgListAddRow* msgadd = (GOC_StMsgListAddRow*)msg;
		return listAddRow(uchwyt, msgadd->pRow);
	}
	else if ( msg->id == GOC_MSG_LISTCLEAR_ID )
	{
		return listClear(uchwyt);
	}
	else if ( msg->id == GOC_MSG_LISTSETEXTERNAL_ID )
	{
		GOC_StMsgListSetExternal* msgset = (GOC_StMsgListSetExternal*)msg;
		return listSetExt(uchwyt, msgset->pTextArray, msgset->nTextArray);
	}
	return GOC_ERR_UNKNOWNMSG;
}
