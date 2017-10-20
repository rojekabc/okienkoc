#ifndef _GOC_TYPETABLE_H_
#	define _GOC_TYPETABLE_H_
#	include "global-inc.h"
#	include "tablica.h"

// W za³o¿eniu - tablica zawsze utrzymuje w³asn± kopiê elementu
// i ona zarz±dza jego zwalnianiem. Funkcje bez Copy przekazuj±
// do tablicy utworzony wska¼nik, który bêdzie zwalniany. Funkcje
// z Copy wykonuj± kopiê elementu i go sk³aduj±.
#define GOC_TypeTableDeclare(name, funprefix, type) \
	typedef struct name \
	{ \
		type* pElement; \
		_GOC_TABEL_SIZETYPE_ nElement; \
	} name;\
	\
	name* funprefix ## Alloc(); \
	name* funprefix ## Free(name* pTable); \
	name* funprefix ## Add(name* pTable, type element); \
	name* funprefix ## AddCopy(name* pTable, type element); \
	name* funprefix ## Insert(name* pTable, type element, int pos); \
	name* funprefix ## InsertCopy(name* pTable, type element, int pos); \
	name* funprefix ## Remove(name* pTable, int pos);

#define GOC_TypeTableDefine(name, funprefix, type, freefun, copyfun) \
	name* funprefix ## Alloc() \
	{ \
		name* result = malloc(sizeof(name)); \
		memset(result, 0, sizeof(name)); \
		return result; \
	} \
	\
	name* funprefix ## Free(name* pTable) \
	{ \
		_GOC_TABEL_SIZETYPE_ i; \
		if ( ! pTable ) \
			return NULL; \
		if ( pTable->pElement ) \
		{ \
			for ( i = 0; i < pTable->nElement; i++ ) \
				pTable->pElement[i] = freefun( pTable->pElement[i] ); \
			free(pTable->pElement); \
		} \
		free(pTable); \
		return pTable; \
	} \
	\
	name* funprefix ## Add(name* pTable, type element) \
	{ \
		if ( pTable == NULL ) \
			pTable = funprefix ## Alloc(); \
		pTable->pElement = goc_tableAdd(pTable->pElement, &(pTable->nElement), sizeof(type)); \
		pTable->pElement[pTable->nElement-1] = element; \
		return pTable; \
	} \
	\
	name* funprefix ## AddCopy(name* pTable, type element) \
	{ \
		if ( pTable == NULL ) \
			pTable = funprefix ## Alloc(); \
		pTable->pElement = goc_tableAdd(pTable->pElement, &pTable->nElement, sizeof(type)); \
		pTable->pElement[pTable->nElement-1] = copyfun(element); \
		return pTable; \
	} \
	\
	name* funprefix ## Insert(name* pTable, type element, int pos) \
	{ \
		if ( pTable == NULL ) \
			pTable = funprefix ## Alloc(); \
		pTable->pElement = goc_tableInsert(pTable->pElement, &pTable->nElement, sizeof(type), pos); \
		pTable->pElement[pos] = element; \
		return pTable; \
	} \
	\
	name* funprefix ## InsertCopy(name* pTable, type element, int pos) \
	{ \
		if ( pTable == NULL ) \
			pTable = funprefix ## Alloc(); \
		pTable->pElement = goc_tableInsert(pTable->pElement, &pTable->nElement, sizeof(type), pos); \
		pTable->pElement[pos] = copyfun(element); \
		return pTable; \
	} \
	\
	name* funprefix ## Remove(name* pTable, int pos) \
	{ \
		if ( pTable == NULL ) \
			return pTable; \
		pTable->pElement = goc_tableRemove(pTable->pElement, &pTable->nElement, sizeof(type), pos); \
		return pTable; \
	}

// name - nazwa struktury
// funprefix - nazwa prefixowa funkcji
// type - typ u¿ywanych danych dla emenetu
// freefun - funkcja zwalniaj±ca [ type free(type element) ]
// copyfun - funkcja kopiuj±ca [ type copy(const type src) ]
// cmpfun - funkcja porównuj±ca [ int compare(const type elemen1, const type element 2) ]
#define GOC_TypeTable(name, funprefix, type, freefun, copyfun) \
	GOC_TypeTableDeclare(name, funprefix, type) \
	GOC_TypeTableDefine(name, funprefix, type, freefun, copyfun)

#endif
