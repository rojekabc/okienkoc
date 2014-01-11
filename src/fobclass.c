#include "global-inc.h"
#include <dlfcn.h>
#include "mystr.h"
#include "fobclass.h"
#include "nbitpola.h"
/*
 * O samym systemie.
 * Pierwsza stworzona klasa jest traktowana, jako domyslna, w przeciwnym razie zostanie utworzona domyslna klasa o nazwie = null.
 * W systemie moze istniec tylko jedna klasa o danej nazwie.
 * Nazwy funkcji przypisanych do klasy moga sie powtarzac, ze wzgledu na mozliwosc wielokrotnego przeslaniania funkcji.
 * Wymaga sie, aby GOC_OBJECTHANDLER stanowil wskazanie na zalozony obiekt - nie nalezy przekazywac nie wskazan - np. int
 *
 */

const char *TEXT_NOMEMORY = "Cannot alloc more memory";
const char *TEXT_NOCLASSFOUND = "Cannot find specified class";
const char *TEXT_CLASSEXISTS = "Specified class already exists";
const char *TEXT_NOFUNCTIONFOUND = "Cannot find specified function";
const char *TEXT_NOLIBFOUND = "Library not found in system";
const char *TEXT_NOLIBLOAD = "Cannot load a external library";
const char *TEXT_NOFUNLOAD = "Cannot load a external function";
const char *TEXT_NOOBJECTFOUND = "Cannot find an object";
const char *TEXT_OBJECTEXISTS = "Object already registered";
const char *TEXT_NOTALLOWED = "Operation not allowed";
const char *TEXT_WRONGARG = "Wrong argument";

// zmienne systemowe
GOC_OBJECTCLASS **_pSystemClass = NULL;
_GOC_TABEL_SIZETYPE_ _nSystemClass = 0;

typedef struct GOC_EXTLIB
{
	void *handler;
	char *name;
} GOC_EXTLIB;
GOC_EXTLIB **_pExternalLib = NULL;
_GOC_TABEL_SIZETYPE_ _nExternalLib = 0;

#define GOC_FLAGEXTERNAL 1

typedef struct GOC_INSTANCE
{
	GOC_OBJECTCLASS *class;
	GOC_OBJECTHANDLER handler;
} GOC_INSTANCE;
GOC_INSTANCE **_pInstance = NULL;
_GOC_TABEL_SIZETYPE_ _nInstance = 0;
GOC_StNBitField *_pExternal = NULL;

// funkcje wewnetrzne
static GOC_FUNCTIONRETURN hasLibrary(const char *name);
static GOC_FUNCTIONRETURN hasFunction(GOC_OBJECTCLASS *oc, const char *name, int start);
static GOC_FUNCTIONRETURN registerFunctionForClass(
	GOC_OBJECTCLASS *oc, const char *name, GOC_FUNCTIONDEFINE *fun);
static GOC_FUNCTIONRETURN unregisterFunctionForClass(
	GOC_OBJECTCLASS *oc, const char *name);
static GOC_FUNCTIONRETURN hasObject(GOC_OBJECTHANDLER handler);
static GOC_FUNCTIONRETURN registerObjectForClass(
	GOC_OBJECTCLASS *tmp, GOC_OBJECTHANDLER handler, unsigned char flag);
static GOC_FUNCTIONRETURN callpath(
	GOC_OBJECTCLASS *class, GOC_OBJECTHANDLER handler, const char *funname,
	void *pBuf, unsigned int nBuf);
static GOC_FUNCTIONRETURN isObjectExternal(GOC_OBJECTHANDLER handler);

// szablon
/*
 * Opis.
 * Funkcja wewnetrzna.
 *
 * Parametry:
 * param - opis
 *
 * Kody powrotu:
 * GOC_ERR_OK - opis
 * 	result - opis
 * GOC_ERR_FALSE
 *	TEXT_ - opis
 *	
 */

