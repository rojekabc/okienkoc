#include <stdio.h>
#include <check.h>
#include <okienkoc/fobclass.h>
#include <okienkoc/fileobject.h>
#include <okienkoc/mystr.h>
#include <okienkoc/plik.h>

// TODO: serializacja i deserializacja obiektow GOC_TABLE

void printResult(GOC_FUNCTIONRETURN ret)
{
	printf("Return: ");
	if ( ret.code == GOC_ERR_OK )
		printf("Ok.");
	else if ( ret.code == GOC_ERR_FALSE )
		printf("False: %s.", ret.description);
	else
		printf("Unknown.");
	printf("\n");
}

#ifdef _DEBUG_
#define checkOk(ret, errMsg) \
{ \
	char *failMsg = NULL; \
	if ( ret.code != GOC_ERR_OK ) \
	{ \
		failMsg = goc_stringAdd(failMsg, ret.file);\
		failMsg = goc_stringAdd(failMsg, ":");\
		failMsg = goc_stringAddInt(failMsg, ret.line);\
		failMsg = goc_stringAdd(failMsg, " - ");\
		failMsg = goc_stringAdd(failMsg, errMsg); \
		failMsg = goc_stringAdd(failMsg, ". Reason: "); \
		failMsg = goc_stringAdd(failMsg, ret.description); \
		fail(failMsg); \
	} \
	failMsg = goc_stringFree( failMsg ); \
}

#define checkFalse(ret, desc, errMsg) \
{ \
	char *failMsg = NULL; \
	if ( ret.code == GOC_ERR_FALSE ) \
	{ \
		if ( ret.description != desc ) \
		{ \
			failMsg = goc_stringAdd(failMsg, ret.file);\
			failMsg = goc_stringAdd(failMsg, ":");\
			failMsg = goc_stringAddInt(failMsg, ret.line);\
			failMsg = goc_stringAdd(failMsg, " - ");\
			failMsg = goc_stringAdd(failMsg, "Unexpected fail: "); \
			failMsg = goc_stringAdd(failMsg, ret.description); \
		} \
	} \
	else \
	{ \
		failMsg = goc_stringAdd(failMsg, errMsg); \
	} \
	if ( failMsg ) \
		fail( failMsg ); \
	failMsg = goc_stringFree( failMsg ); \
}
#else
#define checkOk(ret, errMsg) \
{ \
	char *failMsg = NULL; \
	if ( ret.code != GOC_ERR_OK ) \
	{ \
		failMsg = goc_stringAdd(failMsg, errMsg); \
		failMsg = goc_stringAdd(failMsg, ". Reason: "); \
		failMsg = goc_stringAdd(failMsg, ret.description); \
		fail(failMsg); \
	} \
	failMsg = goc_stringFree( failMsg ); \
}

#define checkFalse(ret, desc, errMsg) \
{ \
	char *failMsg = NULL; \
	if ( ret.code == GOC_ERR_FALSE ) \
	{ \
		if ( ret.description != desc ) \
		{ \
			failMsg = goc_stringAdd(failMsg, "Unexpected fail: "); \
			failMsg = goc_stringAdd(failMsg, ret.description); \
		} \
	} \
	else \
	{ \
		failMsg = goc_stringAdd(failMsg, errMsg); \
	} \
	if ( failMsg ) \
		fail( failMsg ); \
	failMsg = goc_stringFree( failMsg ); \
}
#endif

/*
 * Testowanie rejestrowanie klasy NULL
 */
START_TEST(test_001)
{
	checkOk( registerClass(NULL), "Cannot register NULL class");
/*#ifdef _DEBUG_
	GOC_FUNCTIONRETURN res = registerClass("aaa");
	printf("%s:%d\n", res.file, res.line);
#endif*/
}
END_TEST

/*
 * Testowanie rejestrowania klasy NULL wiecej niz jeden raz
 */
START_TEST(test_002)
{
	int n = 10;
	int i;
	checkOk( registerClass(NULL), "Cannot register NULL class");
	for ( i=0; i<n; i++ )
		checkFalse( registerClass(NULL), TEXT_CLASSEXISTS, "Register twice the same class");
}
END_TEST

