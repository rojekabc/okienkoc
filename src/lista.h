#ifndef _GOC_LIST_H_
#define _GOC_LIST_H_

#include "element.h"

extern const char* GOC_ELEMENT_LIST;

/*
 * Przesuniecie na liscie. W nBuf pozycja, w pBuf nazwa
 */
extern const char* GOC_MSG_LISTCHANGE;
/*
 * Ustalenie koloru, jaki ma zostac nadany dla danego pola.
 * W nBuf jest pozycja pola, o które nastepuje zapytanie
 * W pBuf jest wskazanie na zmienna typu GOC_COLOR, ktora nalezy ustawic
 */
extern const char* GOC_MSG_LISTSETCOLOR;
/*
 * Dodanie tekstu do listy.
 * W pBuf wskazanie na dodawany tekst
 * W nBuf zero.
 */
extern const char* GOC_MSG_LISTADDTEXT;
/*
 * Czyszczenie listy. pBuf = 0, nBuf = 0
 */
extern const char* GOC_MSG_LISTCLEAR;
/*
 * Ustawienie danych zewnetrznych
 * pBuf dane zewnetrzne
 * nBuf ilosc danych
 */
extern const char* GOC_MSG_LISTSETEXTERNAL;
/*
 * Dodanie tekstu do listy.
 * W pBuf wskazanie na dodawana strukture wiersza
 * W nBuf zero.
 */
extern const char* GOC_MSG_LISTADDROW;

typedef struct GOC_StColumn
{
	char **pText;
	int nText;
	GOC_POSITION position;
} GOC_COLUMN;

typedef struct GOC_StListRow
{
	const char **pText; // teksty do kolumn - kolejnych
	int nText; // liczba zamieszczonych tekstow
	int nRow; // jesli -1 ustawi po wykonaniu funkcji nadana
	// pozcje, jesli != -1 to umiesci we wskazanej pozcji.
} GOC_LISTROW;

#define GOC_STRUCT_LIST \
	GOC_STRUCT_ELEMENT; \
	GOC_COLOR kolorRamka; \
	GOC_COLOR kolorRamkaNieaktywny; \
	GOC_COLOR kolorRamkaAktywny; \
	GOC_COLOR kolorPoleAktywny; \
	GOC_COLOR kolorPoleNieaktywny; \
	GOC_COLOR kolorPoleKursor; \
	GOC_COLUMN **pKolumna; \
	char *pTytul; \
	int nKolumna; \
	int kursor; \
	int start

typedef struct GOC_StList
{
	GOC_STRUCT_LIST;
} GOC_StList;

int goc_listListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, uintptr_t nBuf);
int goc_listAddText(GOC_HANDLER u, const char *tekst);
int goc_listAddColumnText(GOC_HANDLER u, const char *tekst, int kolumna);
int goc_listSetColumnText(GOC_HANDLER u, const char *tekst, int kolumna);
int goc_listAddColumn(GOC_HANDLER u, GOC_POSITION width);
int goc_listSetCursor(GOC_HANDLER uchwyt, int pos);
#define goc_listGetCursor(uchwyt) ((GOC_StList*)uchwyt)->kursor
char *goc_listGetUnderCursor(GOC_HANDLER u);
int goc_listClear(GOC_HANDLER uchwyt);
int goc_listSetExtTable(
	GOC_HANDLER uchwyt, const char **pTable, unsigned int size);
const char *goc_listGetText(GOC_HANDLER u, int pos);
int goc_listGetRowsAmmount(GOC_HANDLER u);
int goc_listFindText(GOC_HANDLER u, const char *tekst);
int goc_listSetTitle(GOC_HANDLER u, const char *tekst);
#define goc_listGetTitle(uchwyt) ((GOC_StList*)uchwyt)->pTytul
const char *goc_listGetColumnText(GOC_HANDLER u, int pos, int kol);

#endif // ifndef _GOC_LIST_H_