/*
 * Sprawdza, czy klasa o podanej nazwie jest utworzona w systemie.
 * Funkcja wewnetrzna.
 *
 * Parametry:
 * name - nazwa klasy
 *
 * Kody powrotu:
 * GOC_ERR_OK - klasa zostala znaleciona
 * 	result - position klasy w tablicy systemowej (dana nie udostepniana na zewnatrz)
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasy nie znaleziono
 *	
 */
GOC_FUNCTIONRETURN hasClass(const char *name)
{
	int i;
	for ( i=0; i<_nSystemClass; i++ )
	{
		if ( goc_stringEquals( _pSystemClass[i]->name, name ) )
			GOC_RET_RESULT(i);
	}
	GOC_RET_FALSE( TEXT_NOCLASSFOUND );
}

/*
 * Sprawdza, czy biblioteka o podanej nazwie jest zaladowana w systemie.
 * Funkcja wewnetrzna.
 *
 * Parametry:
 * name - nazwa biblioteki
 *
 * Kody powrotu:
 * GOC_ERR_OK - biblioteka zostala znaleciona
 * 	result - position biblioteki w tablicy systemowej (dana nie udostepniana na zewnatrz)
 * GOC_ERR_FALSE
 *	TEXT_NOLIBFOUND - biblioteki nie znaleziono
 *	
 */
static GOC_FUNCTIONRETURN hasLibrary(const char *name)
{
	int i;
	for ( i=0; i<_nExternalLib; i++ )
	{
		if ( goc_stringEquals( _pExternalLib[i]->name, name ) )
			GOC_RET_RESULT(i);
	}
	GOC_RET_FALSE( TEXT_NOLIBFOUND );
}

/*
 * Funkcja rejestruje nowa klase w systemie.
 *
 * Parametry:
 * name - unikatowa nazwa klasy rejestrowanej
 *
 * Kody powrotu:
 * GOC_ERR_OK - zarejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOMEMEORY - brak pamieci
 *	TEXT_CLASSEXISTS - klasa juz istnieje w systemie
 *	
 */
GOC_FUNCTIONRETURN registerClass(const char *name)
{
	GOC_OBJECTCLASS *tmp;
	if ( hasClass(name).code == GOC_ERR_OK )
		GOC_RET_FALSE( TEXT_CLASSEXISTS );
	tmp = (GOC_OBJECTCLASS *)malloc(sizeof(GOC_OBJECTCLASS));
	if ( !tmp )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	memset(tmp, 0, sizeof(GOC_OBJECTCLASS));
	tmp->name = goc_stringCopy( tmp->name, name );
	_pSystemClass = goc_tableAdd( _pSystemClass, &_nSystemClass, sizeof(GOC_OBJECTCLASS*) );
	if ( !_pSystemClass )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	_pSystemClass[_nSystemClass-1] = tmp;
	GOC_RET_OK;
}

GOC_FUNCTIONRETURN registerClassParented(
	const char *classname, const char *parentclassname)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTCLASS *parentclass = NULL;

	// czy jest klasa rodzica
	res = hasClass( parentclassname );
	if ( res.code != GOC_ERR_OK )
		return res;
	parentclass = _pSystemClass[ res.result ];

	// zarejestrowanie nowej klasy
	res = registerClass( classname );
	if ( res.code != GOC_ERR_OK )
		return res;

	// przypisanie do nowej klasy rodzica
	_pSystemClass[ _nSystemClass-1 ]->parent = parentclass;
	GOC_RET_OK;
}

/*
 * Wyrejestrowanie klasy z systemu.
 *
 * Parametry:
 * name - nzawa klasy nadana podczas rejestrowania
 *
 * Kody powrotu:
 * GOC_ERR_OK - opis
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasa nie zostala zarejestrowana w systemie
 *	
 */
GOC_FUNCTIONRETURN unregisterClass(const char *name)
{
	GOC_OBJECTCLASS *tmp;
	GOC_FUNCTIONRETURN res;
	int i;

	res = hasClass(name);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	tmp = _pSystemClass[res.result];
	tmp->name = goc_stringFree( tmp->name );
	for (i=0; i<tmp->nFunction; i++)
		unregisterFunctionForClass(tmp, tmp->function[i]->name);
	// TODO: Zwolnienie inych pol, stanowiacych zawartosc definicji klasy
	free(tmp);
	_pSystemClass = goc_tableRemove( _pSystemClass, &_nSystemClass, sizeof(GOC_OBJECTCLASS*), res.result );
	GOC_RET_OK;
}