/*
 * Testowanie rejestrowania i odrejestrowania klasy NULL
 */
START_TEST(test_003)
{
	checkOk( registerClass(NULL), "Cannot register NULL class");
	checkOk( unregisterClass(NULL), "Cannot unregister NULL class");
}
END_TEST

/*
 * Testowanie rejestrowania i odrejestrowania klasy NULL parokrotnie z mozliwoscia istnienia w systemie, i gdy juz nie istnieje rejestracja
 */
START_TEST(test_004)
{
	checkOk( registerClass(NULL), "Cannot register NULL class");
	checkFalse( registerClass(NULL), TEXT_CLASSEXISTS, "Register twice the same class");
	checkOk( unregisterClass(NULL), "Cannot unregister NULL class");
	checkFalse( unregisterClass(NULL), TEXT_NOCLASSFOUND, "Unregister twice the same class");
}
END_TEST

/*
 * Testowanie rejestrowania klasy nazwanej
 */
START_TEST(test_005)
{
	checkOk( registerClass("alfa"), "Cannot register class");
}
END_TEST

/*
 * Testowanie wyrejestrowania klasy nazwanej wiecej niz jeden raz
 */
START_TEST(test_006)
{
	int n = 10;
	int i;
	checkOk( registerClass("alfa"), "Cannot register class");
	for ( i=0; i<n; i++ )
		checkFalse( registerClass("alfa"), TEXT_CLASSEXISTS, "Register twice the same class");
}
END_TEST

/*
 * Testowanie zarejestrowania i wyrejestrowania klasy nazwanej
 */
START_TEST(test_007)
{
	checkOk( registerClass("alfa"), "Cannot register class");
	checkOk( unregisterClass("alfa"), "Cannot unregister class");
}
END_TEST

/*
 * Testowanie rejestrowania i wyrejestrowania klas nazwanych o roznych, zblizonych nazwach
 * z rownoczesna proba wyrejestrowania klasy, ktora zostala wczesniej wyrejestrowana, lub
 * zatrejestrowania juz zarejestrowanej klasy
 */
START_TEST(test_008)
{
	const char *pClassNames[] = {"alfa", "beta", "Alfa", "Beta"};
	int nClassNames = 4;
	int i, j;

	for ( i=0; i<nClassNames; i++ )
	{
		checkOk( registerClass(pClassNames[i]), "Cannot register class");
		for ( j=0; j<=i; j++ )
			checkFalse( registerClass(pClassNames[j]), TEXT_CLASSEXISTS, "Register twice the same class");
	}
	for ( i=0; i<nClassNames; i++ )
	{
		checkOk( unregisterClass(pClassNames[i]), "Cannot unregister class");
		for ( j=0; j<=i; j++ )
			checkFalse( unregisterClass(pClassNames[j]), TEXT_NOCLASSFOUND, "Unregister twice the same class");
	}
}
END_TEST

/*
 * Testowanie wyrejestrowania clasy, ktora nie byla rejestrowana
 */
START_TEST(test_009)
{
	checkFalse( unregisterClass("alfa"), TEXT_NOCLASSFOUND, "Unregister not existing class" );
}
END_TEST

/*
 * przykladowa func do testow - zwraca GOC_ERR_OK
 */
static GOC_FUNCTIONRETURN exfunok(GOC_OBJECTHANDLER oh, void *pBuf, unsigned int nBuf)
{
	GOC_RET_OK;
}

/*
 * przykladowa func do testow - zwraca GOC_ERR_FALSE z rezultatem null
 */
static GOC_FUNCTIONRETURN exfunfalse(GOC_OBJECTHANDLER oh, void *pBuf, unsigned int nBuf)
{
	GOC_RET_FALSE( NULL );
}

/*
 * Testowanie zarejestrowania i wyrejestrowania funkcji na klasie domyslnej, gdy zarejestrowano juz klase i nie
 * zarejestrowano oraz przy wielokrotnych wystapieniach funkcji i proba wyrejestrowania nadamiarowego
 */
