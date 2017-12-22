#include "global-inc.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include <stdio.h>
#include "lista.h"
#include <tools/screen.h>
#include <tools/mystr.h>
#include <tools/tablica.h>
#include <tools/malloc.h>
#include "napis.h"
#include "ramka.h"
#include "hotkey.h"
#include "system.h"

// element id
const char* GOC_ELEMENT_LIST = "GOC_List";

// message ids
const char* GOC_MSG_LISTCHANGE_ID = "GOC_MsgListChange";
const char* GOC_MSG_LISTSETCOLOR_ID = "GOC_MsgListSetColor";
const char* GOC_MSG_LISTADD_ID = "GOC_MsgListAdd";
const char* GOC_MSG_LISTCLEAR_ID = "GOC_MsgListClear";
const char* GOC_MSG_LISTSETEXTERNAL_ID = "GOC_MsgListSetExternal";

int goc_listAddColumn(GOC_HANDLER u, GOC_POSITION width)
{
	GOC_POSITION free;
	ALLOC(GOC_COLUMN, newColumn);
	int i;
	GOC_StList *lista = (GOC_StList*)u;

	// calculate witdh of new column
	free = goc_elementGetWidth(u);
	free -= 2; /* ramka */
	for ( i=0; i < goc_arraySize(&lista->columns); i++ ) {
		free -= ((GOC_COLUMN*)goc_arrayGet(&lista->columns, i))->position;
	}
	goc_arrayAdd(&lista->columns, newColumn);
	newColumn->position = width <= free ? width : free;

	// new column has to have same number of rows
	if ( goc_arraySize(&lista->columns) > 1 )
	{
		GOC_COLUMN* firstColumn = goc_arrayGet( &lista->columns, 0);
		int i = goc_arraySize( &firstColumn->elements );
		for ( ; i>0; i--) {
			goc_arrayAdd( &newColumn->elements, NULL );
		}
	}
	return GOC_ERR_OK;
}

int goc_listSetToColumn(GOC_HANDLER u, void* element, int column)
{
	GOC_StList* lista = (GOC_StList*)u;
	GOC_COLUMN *k = NULL;
	int i;

	if ( column >= goc_arraySize( &lista->columns) ) {
		return GOC_ERR_REFUSE;
	}
	k = goc_arrayGet( &lista->columns, column );
	for ( i=0; i < goc_arraySize( &k->elements ); i++ )
	{
		if ( goc_arrayGet( &k->elements, i ) == NULL )
		{
			goc_arraySet( &k->elements, i, element );
			return GOC_ERR_OK;
		}
	}
	return goc_listAddTextToColumn(u, element, column);
}
// ustawienie w kolumnie tekstu w pierwszej wolnej pozycji, jesli nie znajdzie
// zostanie dodana nowa kolumna
int goc_listSetTextToColumn(GOC_HANDLER u, const char *tekst, int kolumna)
{
	GOC_StList* lista = (GOC_StList*)u;

	if ( lista->flag & GOC_LISTFLAG_EXTERNAL ) {
		return goc_listSetToColumn( u, (void*)tekst, kolumna );
	} else {
		return goc_listSetToColumn( u, goc_stringCopy(NULL, tekst), kolumna );
	}
}

int goc_listInsertInColumn(GOC_HANDLER u, void* element, int column, int row) {
	GOC_StList* lista = (GOC_StList*)u;
	GOC_COLUMN *k = NULL;
	int i;
	
	if ( lista->flag & GOC_LISTFLAG_EXTERNAL )
		return GOC_ERR_REFUSE;
	// jesli nie ma to stworz nowa kolumne
	if ( goc_arraySize( &lista->columns ) == 0 ) {
		goc_listAddColumn(u, goc_elementGetWidth(u));
	}
	// ustal pozcje na podstawie sortowania lub jako ostatni element
	k = goc_arrayGet( &lista->columns, 0 );
	if ( row == -1 ) {
		if ( lista->flag & GOC_LISTFLAG_SORT ) {
			const char* newElementText = lista->elementToText( element );
			for (i=0; i< goc_arraySize( &k->elements ); i++) {
				void* element = goc_arrayGet( &k->elements, i );
				if ( strcmp(newElementText, lista->elementToText( element )) < 0 ) {
					break;
				}
			}
			row = i;
		}
		else {
			row = goc_arraySize( &k->elements );
		}
	}
	// czy nie wykracza ponad dodanie
	if ( row > goc_arraySize( &k->elements ) ) {
		return GOC_ERR_REFUSE;
	}
	// dodawanie do wybranej kolumny wiersza
	// pozotale kolumny puste
	for ( i=0; i< goc_arraySize( &lista->columns ); i++ ) {
		k = goc_arrayGet( &lista->columns, i );
		if ( i == column ) {
			goc_arrayInsert( &k->elements, row, element );
		} else {
			goc_arrayInsert( &k->elements, row, NULL );
		}
	}
	return GOC_ERR_OK;
}