/*
 * Sprawdzenie od podanego punktu, z przeszukiwaniem w dol, czy w okreslonej klasie istnieje func typowana przez podana nazwe.
 * Funkcja wewnetrzna.
 *
 * Parametry:
 * oc - przeszukiwana klasa
 * name - nazwa funkcji
 * start - punkt rozpoczecia poszukiwan, bez tego punktu (o jeden mniej)
 *
 * Kody powrotu:
 * GOC_ERR_OK - znaleziono
 * 	result - position
 * GOC_ERR_FALSE
 *	TEXT_NOFUNCTIONFOUND - nie znaleziono funkcji
 *	
 */
static GOC_FUNCTIONRETURN hasFunction(GOC_OBJECTCLASS *oc, const char *name, int start)
{
	int i;
	for ( i=start-1; i>=0; i-- )
	{
		if ( goc_stringEquals( oc->function[i]->name, name ) )
			GOC_RET_RESULT(i);
	}
	GOC_RET_FALSE( TEXT_NOFUNCTIONFOUND );
}

/*
 * Zarejestruj funkcje dla klasy.
 * Funkcja wewnetrzna.
 *
 * Parametry:
 * oc - klasa
 * name - nazwa funkcji
 * fun - wskazanie na funkcje
 *
 * Kody powrotu:
 * GOC_ERR_OK - zarejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOMEMORY - brak pamieci
 *	
 */
static GOC_FUNCTIONRETURN registerFunctionForClass(GOC_OBJECTCLASS *oc, const char *name, GOC_FUNCTIONDEFINE *fun)
{
	GOC_OBJECTFUNCTION *of = NULL;
	
	of = (GOC_OBJECTFUNCTION *)malloc(sizeof(GOC_OBJECTFUNCTION));
	if ( !of )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	memset( of, 0, sizeof(GOC_OBJECTFUNCTION) );
	of->name = goc_stringCopy( of->name, name );
	of->function = fun;
	oc->function = goc_tableAdd(oc->function, &(oc->nFunction), sizeof(GOC_OBJECTFUNCTION));
	if ( !oc->function )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	oc->function[oc->nFunction - 1] = of;
	GOC_RET_OK;
}

/*
 * Wyrejestrowanie funkcji z danej klasy.
 * Funkcja wewnetrzna.
 *
 * Parametry:
 * oc - klasa
 * name - nazwa funkcji
 *
 * Kody powrotu:
 * GOC_ERR_OK - wyrejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOFUNCTIONFOUND - nie znaleziono funkcji
 *	
 */
static GOC_FUNCTIONRETURN unregisterFunctionForClass(GOC_OBJECTCLASS *oc, const char *name)
{
	GOC_FUNCTIONRETURN res;

	// znalezienie pozycji wystapienia funkcji
	res = hasFunction(oc, name, oc->nFunction);
	if ( res.code == GOC_ERR_FALSE )
		return res;

	// zwolnienie zasobow pamieciowych
	goc_stringFree(oc->function[res.result]->name);
	free(oc->function[res.result]);

	// usuniecie z tablicy
	oc->function = goc_tableRemove( oc->function, &(oc->nFunction), sizeof(GOC_OBJECTFUNCTION), res.result );
	GOC_RET_OK;
}

/*
 * Zarejestrowanie funkcji dla domyslnej klasy.
 *
 * Parametry:
 * name - nazwa funkcji
 * fun - func
 *
 * Kody powrotu:
 * GOC_ERR_OK - zarejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasy nie znaleziono
 *	TEXT_NOMEMORY - brak pamieci
 *	
 */
GOC_FUNCTIONRETURN registerFunction(const char *name, GOC_FUNCTIONDEFINE *fun)
{
	GOC_OBJECTCLASS *tmp;
	if ( !_nSystemClass )
		GOC_RET_FALSE( TEXT_NOCLASSFOUND );
	tmp = _pSystemClass[0];
	return registerFunctionForClass( tmp, name, fun );
}

