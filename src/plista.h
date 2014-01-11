#ifndef _SLISTA_H
#define _GOC_SELLIST_H_

#include "lista.h"

extern const char* GOC_ELEMENT_FILELIST;
/*
 * Wystosowanie zapytania, czy dana pozycja jest katalogiem.
 * pBuf nazwa sprawdzanej pozycji
 * nBuf umiejscowienie pozycji na liscie, jezeli -1 to sprawdzenie po
 * 	podanej nazwie obowiazujacej w systemie
 * return GOC_ERR_OK, jezeli jest to katalog
 * return GOC_ERR_FALSE, jezeli nie jest to katalog
 */
extern const char* GOC_MSG_FILELISTISFOLDER;
/*
 * Wystosowanie akcji dodawania pliku do listy
 * pBuf nazwa pozycji
 * nBuf flag, jezeli jest to katalog to SLISTAFLAGA_KATALOG
 * return GOC_ERR_OK, plik nalezy dodac
 * return GOC_ERR_FALSE, nie dodawac pliku (np. filtr)
 */
extern const char* GOC_MSG_FILELISTADDFILE;
/*
 * Wywolanie akcji listowania katalogu. Nie nastepuje zmiana ustawionego
 * folderu listy, ale zostanie zmieniona jej zawartosc.
 * Listowanie polega na kolejnym dodawaniu elementów katalogu do obiektu listy.
 *
 * pBuf nazwa listowanego katalogu, jezeli NULL to uzyje ustawionego aktualnie
 * 	dla listy
 * return GOC_ERR_OK, gdy OK
 * return GOC_ERR_FALSE, gdy wskazana pozycja nie jest katalogiem,
 *	 lub nie jest odnaleziona w systemie plikow.
 */
extern const char* GOC_MSG_FILELISTLISTFOLDER;
/*
 * Ustawienie katalogu dla listy, z rownoczesnym sprawdzeniem, czy
 * takowy istnieje. Ustawienie odbywa sie wzgledem aktualnego katalogu,
 * ktory odzwierciedla lista. Jezeli jest to ustawienie od sciezki glownej,
 * to nastapi przejscie od niej, jezeli wzglednej, to od katalogu listy.
 * W pBuf nazwa
 */
extern const char* GOC_MSG_FILELISTSETFOLDER;
/*
 * Zapytanie czy dana nazwa na li¶cie stanowi katalog wskazuj±cy wy¿ej
 * W pBuf nazwa katalogu, o który nastêpuje zapytanie
 * W nBuf pozycja, o któr± nastêpuje zapytanie. -1 nieznana pozycja
 * Return: GOC_ERR_OK Dany katalog oznacza odpowiada za przej¶cie wy¿ej
 */
extern const char* GOC_MSG_FILELISTISUPFOLDER;
/*
 * Zapytanie o nazwê identyfikuj±c± obecny katalog, tak, aby po przej¶ciu do
 * folderu wy¿ej pozwala³a na odnalezienie wiersza, który bêdzie zawiera³ ow±
 * nazwê.
 * W pBuf wskazanie na wska¼nik, w którym nale¿y przypisaæ kopiê ci±gu znaków
 * 	odpowiadaj±cych identyfikatorowi. Ci±g ten zostanie zwolniony
 * 	automatycznie przez system po wykorzystaniu.
 * Return: GOC_ERR_OK Wska¼nik zosta³ ustawiony
 */
extern const char* GOC_MSG_FILELISTGETFOLDERNAME;

#define GOC_STRUCT_FILELIST \
	GOC_STRUCT_LIST; \
	char *folder;

typedef struct GOC_StFileList
{
	GOC_STRUCT_FILELIST;
} GOC_StFileList;

int goc_filelistSetFolder(GOC_HANDLER uchwyt, const char *dirname);
const char *goc_filelistGetFolder(GOC_HANDLER uchwyt);
int goc_filelistListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf);

#endif // ifndef _GOC_SELLIST_H_