START_TEST(test_010)
{
	checkFalse( registerFunction("func", exfunok ), TEXT_NOCLASSFOUND, "Cannot register function" );
	checkFalse( unregisterFunction("func" ), TEXT_NOCLASSFOUND, "Unregister nonexisted function" );
	checkOk( registerClass( "alfa" ), "Cannot register class" );
	checkFalse( unregisterFunction("func" ), TEXT_NOFUNCTIONFOUND, "Unregister nonexisted function" );
	checkOk( registerFunction("func", exfunok ), "Cannot register function" );
	checkOk( registerFunction("func", exfunok ), "Cannot register function" );
	checkOk( unregisterFunction("func" ), "Cannot unregister function" );
	checkOk( unregisterFunction("func" ), "Cannot unregister function" );
	checkFalse( unregisterFunction("func" ), TEXT_NOFUNCTIONFOUND, "Unregister nonexisted function" );
	checkOk( unregisterClass( "alfa" ), "Cannot unregister class" );
	checkFalse( unregisterFunction("func" ), TEXT_NOCLASSFOUND, "Unregister nonexisted function" );
}
END_TEST

/*
 * Testowanie rejestrowania i wyrejestrowania funkcji zewnetrznej.
 * Proba rejestrowania, gdy klasa nie isnieje.
 * Proba rejestrowania na nieistniejaca biblioteke i funkcje biblioteki
 */
START_TEST(test_011)
{
	checkFalse( registerClassExtFunction( "alfa", "func", "testfunok", "./libtestlib.so" ), TEXT_NOCLASSFOUND,
		"Register external function without class" );
	checkOk( registerClass( "alfa" ), "Cannot register class" );
	checkFalse( registerClassExtFunction( "alfa", "func", "testfunok", "./libnotestlib.so" ), TEXT_NOLIBLOAD,
		"Loaded not existed library" );
	checkOk( registerClassExtFunction( "alfa", "func", "testfunok", "./libtestlib.so" ), "Cannot register external function" );
	checkFalse( registerClassExtFunction( "alfa", "func", "testfunnoexist", "./libtestlib.so" ), TEXT_NOFUNLOAD,
		"Loaded not existed function" );
	checkOk( unregisterClassFunction( "alfa", "func" ), "Cannot unregister function" );
	checkOk( unregisterClass( "alfa" ), "Cannot unregister class" );
	checkFalse( registerClassExtFunction( "alfa", "func", "testfunok", "./libtestlib.so" ), TEXT_NOCLASSFOUND,
		"Register external function without class" );
}
END_TEST

/*
 * Testowanie rejestrowania i wyrejestrowywania obiektow dla klasy domyslnej.
 */
START_TEST(test_012)
{
	checkFalse( registerObject( (GOC_OBJECTHANDLER)5 ), TEXT_NOCLASSFOUND, "Registered object without class" );
	checkOk( registerClass( "alfa" ), "Cannot register named class" );
	checkOk( registerObject( (GOC_OBJECTHANDLER)5 ), "Cannot register object for default class" );
	checkFalse( registerObject( (GOC_OBJECTHANDLER)5 ), TEXT_OBJECTEXISTS, "Cannot register the same object twice" );
	checkFalse( registerObject( (GOC_OBJECTHANDLER)5 ), TEXT_OBJECTEXISTS, "Cannot register the same object twice" );
	checkOk( registerObject( (GOC_OBJECTHANDLER)6 ), "Cannot register object for default class" );
	checkFalse( registerObject( (GOC_OBJECTHANDLER)5 ), TEXT_OBJECTEXISTS, "Cannot register the same object twice" );
	checkFalse( registerObject( (GOC_OBJECTHANDLER)6 ), TEXT_OBJECTEXISTS, "Cannot register the same object twice" );
}
END_TEST

/*
 * Testowanie wywolywania funkcji wewnetrznych podlaczonych do klasy - sciezka podstawowa
 */
START_TEST(test_013)
{
	checkOk( registerClass( "alfa" ), "Cannot register named class" );
	checkOk( registerClassFunction( "alfa", "ok", exfunok ), "Cannot register function" );
	checkOk( registerClassFunction( "alfa", "false", exfunfalse ), "Cannot register function" );
	checkOk( registerObject( (GOC_OBJECTHANDLER)1 ), "Cannot register object" );
	checkOk( call( (GOC_OBJECTHANDLER)1, "ok", NULL, 0 ), "Function return wrong result" );
	checkFalse( call( (GOC_OBJECTHANDLER)1, "false", NULL, 0 ), NULL, "Function return wrong result" );
}
END_TEST