/*
 * Zarejestrowanie funkcji dla wskazanej klasy.
 *
 * Parametry:
 * classname - nazwa klasy
 * name - nazwa funckji
 * fun - func
 *
 * Kody powrotu:
 * GOC_ERR_OK - zarejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasy nie znaleziono
 *	TEXT_NOMEMORY - brak pamieci
 *	
 */
GOC_FUNCTIONRETURN registerClassFunction(const char *classname, const char *name, GOC_FUNCTIONDEFINE *fun)
{
	GOC_OBJECTCLASS *tmp;
	GOC_FUNCTIONRETURN res;
	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	tmp = _pSystemClass[res.result];
	return registerFunctionForClass( tmp, name, fun );
}

/*
 * Wyrejestrowanie funkcji z domyslnej klasy.
 *
 * Parametry:
 * name - nazwa funckji
 *
 * Kody powrotu:
 * GOC_ERR_OK - wyrejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasy nie znaleziono
 *	TEXT_NOFUNCTIONFOUND - nie znaleziono funkcji
 *	
 */
GOC_FUNCTIONRETURN unregisterFunction(const char *name)
{
	GOC_OBJECTCLASS *tmp;
	if ( !_nSystemClass )
		GOC_RET_FALSE( TEXT_NOCLASSFOUND );
	tmp = _pSystemClass[0];
	return unregisterFunctionForClass( tmp, name );
}

/*
 * Wyrejestrowanie funkcji z domyslnej klasy.
 *
 * Parametry:
 * classname - nazwa klasy
 * name - nazwa funckji
 *
 * Kody powrotu:
 * GOC_ERR_OK - wyrejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasy nie znaleziono
 *	TEXT_NOFUNCTIONFOUND - nie znaleziono funkcji
 */
GOC_FUNCTIONRETURN unregisterClassFunction(const char *classname, const char *name)
{
	GOC_OBJECTCLASS *tmp;
	GOC_FUNCTIONRETURN res;
	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	tmp = _pSystemClass[res.result];
	return unregisterFunctionForClass( tmp, name );
}



/*
 * Zarejestrowanie zewnetrznej funkcji dla domyslnej klasy.
 *
 * Parametry:
 * name - nazwa funkcji
 * funName - nazwa funkcji
 * libname - nazwa biblioteki
 *
 * Kody powrotu:
 * GOC_ERR_OK - zarejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasy nie znaleziono
 *	TEXT_NOMEMORY - brak pamieci
 *	TEXT_NOLIBLOAD - nie udalo sie zaladowac biblioteki
 *	TEXT_NOFUNLOAD - nie udalo sie zaladowac funkcji
 *	
 */
GOC_FUNCTIONRETURN registerExtFunction(const char *name, const char *funName, const char *libname)
{
	GOC_OBJECTCLASS *tmp = NULL;
	void *funHandler = NULL;
	GOC_FUNCTIONRETURN res;
	GOC_EXTLIB *extlib = NULL;

	if ( !_nSystemClass )
		GOC_RET_FALSE( TEXT_NOCLASSFOUND );
	tmp = _pSystemClass[0];

	res = hasLibrary(libname);
	if ( res.code == GOC_ERR_OK )
	{
		extlib = _pExternalLib[res.result];
	}
	else
	{
		extlib = (GOC_EXTLIB*)malloc( sizeof(GOC_EXTLIB) );
		if ( extlib == NULL )
			GOC_RET_FALSE( TEXT_NOMEMORY );
		memset(extlib, 0, sizeof(GOC_EXTLIB));
		extlib->handler = dlopen(libname, RTLD_NOW);
		if (extlib->handler == NULL)
			GOC_RET_FALSE( TEXT_NOLIBLOAD );
		extlib->name = goc_stringCopy(extlib->name, libname);
		_pExternalLib = goc_tableAdd(_pExternalLib, &_nExternalLib, sizeof(GOC_EXTLIB*));
		if ( !_pExternalLib )
			GOC_RET_FALSE( TEXT_NOMEMORY );
		_pExternalLib[_nExternalLib-1 ] = extlib;
	}

	funHandler = dlsym(extlib->handler, funName);
	if (funHandler == NULL )
		GOC_RET_FALSE( TEXT_NOFUNLOAD );
	
	return registerFunctionForClass( tmp, name, funHandler );
}