// Dodanie nowego wiersza dla wybranej kolumny
// tekst zostanie w tym przypadku skopiowany
int goc_listAddTextToColumn(GOC_HANDLER u, const char *tekst, int kolumna)
{
	GOC_StList* lista = (GOC_StList*)u;
	if ( lista->flag & GOC_LISTFLAG_EXTERNAL ) {
		return goc_listInsertInColumn(u, (void*)tekst, kolumna, -1);
	} else {
		return goc_listInsertInColumn(u, goc_stringCopy(NULL, tekst), kolumna, -1);
	}
}

int goc_listAddToColumn(GOC_HANDLER u, void* element, int column) {
	return goc_listInsertInColumn(u, element, column, -1);
}

static int listAdd(GOC_HANDLER u, void* element) {
	return goc_listInsertInColumn(u, element, 0, -1);
}

/*
 * Odmowa wykonania polecenia, jezeli ustawione dane sa zewnetrzne.
 */
int goc_listAddText(GOC_HANDLER u, const char *tekst)
{
	GOC_StList* lista = (GOC_StList*)u;
	if ( lista->flag & GOC_LISTFLAG_EXTERNAL ) {
		return goc_listAdd(u, (void*)tekst);
	} else {
		return goc_listAdd(u, goc_stringCopy(NULL, tekst) );
	}
}

int goc_listAdd(GOC_HANDLER u, void* element) {
	GOC_MSG_LISTADD(msg, element);
	return goc_systemSendMsg(u, msg);
}

int goc_listSetTitle(GOC_HANDLER u, const char *tekst)
{
	GOC_StList *lista = (GOC_StList*)u;
	lista->pTitle = goc_stringCopy(lista->pTitle, tekst);
	return GOC_ERR_OK;
}