/*
 * Testowanie wywolywania funkcji zewnetrznych podlaczonych do klasy - sciezka podstawowa
 */
START_TEST(test_014)
{
	checkOk( registerClass( "alfa" ), "Cannot register named class" );
	checkOk( registerClassExtFunction( "alfa", "ok", "testfunok", "./libtestlib.so" ), "Cannot register function" );
	checkOk( registerClassExtFunction( "alfa", "false", "testfunfalse", "./libtestlib.so" ), "Cannot register function" );
	checkOk( registerObject( (GOC_OBJECTHANDLER)1 ), "Cannot register object" );
	checkOk( call( (GOC_OBJECTHANDLER)1, "ok", NULL, 0 ), "Function return wrong result" );
	checkFalse( call( (GOC_OBJECTHANDLER)1, "false", NULL, 0 ), NULL, "Function return wrong result" );
}
END_TEST

/*
 * Szybkie testy na FileObject
 */
START_TEST(test_015)
{
	GOC_FILEOBJECT *fob = NULL;
	fileObjectInit();
	fob = fileObjectCreate();
	if ( fob == NULL )
		fail("FileObject not created");
	fob = fileObjectDestroy(fob);
	if ( fob != NULL )
		fail("FileObject not removed");
}
END_TEST

START_TEST(test_016)
{
	GOC_FILEOBJECT *fob = NULL;
	fileObjectInit();
	fob = fileObjectCreate();
	if ( fob == NULL )
		fail("FileObject not created");

//	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), NULL ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectSetAttribute( fob, "alfa", "alfa");
//	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa"  ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectSetAttribute( fob, "beta", "beta");
	fileObjectSetAttribute( fob, "gamma", allocObject("Integer", 12, 0).result );
//	fileObjectSerialize( fob, stdout );
	writeObjectToFile( "plik.txt", fob );

	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), "beta" ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa" ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectSetAttribute( fob, "beta", "Beta");
//	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), "Beta" ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa" ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectRemoveAttribute( fob, "beta");
//	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa" ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectRemoveAttribute( fob, "alfa");
//	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), NULL ) )
		fail("FileObject attribute alfa not correctly setted");

	fob = fileObjectDestroy(fob);
	if ( fob != NULL )
		fail("FileObject not removed");

}
END_TEST

int cosik(GOC_OBJECTHANDLER oh, void *p)
{
	FILE *fout = fopen("/dev/null", "w");
	fileObjectSerialize( (GOC_FILEOBJECT *)oh, fout );
	fclose( fout );
	return 1;
}

START_TEST(test_017)
{
	GOC_FILEOBJECT *fob = NULL;
	fileObjectInit();
	listObjectsInFile( "plik.txt", cosik, NULL );
}
END_TEST