/*
 * Zarejestrowanie funkcji zewnetrznej dla wskazanej klasy.
 *
 * Parametry:
 * classname - nazwa klasy
 * name - nazwa funkcji
 * funName - nazwa funkcji
 * libname - nazwa biblioteki
 *
 * Kody powrotu:
 * GOC_ERR_OK - zarejestrowano
 * GOC_ERR_FALSE
 *	TEXT_NOCLASSFOUND - klasy nie znaleziono
 *	TEXT_NOMEMORY - brak pamieci
 *	TEXT_NOLIBLOAD - nie udalo sie zaladowac biblioteki
 *	TEXT_NOFUNLOAD - nie udalo sie zaladowac funkcji
 *	
 */
GOC_FUNCTIONRETURN registerClassExtFunction(const char *classname, const char *name, const char *funName, const char *libname)
{
	GOC_OBJECTCLASS *tmp = NULL;
	void *funHandler = NULL;
	GOC_FUNCTIONRETURN res;
	GOC_EXTLIB *extlib = NULL;

	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	tmp = _pSystemClass[res.result];

	res = hasLibrary(libname);
	if ( res.code == GOC_ERR_OK )
	{
		extlib = _pExternalLib[res.result];
	}
	else
	{
		extlib = (GOC_EXTLIB*)malloc( sizeof(GOC_EXTLIB) );
		if ( extlib == NULL )
			GOC_RET_FALSE( TEXT_NOMEMORY );
		memset(extlib, 0, sizeof(GOC_EXTLIB));
		extlib->handler = dlopen(libname, RTLD_NOW);
		if (extlib->handler == NULL)
			GOC_RET_FALSE( TEXT_NOLIBLOAD );
		extlib->name = goc_stringCopy(extlib->name, libname);
		_pExternalLib = goc_tableAdd(_pExternalLib, &_nExternalLib, sizeof(GOC_EXTLIB*));
		if ( !_pExternalLib )
			GOC_RET_FALSE( TEXT_NOMEMORY );
		_pExternalLib[_nExternalLib-1 ] = extlib;
	}

	funHandler = dlsym(extlib->handler, funName);
	if (funHandler == NULL )
		GOC_RET_FALSE( TEXT_NOFUNLOAD );
	
	return registerFunctionForClass( tmp, name, funHandler );
}

static GOC_FUNCTIONRETURN hasObject(GOC_OBJECTHANDLER handler)
{
	int i;
	for ( i=0; i<_nInstance; i++ )
	{
		if ( _pInstance[i]->handler == handler )
			GOC_RET_RESULT(i);
	}
	GOC_RET_FALSE( TEXT_NOOBJECTFOUND );
}

