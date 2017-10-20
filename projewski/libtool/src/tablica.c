#include "tablica.h"
#include <unistd.h>
#include <string.h>
#include "global-inc.h"
/*
 * Funkcja:
 * Dodanie jednego elementu na koniec tablicy
 * Argumenty nie s± sprawdzane pod k±tem poprawno¶ci. Zak³ada siê, ¿e
 * 1. nTablica != NULL
 * 2. je¿eli *nTablica != 0  =>  pTablica != NULL
 * 3. elemSize > 0
 *
 * Argumenty:
 * pTablica - aktualne wskazanie na tablicê
 * nTablica - wskazanie na zmienn± zawieraj±c± liczbê elementów w tablicy
 * elemSize - rozmiar pojedynczego elementu
 *
 * Zwraca:
 * Wskazanie na nowy obszar pamiêci z pamiêci± przydzielon± do tablicy.
 * Stare wskazanie jest ju¿ nieaktualne !
 * Dodany obszar nie jest zerowany !
 *
 */
void *goc_tableAdd(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize)
{
	if ( *nTablica )
	{
		(*nTablica)++;
		return realloc(pTablica, elemSize*(*nTablica));
	}
	else
	{
		(*nTablica) |= 1;
		return malloc(elemSize);
	}
}
/*
 * Funkcja:
 * Dodanie podanej liczby elementów na koniec tablicy
 * Argumenty nie s± sprawdzane pod k±tem poprawno¶ci. Zak³ada siê, ¿e
 * 1. nTablica != NULL
 * 2. je¿eli *nTablica != 0  =>  pTablica != NULL
 * 3. elemSize > 0
 * 4. nElem > 0
 *
 * Argumenty:
 * pTablica - aktualne wskazanie na tablicê
 * nTablica - wskazanie na zmienn± zawieraj±c± liczbê elementów w tablicy
 * elemSize - rozmiar pojedynczego elementu
 * nElem - liczba dodawanych elementów
 *
 * Zwraca:
 * Wskazanie na nowy obszar pamiêci z pamiêci± przydzielon± do tablicy.
 * Stare wskazanie jest ju¿ nieaktualne !
 * Dodany obszar nie jest zerowany !
 *
 */
void *goc_tableResize(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize,
		_GOC_TABEL_SIZETYPE_ nElem)
{
	if ( nElem == 0 )
		return pTablica;
	if ( *nTablica )
	{
		(*nTablica) += nElem;
		return realloc(pTablica, elemSize*(*nTablica));
	}
	else
	{
		(*nTablica) = nElem;
		return malloc(elemSize*nElem);
	}
}
/*
 * Funkcja:
 * Usuniêcie jednego elementu
 * Argumenty nie s± sprawdzane pod k±tem poprawno¶ci. Zak³ada siê, ¿e
 * 1. nTablica != NULL
 * 2. je¿eli *nTablica != 0  =>  pTablica != NULL
 * 3. elemSize > 0
 *
 * Argumenty:
 * pTablica - aktualne wskazanie na tablicê
 * nTablica - wskazanie na zmienn± zawieraj±c± liczbê elementów w tablicy
 * elemSize - rozmiar pojedynczego elementu
 * pozycja - pozycja elementu do usuniêcia
 *
 * Zwraca:
 * Wskazanie na nowy obszar pamiêci z pamiêci± przydzielon± do tablicy.
 * Stare wskazanie jest ju¿ nieaktualne (zwolniona pamiêæ) !
 * Je¿eli rozmiar nowej tablicy jest zerowy, zwróci wskazanie na NULL
 *
 */
void *goc_tableRemove(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize,
		_GOC_TABEL_SIZETYPE_ pozycja)
{
	if ( *nTablica )
	{
		void *tmp = NULL;
		(*nTablica)--;
		if ( *nTablica )
		{
			tmp = malloc(elemSize*(*nTablica));
			memcpy(tmp, pTablica, elemSize*pozycja);
			if ( *nTablica > pozycja )
				memcpy(((char*)tmp)+(elemSize*pozycja),
				  	((char *)pTablica)+(elemSize*(pozycja+1)),
					  ((*nTablica)-pozycja)*(elemSize) );
		}
		free(pTablica);
		return tmp;
	}
	else
		return NULL;
}

/*
 * Funkcja:
 * Wyczyszczenie tablicy. Zajêta pamiêæ jest zwalniana.
 *
 * Argumenty:
 * pTablica - wskazanie na tablicê elementów
 * nTablica - wskazanie na licznik elementów tablicy
 *
 * Zwraca:
 * Nowe wskazanie. Zawsze jako NULL.
 *
 */
void *goc_tableClear(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica)
{
	if ( *nTablica )
		free(pTablica);
	(*nTablica) = 0;
	return NULL;
}

/*
 * Funkcja:
 * Wstawienie w obszartablicy miejsca na nowy element.
 *
 * Argumenty:
 * pTablica - tablica poddawana zmianie.
 * nTablica - licznik elementów tablicy
 * elemSize - rozmiar pojedynczego elementu tablicy
 * pozycja  - pozycja, w jak± nale¿y wstawiæ element
 *
 * Zwraca:
 * wskazanie na now± tablicê z zarezerwowan± przestrzeni±
 * stara tablica jest zwalniana.
 *
 *
 */
void *goc_tableInsert(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize,
		_GOC_TABEL_SIZETYPE_ pozycja)
{
	if ( *nTablica )
	{
		void *tmp;
		(*nTablica)++;
		tmp = malloc( (*nTablica)*elemSize );
		if ( pozycja )
		{
			memcpy( tmp, pTablica, elemSize*(pozycja) );
			if ( pozycja < *nTablica-1 )
				memcpy(((char*)tmp)+(elemSize*(pozycja+1)),
				  	((char *)pTablica)+(elemSize*pozycja),
					((*nTablica)-pozycja-1)*(elemSize) );
		}
		else
		{
			memcpy(((char*)tmp)+(elemSize), pTablica, ((*nTablica)-1)*(elemSize) );
		}
		free(pTablica);
		return tmp;
	}
	else
	{
		(*nTablica)++;
		return malloc(elemSize);
	}
}