int test_018_fun(const char *classname, void *node, void *arg)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTHANDLER handler;
	if ( goc_stringEquals(classname, CN_STRING) )
	{
		res = allocObject(CN_STRING, NULL, 0);
		checkOk( res, "Nie utworzono obiektu CN_STRING" );
		handler = res.result;
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		res = call( handler, "get", NULL, 0 );
		checkOk( res, "Nieudane pobieranie" );
		if ( !goc_stringEquals( res.result, "lancuch znakow" ) )
			fail("Zdeserializowany ciag znakow jest niezgodny");
		res = freeObject(handler);
		checkOk(res, "Problem podczas zwalniania");
	}
	else if ( goc_stringEquals(classname, CN_INTEGER) )
	{
		res = allocObject(CN_INTEGER, 0, 0);
		checkOk( res, "Nie utworzono obiektu CN_INTEGER" );
		handler = res.result;
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		res = call( handler, "get", NULL, 0 );
		checkOk( res, "Nieudane pobieranie" );
		if ( res.result != 123 )
			fail("Zdeserializowana wartosc Integer jest niezgodna");
		res = freeObject(handler);
		checkOk(res, "Problem podczas zwalniania");
	}
	else if ( goc_stringEquals(classname, CN_FILEOBJECT) )
	{
		res = allocObject(CN_FILEOBJECT, 0, 0);
		checkOk( res, "Nie utworzono obiektu CN_FILEOBJECT" );
		handler = res.result;
		call( handler, "set", "pocopol", allocObject(CN_STRING, NULL, 0).result );
		call( handler, "set", "pisak", allocObject(CN_INTEGER, 0, 0).result );
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		//call( handler, "serialize", stdout, 0 );
		if ( !goc_stringEquals(
			call( call( handler, "get", "pocopol", 0 ).result, "get", 0, 0).result,
			"lancuch znakow") )
			fail("Nieudana deserializacja wartosci pocopol");
		if ( call( call( handler, "get", "pisak", 0 ).result, "get", 0, 0).result != 1212 )
			fail("Nieudana deserializacja wartosci pisak");
		if ( !goc_stringEquals(fileObjectGetAttribute(handler, "pocopol"), "lancuch znakow") )
			fail("Nieudane pobieranie przy pomocy fileObjectGetattribute");
	}
	else if ( goc_stringEquals(classname, CN_TABLE ) )
	{
		res = allocObject(CN_TABLE, 0, 0);
		checkOk( res, "Nie utworzono obiektu CN_TABLE" );
		handler = res.result;
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		res = freeObject(handler);
		checkOk(res, "Problem podczas zwalniania");
	}
	else
	{
		fail("I don recogni thi class");
	}
}

// test na serializacje, w ktorej przekazywany jest obiekt do wypelnienia
// seializowanym elementem
START_TEST(test_018)
{
	const char *testfile = "test-fob-018.txt";
	GOC_FUNCTIONRETURN res;

	GOC_OBJECTHANDLER handlerString;
	GOC_OBJECTHANDLER handlerInt;
	GOC_OBJECTHANDLER handlerFileObject;
	GOC_OBJECTHANDLER handlerTable;

	fileObjectInit();
	if ( !goc_isFileExists(testfile) )
	{
		// wygenerowanie danych testowych
		res = allocObject(CN_STRING, "lancuch znakow", 0);
		checkOk( res, "Nie utworzono obiektu CN_STRING" );
		handlerString = res.result;
		res = call( handlerString, "get", NULL, 0);
		checkOk( res, "Nie udalo sie odczytac obiektu CN_STRING" );
		if ( !goc_stringEquals(res.result, "lancuch znakow") )
			fail("Nierowne ciagi znakow");
		writeObjectToFile( testfile, handlerString );
		handlerString = freeObject( handlerString ).result;

		res = allocObject(CN_INTEGER, 123, 0);
		checkOk( res, "Nie utworzono obiektu CN_INTEGER" );
		handlerInt = res.result;
		res = call( handlerInt, "get", NULL, 0);
		checkOk( res, "Nie udalo sie odczytac obiektu CN_INTEGER" );
		if ( res.result != 123 )
			fail("Nierowne wartosci int");
		writeObjectToFile( testfile, handlerInt );
		handlerInt = freeObject( handlerInt ).result;

		res = allocObject(CN_FILEOBJECT, NULL, 0);
		handlerFileObject = res.result;
		call( handlerFileObject, "set", "pocopol", allocObject(CN_STRING, "lancuch znakow", 0).result );
		call( handlerFileObject, "set", "pisak", allocObject(CN_INTEGER, 1212, 0).result );
		writeObjectToFile( testfile, handlerFileObject );
		handlerFileObject = freeObject( handlerFileObject ).result;

		res = allocObject(CN_TABLE, NULL, 0);
		checkOk( res, "Nie utworzono CN_TABLE" );
		handlerTable = res.result;
		call( handlerTable, "add", goc_stringCopy(NULL, "burak"), CN_STRING );
		call( handlerTable, "add", goc_stringCopy(NULL, "origami"), CN_STRING );
		call( handlerTable, "add", goc_stringCopy(NULL, "gaz"), CN_STRING );
		writeObjectToFile( testfile, handlerTable );
		handlerTable = freeObject( handlerTable ).result;
	}
	deserializeObjectsInFile( testfile, test_018_fun, NULL );
}
END_TEST