GOC_FUNCTIONRETURN getObjectClass(GOC_OBJECTHANDLER handler)
{
	GOC_FUNCTIONRETURN res;
	res = hasObject(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	GOC_RET_RESULT( _pInstance[res.result]->class->name );
}

static GOC_FUNCTIONRETURN registerObjectForClass(GOC_OBJECTCLASS *tmp, GOC_OBJECTHANDLER handler, unsigned char flags)
{
	GOC_INSTANCE *instance = NULL;
	if ( hasObject(handler).code == GOC_ERR_OK )
		GOC_RET_FALSE( TEXT_OBJECTEXISTS );

	instance = malloc(sizeof(GOC_INSTANCE));
	if ( instance == NULL )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	memset(instance, 0, sizeof(GOC_INSTANCE));
	instance->handler = handler;
	instance->class = tmp;

	_pInstance = goc_tableAdd( _pInstance, &_nInstance, sizeof(GOC_INSTANCE*) );
	if ( !_pInstance )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	_pInstance[ _nInstance-1 ] = instance;

	// uwzglednienie flagi okreslajacej czy dane sa zewnetrzne/wewnetrzne
	if ( _pExternal == NULL )
		_pExternal = goc_nbitFieldCreate(1, 0);
	if ( flags == GOC_FLAGEXTERNAL )
		goc_nbitFieldAdd(_pExternal, 1);
	else
		goc_nbitFieldAdd(_pExternal, 0);

	GOC_RET_OK;
}

GOC_FUNCTIONRETURN registerObject(GOC_OBJECTHANDLER handler)
{
	GOC_OBJECTCLASS *tmp = NULL;

	if ( !_nSystemClass )
		GOC_RET_FALSE( TEXT_NOCLASSFOUND );
	tmp = _pSystemClass[0];

	return registerObjectForClass(tmp, handler, 0);
}

GOC_FUNCTIONRETURN unregisterObject(GOC_OBJECTHANDLER handler)
{
	GOC_FUNCTIONRETURN res;

	// szukaj obiekt
	res = hasObject(handler);
	if ( res.code != GOC_ERR_OK )
		return res;

	// zwolnij zasoby
	free( _pInstance[res.result] );

	// usun z tablicy instancji
	_pInstance = goc_tableRemove( _pInstance, &_nInstance, sizeof(GOC_INSTANCE*), res.result );
	if ( _nInstance && (!_pInstance) )
		GOC_RET_FALSE( TEXT_NOMEMORY );

	goc_nbitFieldCut( _pExternal, res.result );
	// zwroc rezultat
	GOC_RET_OK;
}

GOC_FUNCTIONRETURN allocExternal(
	const char *classname, void *ptr)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTHANDLER handler = NULL;
	GOC_OBJECTCLASS *class = NULL;

	// odnalezienie klasy
	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	class = _pSystemClass[res.result];

	handler = (GOC_OBJECTHANDLER)ptr;

	// zarejestrowanie obiektu dla danej klasy
	res = registerClassObject( classname, handler, GOC_FLAGEXTERNAL );
	if ( res.code != GOC_ERR_OK )
		return res;
	GOC_RET_RESULT( handler );
}

GOC_FUNCTIONRETURN allocObject(
	const char *classname, void *pBuf, unsigned int nBuf)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTHANDLER handler = NULL;
	GOC_OBJECTCLASS *class = NULL;

	// odnalezienie klasy
	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	class = _pSystemClass[res.result];

	// allokacja zasobow
	res = callpath(class, NULL, "alloc", pBuf, nBuf);
	if ( res.code != GOC_ERR_OK )
		return res;
	handler = (GOC_OBJECTHANDLER)res.result;

	// zarejestrowanie obiektu dla danej klasy
	res = registerClassObject( classname, handler, 0 );
	if ( res.code != GOC_ERR_OK )
	{
		// zwolnienie zasobow
		callpath(class, handler, "free", NULL, 0);
		return res;
	}
	GOC_RET_RESULT( handler );
}

static GOC_FUNCTIONRETURN isObjectExternal(GOC_OBJECTHANDLER handler)
{
	GOC_FUNCTIONRETURN res;
	res = hasObject(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	GOC_RET_RESULT( goc_nbitFieldGet(_pExternal, res.result) );
}

// func zwalniajaca
// to ona zwalnia samo obiekt, wywolanie free dla obiektu ma na celu zwolnienie
// tylko zasobow wewnetrznych tego obiektu
GOC_FUNCTIONRETURN freeObject(GOC_OBJECTHANDLER handler)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTCLASS *class;

	// znalezienie obiektu
	res = hasObject( handler );
	if ( res.code != GOC_ERR_OK )
		return res;

	// ustalenie klasy
	class = _pInstance[res.result]->class;

	// wyrejestrowanie
	res = unregisterObject( handler );
	if ( res.code != GOC_ERR_OK )
		return res;

	// zwolnienie obiektu
	res = callpath(class, handler, "free", NULL, 0);
	if ( res.code != GOC_ERR_OK )
		return res;
	handler = res.result;

	if ( isObjectExternal( handler ).result )
	{
		GOC_RET_RESULT( NULL );
	}
	else
	{
		free( handler );
	}

	GOC_RET_RESULT( handler );
}

