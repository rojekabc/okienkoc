#ifndef _GOC_TABEL_H_
#define _GOC_TABEL_H_
#include <unistd.h>

#define _GOC_TABEL_SIZETYPE_ int
/*
 * Funkcje tutaj dostêpne nie inicjuj± warto¶ci tablicy. Jedyne co robi±,
 * to allokuj± dan± ilo¶æ miejsca na element lub deallokuj±.
 */
void *goc_tableAdd(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize);
void *goc_tableResize(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize,
		_GOC_TABEL_SIZETYPE_ nElem);
void *goc_tableRemove(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize,
		_GOC_TABEL_SIZETYPE_ pozycja);
void *goc_tableClear(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica);
void *goc_tableInsert(
		void *pTablica, _GOC_TABEL_SIZETYPE_ *nTablica, size_t elemSize,
		_GOC_TABEL_SIZETYPE_ pozycja);

#define GOC_TABLEPTR(type, name) \
	type* p ## name; \
	_GOC_TABEL_SIZETYPE_ n ## name

#define GOC_TABLEIPTR(type, name) \
	type* p ## name = NULL; \
	_GOC_TABEL_SIZETYPE_ n ## name = 0

#define GOC_TABLEADDPTR_FULL(type, tablePointer, tableCounter) \
	( tablePointer = goc_tableAdd( tablePointer, &(tableCounter), sizeof(type) ), tablePointer[ tableCounter - 1 ] = (type)malloc(sizeof(type)), memset( tablePointer[ tableCounter - 1], 0, sizeof(type)), tablePointer[ tableCounter - 1 ])

#define GOC_TABLEADDPTR(type, name) GOC_TABLEADDPTR_FULL(type, p ## name, n ## name)
#define GOC_TABLEADDSPTR(type, struct, name) GOC_TABLEADDPTR_FULL(type, struct->p ## name, struct->n ## name)

//	( p ## name = goc_tableAdd( p ## name, &n ## name, sizeof(type) ), p ## name[ n ## name - 1 ] = (type*)malloc(sizeof(type)), memset( p ## name[ n ## name - 1], 0, sizeof(type)), p ## name[ n ## name - 1 ])

#define GOC_TABLEPUTPTR(type, name, ptr) \
	( p ## name = goc_tableAdd( p ## name, &n ## name, sizeof(type) ), p ## name[ n ## name - 1 ] = (type)malloc(sizeof(type)), memset( p ## name[ n ## name - 1], 0, sizeof(type)), p ## name[ n ## name - 1 ] = ptr)

#define GOC_STABLEFREE(structptr, name, freefun) \
if ( structptr ) \
{ \
	int i; \
	if ( freefun ) { \
		for(i=0; i<structptr->n ## name; i++) { \
			if ( structptr->p ## name[i] ) { \
				freefun(structptr->p ## name[i]); \
			} \
		} \
	} \
	structptr->p ## name = goc_tableClear( structptr->p ## name, &(structptr->n ## name) ); \
}

#define GOC_TABLEFREE(name, freefun) { \
	int i; \
	if ( freefun ) \
		for(i=0; i<n ## name; i++) \
			freefun(p ## name[i]); \
	p ## name = goc_tableClear( p ## name, &n ## name ); \
}

#define GOC_PTRTABLECLEAR(structptr, name) { \
	if ( structptr ) \
		structptr->p ## name = goc_tableClear( structptr->p ## name, &(structptr->n ## name) ); \
	else \
		p ## name = goc_tableClear( p ## name, &n ## name );

#endif