int test_019_fun(const char *classname, void *node, void *arg)
{
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTHANDLER handler;
	if ( goc_stringEquals(classname, CN_STRING) )
	{
		char *extStr = NULL;
		res = allocExternal(CN_STRING, &extStr);
		checkOk( res, "Nie utworzono obiektu CN_STRING" );
		handler = res.result;
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		res = call( handler, "get", NULL, 0 );
		checkOk( res, "Nieudane pobieranie" );
		if ( !goc_stringEquals( res.result, "lancuch znakow" ) )
			fail("Zdeserializowany ciag znakow jest niezgodny (pobrany wewnetrznie)");
		if ( !goc_stringEquals( extStr, "lancuch znakow" ) )
			fail("Zdeserializowany ciag znakow jest niezgodny (pobrany zewnetrznie)");
		res = freeObject(handler);
		checkOk(res, "Problem podczas zwalniania");
	}
	else if ( goc_stringEquals(classname, CN_INTEGER) )
	{
		int extInt = 0;
		res = allocExternal(CN_INTEGER, &extInt);
		checkOk( res, "Nie utworzono obiektu CN_INTEGER" );
		handler = res.result;
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		res = call( handler, "get", NULL, 0 );
		checkOk( res, "Nieudane pobieranie" );
		if ( res.result != 123 )
			fail("Zdeserializowana wartosc Integer jest niezgodna (pobrany wewnetrznie)");
		if ( extInt != 123 )
			fail("Zdeserializowana wartosc Integer jest niezgodna (pobrany zewnetrznie)");
		res = freeObject(handler);
		checkOk(res, "Problem podczas zwalniania");
	}
	else if ( goc_stringEquals(classname, CN_FILEOBJECT) )
	{
		char *extStr = NULL;
		int extInt = 0;

		res = allocObject(CN_FILEOBJECT, 0, 0);
		checkOk( res, "Nie utworzono obiektu CN_FILEOBJECT" );
		handler = res.result;
		call( handler, "set", "pocopol", allocExternal(CN_STRING, &extStr).result );
		call( handler, "set", "pisak", allocExternal(CN_INTEGER, &extInt).result );
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		//call( handler, "serialize", stdout, 0 );
		if ( !goc_stringEquals(
			call( call( handler, "get", "pocopol", 0 ).result, "get", 0, 0).result,
			"lancuch znakow") )
			fail("Nieudana deserializacja wartosci pocopol (pobranie wewnetrzne)");
		if ( call( call( handler, "get", "pisak", 0 ).result, "get", 0, 0).result != 1212 )
			fail("Nieudana deserializacja wartosci pisak (pobranie wewnetrzne)");
		if ( !goc_stringEquals(extStr, "lancuch znakow") )
			fail("Nieudana deserializacja wartosci pocopol (pobranie zewnetrzne)");
		if ( extInt != 1212 )
			fail("Nieudana deserializacja wartosci pisak (pobranie zewnetrzne)");
		res = freeObject(handler);
		checkOk(res, "Problem podczas zwalniania");
	}
	else if ( goc_stringEquals(classname, CN_TABLE ) )
	{
		struct
		{
			char **pStrings;
			int nStrings;
		} extTable;
		memset(&extTable, 0, sizeof(extTable));
		res = allocExternal(CN_TABLE, &extTable);
		checkOk( res, "Nie utworzono obiektu CN_TABLE" );
		handler = res.result;
		res = call( handler, "deserialize", node, 0 );
		checkOk( res, "Deserializacja nieudana" );
		if ( extTable.nStrings != 3 )
			fail("Zly rozmiar tablicy po deserializacji");
		if ( !goc_stringEquals( extTable.pStrings[0], "niebo" ) )
			fail("Nie znaleziono nieba");
		if ( !goc_stringEquals( extTable.pStrings[1], "ziemia" ) )
			fail("Nie znaleziono ziemi");
		if ( !goc_stringEquals( extTable.pStrings[2], "pieklo" ) )
			fail("Nie znaleziono piekla");
		res = freeObject(handler);
		checkOk(res, "Problem podczas zwalniania");
	}
	else
	{
		fail("I don recogni thi class");
	}
}
// test na wykonywanie operacji na obiekcie zewnetrznym od storny mechanizmu i programu
// serializacja i deserializacja, niszczenie, zakladanie
START_TEST(test_019)
{
	const char *testfile = "test-fob-019.txt";
	GOC_FUNCTIONRETURN res;

	GOC_OBJECTHANDLER handlerString;
	GOC_OBJECTHANDLER handlerInt;
	GOC_OBJECTHANDLER handlerFileObject;
	GOC_OBJECTHANDLER handlerTable;

	fileObjectInit();
	if ( !goc_isFileExists(testfile) )
	{
		// obiekty zewnetrzne
		char *extStr = goc_stringCopy(NULL, "lancuch znakow");
		int extInt = 123;
		char *extFStr = goc_stringCopy(NULL, "lancuch znakow");
		int extFInt = 1212;
		struct
		{
			char **pStrings;
			int nStrings;
		} extTable;
	
		// wygenerowanie danych testowych
		res = allocExternal(CN_STRING, &extStr);
		checkOk( res, "Nie utworzono obiektu CN_STRING" );
		handlerString = res.result;
		res = call( handlerString, "get", NULL, 0);
		checkOk( res, "Nie udalo sie odczytac obiektu CN_STRING" );
		if ( !goc_stringEquals(res.result, "lancuch znakow") )
			fail("Nierowne ciagi znakow");
		writeObjectToFile( testfile, handlerString );
		handlerString = freeObject( handlerString ).result;

		res = allocExternal(CN_INTEGER, &extInt);
		checkOk( res, "Nie utworzono obiektu CN_INTEGER" );
		handlerInt = res.result;
		res = call( handlerInt, "get", NULL, 0);
		checkOk( res, "Nie udalo sie odczytac obiektu CN_INTEGER" );
		if ( res.result != 123 )
			fail("Nierowne wartosci int");
		writeObjectToFile( testfile, handlerInt );
		handlerInt = freeObject( handlerInt ).result;

		res = allocObject(CN_FILEOBJECT, NULL, 0);
		handlerFileObject = res.result;
		call( handlerFileObject, "set", "pocopol", allocExternal(CN_STRING, &extFStr).result );
		call( handlerFileObject, "set", "pisak", allocExternal(CN_INTEGER, &extFInt).result );
		writeObjectToFile( testfile, handlerFileObject );
		handlerFileObject = freeObject( handlerFileObject ).result;

		memset(&extTable, 0, sizeof(extTable));
		res = allocExternal(CN_TABLE, &extTable);
		checkOk( res, "Nie utworzono obiektu CN_TABLE" );
		handlerTable = res.result;
		res = call( handlerTable, "add", strdup("niebo"), CN_STRING );
		checkOk( res, "Nie udalo sie dodac nieba" );
		res = call( handlerTable, "add", strdup("ziemia"), CN_STRING );
		checkOk( res, "Nie udalo sie dodac ziemi" );
		res = call( handlerTable, "add", strdup("pieklo"), CN_STRING );
		checkOk( res, "Nie udalo sie dodac piekla" );
		writeObjectToFile( testfile, handlerTable );

	}
	deserializeObjectsInFile( testfile, test_018_fun, NULL );
	deserializeObjectsInFile( testfile, test_019_fun, NULL );
}
END_TEST