GOC_FUNCTIONRETURN reallocObject(GOC_OBJECTHANDLER ptrFrom, GOC_OBJECTHANDLER ptrTo)
{
	GOC_FUNCTIONRETURN res;
	res = hasObject( ptrFrom );
	if ( res.code != GOC_ERR_OK )
		return res;
	_pInstance[res.result]->handler = ptrTo;
	GOC_RET_OK;
}

GOC_FUNCTIONRETURN reallocTableOfObject(GOC_OBJECTHANDLER firstOldElem, GOC_OBJECTHANDLER firstNewElem, int n)
{
	GOC_FUNCTIONRETURN res;
	while ( n-- )
	{
		res = reallocObject(firstOldElem, firstNewElem);
		if ( res.code != GOC_ERR_OK )
			return res;
		firstOldElem += sizeof(GOC_OBJECTHANDLER);
		firstNewElem += sizeof(GOC_OBJECTHANDLER);
	}
	GOC_RET_OK;
}

GOC_FUNCTIONRETURN registerClassObject(const char *classname, GOC_OBJECTHANDLER handler, unsigned char flag)
{
	GOC_OBJECTCLASS *tmp = NULL;
	GOC_FUNCTIONRETURN res;

	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	tmp = _pSystemClass[res.result];

	return registerObjectForClass(tmp, handler, flag);
}

static GOC_FUNCTIONRETURN callpath(
	GOC_OBJECTCLASS *class, GOC_OBJECTHANDLER handler, const char *funname,
	void *pBuf, unsigned int nBuf)
{
	// Sciezka wywolan:
	// 1. Dla danej klasy szukaj funkcje
	// 2. Jesli nie znaleziono punkt 9
	// 3. Jesli znaleziono wywolaj
	// 4. Sprawdz wynik
	// 5. Jesli GOC_ERR_OK lub GOC_ERR_FALSE zwroc wynik
	// 6. Jesli GOC_ERR_PARENT to punkt 13
	// 7. Jesli GOC_ERR_NEXT to punkt 8
	// 8. Przeszukaj dalej klase w poszukiwaniu funkcji -> punkt 2
	// 9. Wywoluj callpath dla kolejndego interfejsu (czyli skok do 1)
	// 10. Jesli zwroci GOC_ERR_OK lub GOC_ERR_FALSE zwroc wynik
	// 11. Jesli zwroci GOC_ERR_NEXT to punkt 8
	// 12. Jesli nie ma wiecej interfejsow to punkt 13
	// 13. callpath dla parenta
	// 14. Jesli zwroci GOC_ERR_OK lub GOC_ERR_FALSE zwroc wynik
	// 15. Zwroc GOC_ERR_NEXT
	// Jezeli z dzialania callpath zostanie zwrocone GOC_ERR_OK lub GOC_ERR_FALSE to jest to wynik operacji
	// Jezeli zostanie zwrocony GOC_ERR_NEXT to znaczy, ze nie znaleziono funkcji
	// Jezeli zwrocono GOC_ERR_PARENT to znaczy, ze cos nie tak
	GOC_FUNCTIONRETURN res;
	int funpos;
	int intpos;

	// szukanie funkcji
	funpos = class->nFunction;
	while ( (res = hasFunction(class, funname, funpos)).code == GOC_ERR_OK )
	{
		GOC_FUNCTIONDEFINE *fun;

		funpos = res.result;
		fun = class->function[res.result]->function;
		res = fun( handler, pBuf, nBuf );
		if ( (res.code == GOC_ERR_OK) || (res.code == GOC_ERR_FALSE) )
			return res;
		if ( res.code == GOC_ERR_NEXT )
			continue;
		if ( res.code == GOC_ERR_PARENT )
		{
			if ( class->parent == NULL )
				return res; // cos nie tak - wywolano ojca, gdy go nie ma
			return callpath(class->parent, handler, funname, pBuf, nBuf);
		}
	}

	// przeszukanie interfejsow
	for (intpos = 0; intpos < class->nInterface; intpos++ )
	{
		res = callpath(class->interface[intpos], handler, funname, pBuf, nBuf);
		if ( (res.code == GOC_ERR_OK) || (res.code == GOC_ERR_FALSE) )
			return res;
		if ( res.code == GOC_ERR_NEXT )
			continue;
	}

	// sprawdzenie rodzica
	if ( class->parent )
	{
		res = callpath(class->parent, handler, funname, pBuf, nBuf);
		if ( (res.code == GOC_ERR_OK) || (res.code == GOC_ERR_FALSE) )
			return res;
	}
	
	GOC_RET_NEXT;
}