static GOC_COLOR listSetColor(GOC_HANDLER uchwyt, int pos)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	if ( goc_arrayIsEmpty( &lista->columns ) ) {
		return lista->color;
	}
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
	for ( j=0; j < goc_arraySize( &lista->columns ); j++ ) {
		GOC_COLUMN* column = goc_arrayGet( &lista->columns, j );
		if ( n < goc_arraySize( &column->elements ) ) {
			n = goc_arraySize( &column->elements );
		}
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
	for ( j = 0; j < goc_arraySize( &lista->columns ); j++ ) {
		k = goc_arrayGet( &lista->columns, j );
		e.width = k->position;
		if ( pos < goc_arraySize( &k->elements ) ) {
			void* element = goc_arrayGet( &k->elements, pos );
			drawer(&e, lista->elementToText(element) );
		} else {
			drawer(&e, "");
		}
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
	if ( lista->pTitle )
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
			if ( strlen(lista->pTitle)+2 < elTytul.width )
				elTytul.width = strlen(lista->pTitle)+2;
		}
		else
			elTytul.width = 0;
		drawer(&elTytul, lista->pTitle);

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
		e.y++;
		e.x = lista->x;
		(e.x)++;
		for ( j = 0; j < goc_arraySize( &lista->columns ); j++ )
		{
			k = goc_arrayGet( &lista->columns, j );
			e.width = k->position;
			if ( i < goc_arraySize( &k->elements ) ) {
				void* element = goc_arrayGet( &k->elements, i );
				drawer(&e, lista->elementToText( element ));
			} else {
				drawer(&e, "");
			}
			e.x += k->position;
		}
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

void* goc_listGetAt(GOC_HANDLER u, int row, int col) {
	GOC_StList *lista = (GOC_StList*)u;
	if ( lista == NULL || col < 0 || col >= goc_arraySize( &lista->columns ) ) {
		return NULL;
	}

	GOC_COLUMN* column = goc_arrayGet( &lista->columns, col );
	if ( column == NULL || row < 0 || row >= goc_arraySize( &column->elements ) ) {
		return NULL;
	}
	return goc_arrayGet( &column->elements, row );
}

const char *goc_listGetTextAt(GOC_HANDLER u, int row, int column)
{
	GOC_StList *lista = (GOC_StList*)u;
	void* element = goc_listGetAt(u, row, column);
	if ( element ) {
		return lista->elementToText( element );	
	} else {
		return NULL;
	}
}

void* goc_listGet(GOC_HANDLER uchwyt) {
	GOC_StList *lista = (GOC_StList*)uchwyt;
	return goc_listGetAt( uchwyt, lista->kursor, 0 );
}

const char *goc_listGetText(GOC_HANDLER uchwyt) {
	GOC_StList *lista = (GOC_StList*)uchwyt;
	return goc_listGetTextAt( uchwyt, lista->kursor, 0 );
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
	if ( goc_arrayIsEmpty( &lista->columns ) ) {
		return GOC_ERR_REFUSE;
	}
	// jezeli pozycja sie nie zmienila nic nie rob
	if ( lista->kursor == pos )
		return GOC_ERR_OK;
	if ( pos < 0 ) {
		lista->kursor = -1;
		GOC_MSG_LISTCHANGE( msg, NULL, -1 );
		goc_systemSendMsg(uchwyt, msg);
	} else if ( pos < cnt ) {
		lista->kursor = pos;
		GOC_COLUMN* column = goc_arrayGet( &lista->columns, 0 );
		GOC_MSG_LISTCHANGE( msg, goc_arrayGet( &column->elements, lista->kursor ),
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
		GOC_COLUMN* column = goc_arrayGet( &lista->columns, 0 );
		GOC_MSG_LISTCHANGE( msg, goc_arrayGet( &column->elements, lista->kursor ),
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
		GOC_COLUMN* column = goc_arrayGet( &lista->columns, 0 );
		GOC_MSG_LISTCHANGE( msg, goc_arrayGet( &column->elements, lista->kursor ),
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
	GOC_COLUMN* column = goc_arrayGet( &lista->columns, 0 );
	GOC_MSG_LISTCHANGE( msgchange, goc_arrayGet( &column->elements, lista->kursor ),
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
	GOC_COLUMN* column = goc_arrayGet( &lista->columns, 0 );
	GOC_MSG_LISTCHANGE( msgchange, goc_arrayGet( &column->elements, lista->kursor ),
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

static const char* listElementToText(void* element) {
	return (const char*)element;
}

static int listInit(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;

	lista->pTitle = NULL;

	lista->start = 0;
	lista->kursor = -1;
	lista->kolorRamkaAktywny = GOC_WHITE | GOC_FBOLD;
	lista->kolorRamkaNieaktywny = GOC_WHITE;
	lista->kolorPoleAktywny = lista->color;
	lista->kolorPoleNieaktywny = GOC_WHITE;
	lista->kolorPoleKursor = GOC_WHITE | GOC_FBOLD;

	lista->color = lista->kolorPoleNieaktywny;
	lista->kolorRamka = lista->kolorRamkaNieaktywny;

	lista->elementToText = listElementToText;

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
	
	for ( j=0; j < goc_arraySize( &lista->columns ); j++ )
	{
		k = goc_arrayGet( &lista->columns, j );
		goc_arrayClear( &k->elements );
		lista->flag &= ~GOC_LISTFLAG_EXTERNAL;
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
	if ( goc_arrayIsEmpty( &lista->columns ) ) {
		goc_listAddColumn(uchwyt, goc_elementGetWidth(uchwyt));
	}
	GOC_COLUMN* column = goc_arrayGet( &lista->columns, 0 );
	for ( int i = 0; i < size; i++ ) {
		goc_arrayAdd( &column->elements, (void*)pTable[i] );
	}
	column->elements.freeElement = NULL;
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

	if ( goc_arrayIsEmpty( &lista->columns ) )
		return -1;
	k = goc_arrayGet( &lista->columns, 0 );;
	for ( i=0; i < goc_arraySize( &k->elements ); i++ )
	{
		void* element = goc_arrayGet( &k->elements, i );
		if ( strcmp( lista->elementToText( element ), tekst) == 0 )
			return i;
	}
	return -1;
}

static int goc_listDestroy(GOC_HANDLER uchwyt)
{
	GOC_StList *lista = (GOC_StList*)uchwyt;
	goc_listClear(uchwyt);
	goc_stringFree(lista->pTitle);
	goc_arrayClear( &lista->columns );
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
	else if ( msg->id == GOC_MSG_LISTADD_ID )
	{
		GOC_StMsgListAdd* msgadd = (GOC_StMsgListAdd*)msg;
		return listAdd(uchwyt, msgadd->element);
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