START_TEST(test_020)
{
	// test na zewnetrzna tablice
	GOC_FUNCTIONRETURN res;
	struct
	{
		char **pStrings;
		int nStrings;
	} myTable;
	GOC_OBJECTHANDLER handler;
	FILE *plik = fopen("/dev/null", "wb");
	char **old = NULL;

	memset( &myTable, 0, sizeof(myTable) );

	fileObjectInit();
	res = allocExternal(CN_TABLE, &myTable);
	checkOk(res, "Nie zaalokowano tablicy");
	handler = res.result;
	if (( myTable.nStrings != 0 ) || ( myTable.nStrings != NULL ))
		fail("Tablica powinna miec rozmiar zerowy");
	checkOk(call( handler, "add", goc_stringCopy( NULL, "alfa" ), CN_STRING ), "Nie dodano elementu");
	checkOk(call( handler, "serialize", plik, 0), "Blad serializacji");
	if (( myTable.nStrings != 1 ) || ( !goc_stringEquals( "alfa", myTable.pStrings[myTable.nStrings-1] ) ))
		fail("Blad dodania ciagu");
	old = &(myTable.pStrings[0]);
	myTable.pStrings = goc_tableAdd(myTable.pStrings, &myTable.nStrings, sizeof(char*));
	checkOk( reallocTableOfObject( old, &(myTable.pStrings[0]), myTable.nStrings-1 ), "Blad reallokacji" );
	myTable.pStrings[ myTable.nStrings-1 ] = goc_stringCopy(NULL, "beta");
	allocExternal(CN_STRING, &(myTable.pStrings[ myTable.nStrings-1 ]));
	if (( myTable.nStrings != 2 ) || ( !goc_stringEquals( "beta", myTable.pStrings[myTable.nStrings-1] ) ))
		fail("Blad dodania ciagu");
	checkOk(call( handler, "serialize", plik, 0), "Blad serializacji");
	checkOk(freeObject(handler), "Nieudane zwalnianie tablicy");
	if (( myTable.nStrings != 0 ) || ( myTable.nStrings != NULL ))
		fail("Tablica powinna miec rozmiar zerowy");
	fclose( plik );
}
END_TEST