GOC_FUNCTIONRETURN __callStatic(const char *classname, const char *functionname, void *pBuf, unsigned int nBuf)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTCLASS *class;
	//GOC_FUNCTIONDEFINE *fun;

	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	class = _pSystemClass[res.result];

	return callpath(class, NULL, functionname, pBuf, nBuf);
}

GOC_FUNCTIONRETURN __call(GOC_OBJECTHANDLER handler, const char *functionname, void *pBuf, unsigned int nBuf)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTCLASS *class;

	if ( (res = hasObject(handler)).code == GOC_ERR_FALSE )
		return res;
	class = _pInstance[res.result]->class;

	return callpath(class, handler, functionname, pBuf, nBuf);
}

GOC_FUNCTIONRETURN callParent(
	GOC_OBJECTHANDLER handler, const char *functionname,
	void *pBuf, unsigned int nBuf)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTCLASS *class;

	if ( (res = hasObject(handler)).code == GOC_ERR_FALSE )
		return res;
	class = _pInstance[res.result]->class;

	if ( class->parent )
		return callpath(
			class->parent, handler, functionname, pBuf, nBuf);
	else
		GOC_RET_FALSE(TEXT_NOCLASSFOUND);
}

GOC_FUNCTIONRETURN callStaticParent(
	const char *classname, const char *functionname,
	void *pBuf, unsigned int nBuf)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTCLASS *class;

	res = hasClass(classname);
	if ( res.code == GOC_ERR_FALSE )
		return res;
	class = _pSystemClass[res.result];

	if ( class->parent )
		return callpath(
			class->parent, NULL, functionname, pBuf, nBuf);
	else
		GOC_RET_FALSE(TEXT_NOCLASSFOUND);
}

// zaladowanie wskazanej biblioteki dynamicznej i wyolanie jej funkcji registerLibrary, ktora powinna zarejestrowac swoje posiadane funkcje i klasy
GOC_FUNCTIONRETURN loadLibrary(const char *libname)
{
	GOC_FUNCTIONRETURN res;
	GOC_EXTLIB *extlib = NULL;
	GOC_FUNCTIONDEFINE *fun = NULL;

	res = hasLibrary(libname);
	if ( res.code == GOC_ERR_OK )
	{
		extlib = _pExternalLib[res.result];
	}
	else
	{
		extlib = (GOC_EXTLIB*)malloc( sizeof(GOC_EXTLIB) );
		if ( extlib == NULL )
			GOC_RET_FALSE( TEXT_NOMEMORY );
		memset(extlib, 0, sizeof(GOC_EXTLIB));
		extlib->handler = dlopen(libname, RTLD_NOW);
		if (extlib->handler == NULL)
			GOC_RET_FALSE( TEXT_NOLIBLOAD );
		extlib->name = goc_stringCopy(extlib->name, libname);
		_pExternalLib = goc_tableAdd(_pExternalLib, &_nExternalLib, sizeof(GOC_EXTLIB*));
		if ( !_pExternalLib )
			GOC_RET_FALSE( TEXT_NOMEMORY );
		_pExternalLib[_nExternalLib-1 ] = extlib;
	}
	fun = dlsym(extlib->handler, "registerLibrary");
	if (fun == NULL )
		GOC_RET_FALSE( TEXT_NOFUNLOAD );
	return fun(0, NULL, 0);
}