// Testy sa niezalezne i sa uruchamiane jako osobne procesy poprzez fork - cale srodowisko jest wiec nowe
int main(int argc, char **argv)
{
	fprintf(stdout, "strat");
	Suite *s;
	TCase *tc_core;
	SRunner *sr;
	int number_failed;
	int runnerOpt = CK_NORMAL;

	if ( argc > 1 )
	{
		int i;
		for ( i=0; i<argc; i++ )
		{
			if ( goc_stringEquals(argv[i], "-verbose") )
				runnerOpt = CK_VERBOSE;
		}
	}

	s = suite_create("Suite Class");

	// register/unregister class
	tc_core = tcase_create ("Register/Unregister Class");
	tcase_add_test (tc_core, test_001);
	tcase_add_test (tc_core, test_002);
	tcase_add_test (tc_core, test_003);
	tcase_add_test (tc_core, test_004);

	tcase_add_test (tc_core, test_005);
	tcase_add_test (tc_core, test_006);
	tcase_add_test (tc_core, test_007);
	tcase_add_test (tc_core, test_008);
	tcase_add_test (tc_core, test_009);
	suite_add_tcase (s, tc_core);
	
	// register/unregister function
	tc_core = tcase_create ("Register/Unregister Function");
	tcase_add_test (tc_core, test_010);
	tcase_add_test (tc_core, test_011);
	suite_add_tcase (s, tc_core);

	// register/unregister object
	tc_core = tcase_create ("Register/Unregister Object");
	tcase_add_test (tc_core, test_012);
	suite_add_tcase (s, tc_core);

	// call
	tc_core = tcase_create ("Call Functions");
	tcase_add_test (tc_core, test_013);
	tcase_add_test (tc_core, test_014);
	suite_add_tcase (s, tc_core);

	// fileobject
	tc_core = tcase_create ("FileObject");
	tcase_add_test (tc_core, test_015);
	tcase_add_test (tc_core, test_016);
	tcase_add_test (tc_core, test_017);
	suite_add_tcase (s, tc_core);

	// serialize/deserialize
	tc_core = tcase_create ("serialize/deserialize");
	tcase_add_test (tc_core, test_018);
	suite_add_tcase (s, tc_core);

	// external objects
	tc_core = tcase_create ("external object");
	tcase_add_test (tc_core, test_019);
	tcase_add_test (tc_core, test_020);
	suite_add_tcase (s, tc_core);

        sr = srunner_create (s);
        srunner_run_all (sr, runnerOpt);
        number_failed = srunner_ntests_failed (sr);
        srunner_free (sr);
	return 0;
}
