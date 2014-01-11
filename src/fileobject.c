#include "global-inc.h"
#include "tablica.h"
#include "mystr.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include <libxml/parser.h>
#include "fileobject.h"
#include "fobclass.h"

#define FILEOBJECTFUNCTION(name) static GOC_FUNCTIONRETURN name(GOC_OBJECTHANDLER handler, void *pBuf, unsigned int uBuf)

const char *CN_FILEOBJECT = "FileObject";
const char *CN_STRING = "String";
const char *CN_TABLE = "Table";
const char *CN_INTEGER = "Integer";
const char *CN_POINTER = "Pointer";
const char *CN_OBJECTTABLE = "ObjectTable";

// Ustalenia:
// func alloc zwraca wskazanie na utworzony obiekt
// func free zwalnia zasoby wewnetrzne obiektu, nie zwalnia samego obiektu !
// funckja set zwraca wskazanie na podany handler wejsciowy, ktory ulegl zmianie

// w deserialize jako pBuf jest podawany uchwyt do struktury XML
// jezeli w deserialize zostanie okreslony handler, to znaczy, ze podano uchwyt zewnetrzny dla serializowanych danych
// i nalezy wykonac do niego przypisanie serializowanej informacji

// Klasa Obiektu: Sring
FILEOBJECTFUNCTION(String_alloc)
{
	char **tmp = NULL;
	tmp = (char **)malloc(sizeof(char*));
	if ( tmp == NULL )
		GOC_RET_FALSE(TEXT_NOMEMORY);
	*tmp = NULL;

	if ( pBuf == NULL )
		GOC_RET_RESULT( tmp );

	// przydzielanie zasobow
	*tmp = goc_stringCopy( *tmp, pBuf );
	if ( *tmp == NULL )
		GOC_RET_FALSE(TEXT_NOMEMORY);

	// zwracanie obiektu
	GOC_RET_RESULT( tmp );
}

FILEOBJECTFUNCTION(String_free)
{
	char **tmp = handler;
	// zwolnienie zasobu
	*tmp = goc_stringFree( *tmp );
//	free( tmp );
	GOC_RET_RESULT(NULL);
}

FILEOBJECTFUNCTION(String_get)
{
	char **tmp = handler;
	GOC_RET_RESULT( *tmp );
}

// w handler podano char* z poprzednim stringiem (lub NULL, jesli nie jest zarejestrowany)
// w pBuf podano const char* ze stringiem do ustawienia
FILEOBJECTFUNCTION(String_set)
{
	GOC_FUNCTIONRETURN res;
	char **tmp = (char **)handler;
	*tmp = goc_stringCopy( *tmp, pBuf );
	res.result = tmp;
	GOC_RET_RESULT(res.result);
}

FILEOBJECTFUNCTION(String_isString)
{
	GOC_FUNCTIONRETURN res;
	res = getObjectClass( handler );
	if ( res.code != GOC_ERR_OK )
		return res;
	if ( goc_stringEquals(res.description, CN_STRING) )
		GOC_RET_OK;
	GOC_RET_FALSE(NULL);
}

FILEOBJECTFUNCTION(String_serialize)
{
	char *str;

	str = *((char **)handler);
	if ( str )
		fprintf(pBuf, "%s", str);
	GOC_RET_OK;
}

FILEOBJECTFUNCTION(String_deserialize)
{
	char* str = NULL;
	xmlNode *node = pBuf;
	xmlNode *child = NULL;
	GOC_FUNCTIONRETURN res;
	// odczytanie zawartosci
	for ( child = node->children; child; child = child->next )
	{
		if ( child->type != XML_TEXT_NODE )
			continue;
		str = goc_stringAdd(
			str, (const char *)xmlNodeGetContent(child) );
	}
	if ( handler )
		// ustawienie istniejacego wskazania
		res = call( handler, "set", str, 0 );
	else
		// zarejestrowanie wskaznika jako ciag znakow
		res = allocObject( CN_STRING, str, 0 );
	str = goc_stringFree( str );
	if ( res.code != GOC_ERR_OK )
	{
		return res;
	}
	// zwrocenie rezultatu
	GOC_RET_RESULT( res.result );
}

// handler - GOC_OBJECTHANDLER
// pBuf - GOC_OBJECTHANDLER, char *
FILEOBJECTFUNCTION(String_equals)
{
	GOC_RET_RESULT( goc_stringEquals( *((char **)handler), *((char **)pBuf) ) );
}

FILEOBJECTFUNCTION(String_copy)
{
	GOC_FUNCTIONRETURN res;
	const char *classname;

	res = getObjectClass(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	classname = (const char *)res.result;

	if ( pBuf )
		return call( pBuf, "set", call( handler, "get", NULL, 0 ).result, 0 );
	else
		return allocObject(classname, *((char**)handler), 0);
}

// rejestrowanie wskazania na obiekt danego typu
// pBuf - wskazanie
// handler - zostanie zalozony
// uwaga - zarejestrowany obiekt podlega niszczeniu przez klase
FILEOBJECTFUNCTION(String_registerPtr)
{
	char **tmp = NULL;
	tmp = (char **)malloc(sizeof(char*));
	if ( tmp == NULL )
		GOC_RET_FALSE(TEXT_NOMEMORY);
	*tmp = NULL;

	if ( pBuf == NULL )
		GOC_RET_RESULT( tmp );

	// przydzielanie zasobow
	tmp = 
	*tmp = goc_stringCopy( *tmp, pBuf );
	if ( *tmp == NULL )
		GOC_RET_FALSE(TEXT_NOMEMORY);

	// zwracanie obiektu
	GOC_RET_RESULT( tmp );
}


// Klasa obiektu: Integer
FILEOBJECTFUNCTION(Integer_alloc)
{
	int *tmp = NULL;

	// przydzielenie pamieci
	tmp = malloc(sizeof(int));
	if ( tmp == NULL )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	*tmp = (int)pBuf;

	// zwrocenie obiektu
	GOC_RET_RESULT(tmp);
}

FILEOBJECTFUNCTION(Integer_free)
{
	GOC_RET_RESULT( NULL );
/*	int* object = (int*)handler;

	if ( object == NULL )
		GOC_RET_RESULT( handler );
	free( object );

	GOC_RET_RESULT( NULL );*/
}

FILEOBJECTFUNCTION(Integer_set)
{
	int* object = (int*)handler;
	*object = (int)pBuf;
	GOC_RET_RESULT( handler );
}

FILEOBJECTFUNCTION(Integer_get)
{
	int* object = (int*)handler;
	GOC_RET_RESULT( *object );
}

FILEOBJECTFUNCTION(Integer_serialize)
{
	fprintf(pBuf, "%d", *((int*)handler));
	GOC_RET_OK;
}

FILEOBJECTFUNCTION(Integer_deserialize)
{
	xmlNode *node = pBuf;
	xmlNode *child = NULL;
	char *tmpStr = NULL;
	GOC_FUNCTIONRETURN res;

	// odczytanie zawartosci
	for ( child = node->children; child; child = child->next )
	{
		if ( child->type != XML_TEXT_NODE )
			continue;
		tmpStr = xmlNodeGetContent(child);
		break;
	}

	if ( handler )
	{
		if ( tmpStr )
			res = call(handler, "set", atoi(tmpStr), 0);
		else
			res = call(handler, "set", 0, 0);
	}
	else
	{
		if ( tmpStr )
			res = allocObject(CN_INTEGER, atoi(tmpStr), 0);
		else
			res = allocObject(CN_INTEGER, 0, 0);
	}

	if ( res.code != GOC_ERR_OK )
		return res;
		
	// zwrocenie rezultatu
	GOC_RET_RESULT( res.result );
}

FILEOBJECTFUNCTION(Integer_equals)
{
	if ( handler != pBuf )
	{
		int *a = handler;
		int *b = pBuf;
		GOC_RET_RESULT( *a == *b );
	}
	else
		GOC_RET_RESULT( 1 );
}

FILEOBJECTFUNCTION(Integer_copy)
{
	GOC_FUNCTIONRETURN res;
	const char *classname;

	res = getObjectClass(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	classname = (const char *)res.result;

	if ( pBuf )
		return call( pBuf, "set", call( handler, "get", NULL, 0 ).result, 0 );
	else
		return allocObject(classname, *((int*)handler), 0);
}

// Klasa Obiektu: FileObject
FILEOBJECTFUNCTION(FileObject_alloc)
{
	GOC_FILEOBJECT *tmp = NULL;

	// przydzielenie pamieci
	tmp = (GOC_FILEOBJECT*)malloc(sizeof(GOC_FILEOBJECT));
	if ( tmp == NULL )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	memset(tmp, 0, sizeof(GOC_FILEOBJECT));

	// zwrocenie obiektu
	GOC_RET_RESULT(tmp);
}

FILEOBJECTFUNCTION(FileObject_free)
{
	GOC_FILEOBJECT* object = (GOC_FILEOBJECT*)handler;
	int i;
	if ( object == NULL )
		GOC_RET_RESULT( handler );
	for (i=0; i<object->attrAmmount; i++)
	{
		goc_stringFree(object->attrName[i]);
		freeObject( object->attrValue[i] );
	}
	object->attrName = goc_tableClear(
		object->attrName, &(object->attrAmmount));
	object->attrAmmount = i;
	object->attrValue = goc_tableClear(
		object->attrValue, &(object->attrAmmount));
//	free( object );

	GOC_RET_RESULT( NULL );
}

// moze zostac podany obiekt jakiejkolwiek klasy, jezeli nie, to zostaje uznane, ze podano obiekt
// typu String i do takiego nastepuje przeksztalcenie
// Obiekt ustawiany nie jest kopiowany (moze bedzie, ale teraz nie)
// Jako pBuf podano nazwe atrybutu
// Jako uBuf podano przypisywany obiekt
FILEOBJECTFUNCTION(FileObject_set)
{
	GOC_FUNCTIONRETURN res;
	int i;
	GOC_FILEOBJECT* object = (GOC_FILEOBJECT*)handler;

	if ( handler == NULL )
		GOC_RET_RESULT(NULL);

	// pobierz pozycje juz istniejacego elementu
	i = fileObjectGetAttributePos( object, pBuf );

	// zwolnij poprzedni zasob
	if ( i != -1 )
	{
		res = freeObject( object->attrValue[i] );
		if ( res.code != GOC_ERR_OK )
			return res;
	}

	// czy podawany element posiada okreslona klase
	res = getObjectClass( (void*)uBuf );
	if ( res.code == GOC_ERR_FALSE )
	{
		// jesli inny blad niz nierozpoznany typ
		if ( res.description != TEXT_NOOBJECTFOUND )
		{
			return 	res;
		}
		else
		{
			// ustaw obecna wartosc - nadajac jej typ string
			res = allocObject(CN_STRING, uBuf, 0);
			if ( res.code != GOC_ERR_OK )
				return res;
			uBuf = res.result;
		}

	}


	if ( i == -1 )
	{
		// nie znaleziono - zak³adanie od nowa
		object->attrName = goc_tableAdd(
			object->attrName, &(object->attrAmmount),
			sizeof(char*));
		object->attrAmmount--;
		object->attrName[object->attrAmmount] =
			goc_stringCopy(NULL, pBuf);
		object->attrValue = goc_tableAdd(
			object->attrValue, &(object->attrAmmount),
			sizeof(GOC_OBJECTHANDLER));
		i = object->attrAmmount - 1;
		object->attrValue[i] = NULL;
	}

	object->attrValue[i] = (GOC_OBJECTHANDLER)uBuf;

	GOC_RET_RESULT(handler);
}
FILEOBJECTFUNCTION(FileObject_get)
{
	int i;
	GOC_FILEOBJECT* object = (GOC_FILEOBJECT*)handler;

	if ( object == NULL)
		GOC_RET_RESULT( NULL );

	for (i=0; i<object->attrAmmount; i++)
	{
		if ( goc_stringEquals( object->attrName[i], pBuf ) )
			GOC_RET_RESULT( object->attrValue[i] );
	}
	GOC_RET_RESULT( NULL );
}

FILEOBJECTFUNCTION(FileObject_remove)
{
	GOC_FILEOBJECT* object = (GOC_FILEOBJECT*)handler;
	int i;

	if ( object == NULL )
		GOC_RET_RESULT( handler );
	for (i=0; i<object->attrAmmount; i++)
	{
		if ( goc_stringEquals( object->attrName[i], pBuf ) )
		{
			object->attrName[i] = goc_stringFree( object->attrName[i] );
			object->attrName = goc_tableRemove(object->attrName,
				&(object->attrAmmount), sizeof(char*), i);
			object->attrAmmount++;
			if ( object->attrValue[i] )
				freeObject( object->attrValue[i] );
			object->attrValue = goc_tableRemove(object->attrValue,
				&(object->attrAmmount), sizeof(char*), i);
			GOC_RET_RESULT( handler );
		}
	}
	GOC_RET_RESULT( handler );
}

// w pBuf wskazanie na FILE*
// w uBuf nazwa elementu
FILEOBJECTFUNCTION(FileObject_serialize)
{
	GOC_FILEOBJECT* object = (GOC_FILEOBJECT*)handler;
	GOC_FUNCTIONRETURN res;
	int i;
	const char *classname;

	fprintf(pBuf, "\n");
	for ( i=0; i<object->attrAmmount; i++ )
	{
		res = getObjectClass( object->attrValue[i] );
		if ( res.code != GOC_ERR_OK )
			return res;
		classname = (const char *)res.result;
		fprintf(pBuf, "<%s name='%s'>", classname, object->attrName[i]);
		res = call( object->attrValue[i], "serialize", pBuf, object->attrName[i] );
		if ( res.code != GOC_ERR_OK )
			return res;
		fprintf(pBuf, "</%s>\n", classname);
	}
	GOC_RET_OK;
}

FILEOBJECTFUNCTION(FileObject_deserialize)
{
	GOC_FILEOBJECT* object = NULL;
	GOC_FUNCTIONRETURN res;
	xmlNode *node = pBuf;
	xmlNode *child = NULL;

	if ( handler )
	{
		object = (GOC_FILEOBJECT*)handler;
	}
	else
	{
		res = allocObject(CN_FILEOBJECT, NULL, NULL);
		if ( res.code != GOC_ERR_OK )
			return res;
		object = (GOC_FILEOBJECT *)res.result;
	}

	for ( child = node->children; child; child = child->next )
	{
		xmlAttr *attr = NULL;
		if ( child->type != XML_ELEMENT_NODE )
			continue;
		attr = child->properties;
		if ( !goc_stringEquals( (const char *)attr->name, "name") )
			continue;
		// pobranie elementu, ktory moze juz istniec (wtedy nastapi jego wypelnienie,
		//  a nie zalozenie)
		res = call( object, "get", xmlNodeGetContent(attr->children), 0);
		if ( res.code != GOC_ERR_OK )
			return res;
		if ( res.result )
		{
			// ustaw istniejacy element
			res = call( res.result, "deserialize", child, NULL );
			if ( res.code != GOC_ERR_OK )
				return res;
		}
		else
		{
			// stworz nowy element
			res = callStatic( (const char *)child->name, "deserialize", child, NULL );
			if ( res.code == GOC_ERR_OK )
			{
				// ustaw element w fileobject
				res = call(object, "set",
					xmlNodeGetContent(attr->children),
					res.result );
				if ( res.code != GOC_ERR_OK )
				{
					printf("Nieudane dodawanie %s (%s): %s\n",
						child->name,
						xmlNodeGetContent(attr->children),
						res.description );
					return res;
				}
			}
			else
			{
				return res;
			}
		}
	}
	GOC_RET_RESULT( object );
}

FILEOBJECTFUNCTION(FileObject_equals)
{
	if ( handler != pBuf )
	{
		GOC_FUNCTIONRETURN res;
		GOC_FILEOBJECT *fobA = handler;
		GOC_FILEOBJECT *fobB = pBuf;
		int i;

		if ( fobA->attrAmmount != fobB->attrAmmount )
			GOC_RET_RESULT( 0 );
		for (i=0; i<fobA->attrAmmount; i++)
		{
			res = call( fobB, "get", fobA->attrName[i], 0 );
			if ( res.code != GOC_ERR_OK )
				return res;
			res = call( fobA->attrValue[i], "equals",
				res.result, 0 );
			if ( res.code != GOC_ERR_OK )
				return res;
			if ( res.result == 0 )
				GOC_RET_RESULT( 0 );
		}
		GOC_RET_RESULT( 1 );
	}
	else
		GOC_RET_RESULT( 1 );
}

// handler - handler do FileObject
// pBuf - nazwa parametru w FileObject
// uBuf - przyrownywany obiekt do wartosci parametru
FILEOBJECTFUNCTION(FileObject_equalAttr)
{
	GOC_FUNCTIONRETURN res;
	res = call(handler, "get", pBuf, 0);
	if ( res.code != GOC_ERR_OK )
		return res;
	return call(res.result, "equals", uBuf, 0);
}

FILEOBJECTFUNCTION(FileObject_copy)
{
	GOC_FILEOBJECT* object = (GOC_FILEOBJECT*)handler;
	GOC_OBJECTHANDLER newObject;
	GOC_FUNCTIONRETURN res;
	const char *classname;
	int i;

	res = getObjectClass(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	classname = (const char *)res.result;

	if ( pBuf )
	{
		newObject = pBuf;
		// zwolnij struktury
		res = call( newObject, "free", NULL, 0 );
		if ( res.code != GOC_ERR_OK )
			return res;
	}
	else
	{
		res = allocObject(classname, *((int*)handler), 0);
		if ( res.code != GOC_ERR_OK )
			return res;
		newObject = res.result;
	}


	for ( i=0; i<object->attrAmmount; i++ )
	{
		res = call( object->attrValue[i], "copy", NULL, 0 );
		if ( res.code != GOC_ERR_OK )
			break;
		res = call( newObject, "set", object->attrName[i], res.result );
		if ( res.code != GOC_ERR_OK )
			break;
	}

	// zniszcz tworzony obiekt, jesli nieudane tworzenie
	if ( res.code != GOC_ERR_OK )
	{
		freeObject( newObject );
		return res;
	}

	GOC_RET_RESULT( newObject );
}

// Klasa Obiektu: Table
typedef struct GOC_TABLE
{
	GOC_OBJECTHANDLER *pElement;
//	GOC_OBJECTHANDLER *pElement;
	_GOC_TABEL_SIZETYPE_ nElement;
} GOC_TABLE;

FILEOBJECTFUNCTION(Table_isTable)
{
	GOC_RET_OK;
}

FILEOBJECTFUNCTION(Table_alloc)
{
	GOC_TABLE *tmp = NULL;

	// przydzielenie pamieci
	tmp = (GOC_TABLE*)malloc(sizeof(GOC_TABLE));
	if ( tmp == NULL )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	memset(tmp, 0, sizeof(GOC_TABLE));

	// zwrocenie obiektu
	GOC_RET_RESULT(tmp);
}

FILEOBJECTFUNCTION(Table_free)
{
	GOC_TABLE *tmp = (GOC_TABLE*)handler;
	GOC_FUNCTIONRETURN res;
	int i;

	// zwolnij kazdy z elementow
	for ( i=0; i<tmp->nElement; i++ )
	{
//		fprintf(stderr, "Zwalniam obiekt tablicy\n");
		res = freeObject( &(tmp->pElement[i]) );
		if ( res.code != GOC_ERR_OK )
			return res;
//		fprintf(stderr, "Zwolniono\n");
	}

	// zwolnij zasoby tablicy
	tmp->pElement = goc_tableClear( tmp->pElement, &tmp->nElement );

	// zwolnij strukture tablicy
//	free( tmp );

	GOC_RET_RESULT( NULL );
}

FILEOBJECTFUNCTION(Table_serialize)
{
	GOC_TABLE* tmp = (GOC_TABLE*)handler;
	GOC_FUNCTIONRETURN res;
	int i;
	const char *classname;

	fprintf(pBuf, "\n");
	for ( i=0; i<tmp->nElement; i++ )
	{
		res = getObjectClass( &(tmp->pElement[i]) );
		if ( res.code != GOC_ERR_OK )
			return res;
		classname = (const char *)res.result;
		fprintf(pBuf, "<%s>", classname);
		res = call( &(tmp->pElement[i]), "serialize", pBuf, NULL );
		if ( res.code != GOC_ERR_OK )
			return res;
		fprintf(pBuf, "</%s>\n", classname);
	}
	GOC_RET_OK;
}

FILEOBJECTFUNCTION(Table_deserialize)
{
	GOC_TABLE* tmp = NULL;
	GOC_FUNCTIONRETURN res;
	xmlNode *node = pBuf;
	xmlNode *child = NULL;

	GOC_OBJECTHANDLER old;

	if ( handler )
	{
		tmp = (GOC_TABLE*)handler;
	}
	else
	{
		res = allocObject(CN_TABLE, NULL, NULL);
		if ( res.code != GOC_ERR_OK )
			return res;
		tmp = (GOC_TABLE*)res.result;
	}

	for ( child = node->children; child; child = child->next )
	{
		if ( child->type != XML_ELEMENT_NODE )
			continue;
		// dodanie nowego elementu z wartoscia zerowa na poczatek
		call( handler, "add", NULL, (const char *)child->name );

		// deserializacja nowego elementu
		res = call( &(tmp->pElement[tmp->nElement-1]), "deserialize", child, 0 );
		if ( res.code != GOC_ERR_OK )
			return res;
	}
	GOC_RET_RESULT( tmp );
}

// w pBuf dodawana wartosc elementu (obiekt) (nie jest czyninona kopia, wiec nie przypisywac wartosci stalych !)
// w uBuf typ dodawanego elementu
// z dodawanego elemenu jest robiona kopia podczas allokowania dla niego zasobu
// z rozwnoczesnym zarejestrowaniem obiektu pod okreslonym typem
FILEOBJECTFUNCTION(Table_add)
{
	GOC_TABLE *tmp = (GOC_TABLE*)handler;
	GOC_FUNCTIONRETURN res;
	GOC_OBJECTHANDLER old;

	if ( !uBuf )
		GOC_RET_FALSE( TEXT_WRONGARG );

	old = &(tmp->pElement[0]);

	tmp->pElement = goc_tableAdd(tmp->pElement, &tmp->nElement, sizeof(GOC_OBJECTHANDLER));
	if ( tmp->pElement == NULL )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	// wykonanie przeniesieinia
	res = reallocTableOfObject(old, &(tmp->pElement[0]), tmp->nElement-1);
	if ( res.code != GOC_ERR_OK )
		return res;
	// dodanie nowego elementu
	tmp->pElement[tmp->nElement-1] = pBuf;
	res = allocExternal( uBuf, &(tmp->pElement[tmp->nElement-1]) );
	if ( res.code != GOC_ERR_OK )
		return res;
	GOC_RET_RESULT( tmp );
}

FILEOBJECTFUNCTION(Table_equals)
{
	GOC_TABLE* tmp = (GOC_TABLE*)handler;
	GOC_TABLE* tmpB = (GOC_TABLE*)pBuf;
	GOC_FUNCTIONRETURN res;
	int i;

	if ( tmp == tmpB )
		GOC_RET_RESULT( 1 );

	if ( tmp->nElement != tmpB->nElement )
		GOC_RET_RESULT( 0 );

	for ( i=0; i<tmp->nElement; i++ )
	{
		if ( getObjectClass( &(tmp->pElement[i]) ).result
			!= getObjectClass( &(tmpB->pElement[i]) ).result )
			GOC_RET_RESULT( 0 );

		res = call(
			&( tmp->pElement[i] ), "equals", &( tmpB->pElement[i] ), NULL );
		if ( res.code != GOC_ERR_OK )
			return res;
		if ( res.result == 0 )
			return res;
	}
	GOC_RET_RESULT( 1 );
}

FILEOBJECTFUNCTION(Table_copy)
{
	GOC_TABLE* object = (GOC_TABLE*)handler;
	GOC_TABLE *newObject = NULL;
	GOC_FUNCTIONRETURN res;
	const char *classname;
	int i;

	res = getObjectClass(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	classname = (const char *)res.result;

	if ( pBuf )
	{
		newObject = pBuf;
		// zwolnij struktury obiektu
		res = call( newObject, "free", NULL, 0 );
		if ( res.code != GOC_ERR_OK )
			return res;
	}
	else
	{
		res = allocObject(classname, *((int*)handler), 0);
		if ( res.code != GOC_ERR_OK )
			return res;
		newObject = (GOC_TABLE *)res.result;
	}


	for ( i=0; i<object->nElement; i++ )
	{
		// dodaj nowy element
		newObject->pElement = goc_tableAdd(newObject->pElement, &newObject->nElement, sizeof(GOC_OBJECTHANDLER));
		// zarejestruj
		allocExternal( getObjectClass( &object->pElement[i] ).result, &(newObject->pElement[newObject->nElement-1]) );
		// skopiuj
		res = call( &(object->pElement[i]), "copy", &(newObject->pElement[newObject->nElement-1]), 0 );
		if ( res.code != GOC_ERR_OK )
			break;
	}
	if ( res.code != GOC_ERR_OK )
	{
		freeObject(newObject);
		return res;
	}
	GOC_RET_RESULT( newObject );

}

// Klasa OTable
FILEOBJECTFUNCTION(OTable_serialize)
{
	GOC_TABLE* tmp = (GOC_TABLE*)handler;
	GOC_FUNCTIONRETURN res;
	int i;
	const char *classname;

	fprintf(pBuf, "\n");
	for ( i=0; i<tmp->nElement; i++ )
	{
		res = getObjectClass( tmp->pElement[i] );
		if ( res.code != GOC_ERR_OK )
			return res;
		classname = (const char *)res.result;
		fprintf(pBuf, "<%s>", classname);
		res = call( tmp->pElement[i], "serialize", pBuf, NULL );
		if ( res.code != GOC_ERR_OK )
			return res;
		fprintf(pBuf, "</%s>\n", classname);
	}
	GOC_RET_OK;
}

FILEOBJECTFUNCTION(OTable_deserialize)
{
	GOC_TABLE* tmp = NULL;
	GOC_FUNCTIONRETURN res;
	xmlNode *node = pBuf;
	xmlNode *child = NULL;

	GOC_OBJECTHANDLER old;

	if ( handler )
	{
		tmp = (GOC_TABLE*)handler;
	}
	else
	{
		res = allocObject(CN_TABLE, NULL, NULL);
		if ( res.code != GOC_ERR_OK )
			return res;
		tmp = (GOC_TABLE*)res.result;
	}

	for ( child = node->children; child; child = child->next )
	{
		if ( child->type != XML_ELEMENT_NODE )
			continue;
		// dodanie nowego elementu z wartoscia zerowa na poczatek
		call( handler, "add", NULL, (const char *)child->name );

		// deserializacja nowego elementu
		res = call( tmp->pElement[tmp->nElement-1], "deserialize", child, 0 );
		if ( res.code != GOC_ERR_OK )
			return res;
	}
	GOC_RET_RESULT( tmp );
}

// w pBuf dodawana wartosc elementu (obiekt) (nie jest czyninona kopia, wiec nie przypisywac wartosci stalych !)
// w uBuf typ dodawanego elementu
// z dodawanego elemenu jest robiona kopia podczas allokowania dla niego zasobu
// z rozwnoczesnym zarejestrowaniem obiektu pod okreslonym typem
FILEOBJECTFUNCTION(OTable_add)
{
	GOC_TABLE *tmp = (GOC_TABLE*)handler;
	GOC_FUNCTIONRETURN res;

	tmp->pElement = goc_tableAdd(tmp->pElement, &tmp->nElement, sizeof(GOC_OBJECTHANDLER));
	if ( tmp->pElement == NULL )
		GOC_RET_FALSE( TEXT_NOMEMORY );
	// dodanie nowego elementu
	tmp->pElement[tmp->nElement-1] = pBuf;
	GOC_RET_RESULT( tmp );
}

FILEOBJECTFUNCTION(OTable_equals)
{
	GOC_TABLE* tmp = (GOC_TABLE*)handler;
	GOC_TABLE* tmpB = (GOC_TABLE*)pBuf;
	GOC_FUNCTIONRETURN res;
	int i;

	if ( tmp == tmpB )
		GOC_RET_RESULT( 1 );

	if ( tmp->nElement != tmpB->nElement )
		GOC_RET_RESULT( 0 );

	for ( i=0; i<tmp->nElement; i++ )
	{
		if ( getObjectClass( tmp->pElement[i] ).result
			!= getObjectClass( tmpB->pElement[i] ).result )
			GOC_RET_RESULT( 0 );

		res = call(
			tmp->pElement[i], "equals", tmpB->pElement[i], NULL );
		if ( res.code != GOC_ERR_OK )
			return res;
		if ( res.result == 0 )
			return res;
	}
	GOC_RET_RESULT( 1 );
}

FILEOBJECTFUNCTION(OTable_copy)
{
	GOC_TABLE* object = (GOC_TABLE*)handler;
	GOC_TABLE *newObject = NULL;
	GOC_FUNCTIONRETURN res;
	const char *classname;
	int i;

	res = getObjectClass(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	classname = (const char *)res.result;

	if ( pBuf )
	{
		newObject = pBuf;
		// zwolnij struktury obiektu
		res = call( newObject, "free", NULL, 0 );
		if ( res.code != GOC_ERR_OK )
			return res;
	}
	else
	{
		res = allocObject(classname, *((int*)handler), 0);
		if ( res.code != GOC_ERR_OK )
			return res;
		newObject = (GOC_TABLE *)res.result;
	}


	for ( i=0; i<object->nElement; i++ )
	{
		// dodaj nowy element
		newObject->pElement = goc_tableAdd(newObject->pElement, &newObject->nElement, sizeof(GOC_OBJECTHANDLER));
		// skopiuj
		res = callStatic( classname, "copy", NULL, 0 );
		if ( res.code != GOC_ERR_OK )
			break;
		else
			newObject->pElement[newObject->nElement-1] = res.result;
	}
	if ( res.code != GOC_ERR_OK )
	{
		freeObject(newObject);
		return res;
	}
	GOC_RET_RESULT( newObject );

}

// Klasa Obiektu: Pointer
FILEOBJECTFUNCTION(Pointer_alloc)
{
	GOC_RET_RESULT( pBuf );
}

FILEOBJECTFUNCTION(Pointer_free)
{
	GOC_RET_RESULT( NULL );
}

FILEOBJECTFUNCTION(Pointer_get)
{
	GOC_RET_RESULT( handler );
}

FILEOBJECTFUNCTION(Pointer_set)
{
	GOC_FUNCTIONRETURN res;
	const char *classname = NULL;

	// pobranie nazwy klasy
	res = getObjectClass(handler);
	if ( res.code != GOC_ERR_OK )
		return res;
	classname = (const char *)res.result;

	// zwolnienie zasobow
	if ( handler )
	{
		res = freeObject( handler );
		if ( res.code != GOC_ERR_OK )
			return res;
		handler = (GOC_OBJECTHANDLER)res.result;
	}
	// utworz nowy obiekt
	res = allocObject( classname, pBuf, 0 );
	if ( res.code != GOC_ERR_OK )
		return res;
	// zwroc nowa rejestracje obiektu
	GOC_RET_RESULT(res.result);
}

FILEOBJECTFUNCTION(Pointer_serialize)
{
#ifdef _DEBUG
	fprintf(pBuf, "0x%X", (unsigned int)handler);
#endif // ifdef _DEBUG
	GOC_RET_OK;
}

FILEOBJECTFUNCTION(Pointer_deserialize)
{
	GOC_RET_FALSE( TEXT_NOTALLOWED );
}

// handler - GOC_OBJECTHANDLER
// pBuf - GOC_OBJECTHANDLER
FILEOBJECTFUNCTION(Pointer_equals)
{
	GOC_RET_RESULT( handler == (GOC_OBJECTHANDLER)pBuf );
}

FILEOBJECTFUNCTION(Pointer_copy)
{
	// Nie wykonywac kopii czegos, co bedzie wygladalo tak samo !
	GOC_RET_RESULT( handler );
}

GOC_FUNCTIONRETURN registerLibrary(GOC_OBJECTHANDLER oh, void *pBuf, unsigned int uBuf)
{
	// to jest klasa, ktora jest tworzona z obiektow bezposrednich,
	// natywnych - ktore sa rejestrowane podczas dodawania do tabeli
	registerClass(CN_TABLE);
	registerClassFunction(CN_TABLE, "isTable", Table_isTable);
	registerClassFunction(CN_TABLE, "free", Table_free);
	registerClassFunction(CN_TABLE, "alloc", Table_alloc);
	registerClassFunction(CN_TABLE, "serialize", Table_serialize);
	registerClassFunction(CN_TABLE, "deserialize", Table_deserialize);
	registerClassFunction(CN_TABLE, "add", Table_add);
	registerClassFunction(CN_TABLE, "equals", Table_equals);
	registerClassFunction(CN_TABLE, "copy", Table_copy);

	// to jest klas, ktora przechowuje uchwyty do objectHandler'ow
	// nie wykonuje rejestrowania dodawanych elementow, tylko je
	// dodaje
	registerClassParented(CN_OBJECTTABLE, CN_TABLE);
	registerClassFunction(CN_OBJECTTABLE, "serialize", OTable_serialize);
	registerClassFunction(CN_OBJECTTABLE, "deserialize", OTable_deserialize);
	registerClassFunction(CN_OBJECTTABLE, "add", OTable_add);
	registerClassFunction(CN_OBJECTTABLE, "equals", OTable_equals);
	registerClassFunction(CN_OBJECTTABLE, "copy", OTable_copy);

	registerClass(CN_INTEGER);
	registerClassFunction(CN_INTEGER, "set", Integer_set);
	registerClassFunction(CN_INTEGER, "get", Integer_get);
	registerClassFunction(CN_INTEGER, "alloc", Integer_alloc);
	registerClassFunction(CN_INTEGER, "free", Integer_free);
	registerClassFunction(CN_INTEGER, "serialize", Integer_serialize);
	registerClassFunction(CN_INTEGER, "deserialize", Integer_deserialize);
	registerClassFunction(CN_INTEGER, "equals", Integer_equals);
	registerClassFunction(CN_INTEGER, "copy", Integer_copy);

	registerClass(CN_STRING);
	registerClassFunction(CN_STRING, "set", String_set);
	registerClassFunction(CN_STRING, "get", String_get);
	registerClassFunction(CN_STRING, "alloc", String_alloc);
	registerClassFunction(CN_STRING, "free", String_free);
	registerClassFunction(CN_STRING, "isString", String_isString);
	registerClassFunction(CN_STRING, "serialize", String_serialize);
	registerClassFunction(CN_STRING, "deserialize", String_deserialize);
	registerClassFunction(CN_STRING, "equals", String_equals);
	registerClassFunction(CN_STRING, "copy", String_copy);
	registerClassFunction(CN_STRING, "registerPtr", String_registerPtr);

	registerClass(CN_FILEOBJECT);
	registerClassFunction(CN_FILEOBJECT, "alloc", FileObject_alloc);
	registerClassFunction(CN_FILEOBJECT, "free", FileObject_free);
	registerClassFunction(CN_FILEOBJECT, "set", FileObject_set);
	registerClassFunction(CN_FILEOBJECT, "get", FileObject_get);
	registerClassFunction(CN_FILEOBJECT, "remove", FileObject_remove);
	registerClassFunction(CN_FILEOBJECT, "serialize", FileObject_serialize);
	registerClassFunction(CN_FILEOBJECT, "deserialize", FileObject_deserialize);
	registerClassFunction(CN_FILEOBJECT, "equals", FileObject_equals);
	registerClassFunction(CN_FILEOBJECT, "equalAttr", FileObject_equalAttr);
	registerClassFunction(CN_FILEOBJECT, "copy", FileObject_copy);
//	registerClassFunction(CN_FILEOBJECT, "assign", FileObject_assign);


	// To tylko okreslenie wskazania na cokolwiek (najczesciej na funkcje)
	// jako Pointer. Nie jest skladowana zadna dana - jest tylko
	// sklasyfikowana
	registerClass(CN_POINTER);
	registerClassFunction(CN_POINTER, "alloc", Pointer_alloc);
	registerClassFunction(CN_POINTER, "free", Pointer_free);
	registerClassFunction(CN_POINTER, "set", Pointer_set);
	registerClassFunction(CN_POINTER, "get", Pointer_get);
	registerClassFunction(CN_POINTER, "serialize", Pointer_serialize);
	registerClassFunction(CN_POINTER, "deserialize", Pointer_deserialize);
	registerClassFunction(CN_POINTER, "equals", Pointer_equals);
	registerClassFunction(CN_POINTER, "copy", Pointer_copy);
	GOC_RET_OK;
}

void fileObjectInit()
{
	registerLibrary(NULL, NULL, 0);
}

/**
 * Utworzenie podstawowych struktur dla obiektu GOC_FILEOBJECT
 * 
 * @return wskazanie na stworzony obiekt.
 */
GOC_FILEOBJECT *fileObjectCreate()
{
	GOC_FUNCTIONRETURN res = allocObject(CN_FILEOBJECT, NULL, 0);
	if ( res.code != GOC_ERR_OK )
		return NULL;
	return (GOC_FILEOBJECT *)res.result;
}

/**
 * Ustawienie atrybutu obiektu.
 * Je¿eli takowy atrybut jest ju¿ ustawiony, jego warto¶æ zostanie usuniêta.
 *
 * @param object zmieniany obiekt. Je¶li NULL zostanie zwrócony NULL.
 * @param attrName nazwa atrybutu, ktory ma podlegac okresleniu.
 * @param attrValue wartosc nadawana dla atrybutu.
 * @return wskazanie na zmieniony obiekt (wskazanie nie podlega zmianie).
 */
GOC_FILEOBJECT *fileObjectSetAttribute(
	GOC_FILEOBJECT *object, const char *attrName, const char *attrValue)
{
	GOC_FUNCTIONRETURN res = call( object, "set", attrName, attrValue);
	if ( res.code != GOC_ERR_OK )
		return NULL;
	return (GOC_FILEOBJECT *)res.result;
}

/**
 * Usuniêcie z obiektu okre¶lonego atrybutu.
 * Je¿eli obiekt jest NULL, zostanie zwrócony NULL.
 * Je¿eli attrName podano jako NULL, zostanie usuniêty parametr ustawiony
 * jako NULL.
 *
 * @param object wskazanie na obiekt.
 * @param attrName nazwa atrybutu, który ma zostaæ usuniêty.
 * @return wskazanie na zmieniony obiekt (wskazanie nie podlega zmianie).
 */
GOC_FILEOBJECT *fileObjectRemoveAttribute(
	GOC_FILEOBJECT *object, const char *attrName)
{
	GOC_FUNCTIONRETURN res = call(object, "remove", attrName, 0);
	if ( res.code != GOC_ERR_OK )
		return NULL;
	return (GOC_FILEOBJECT *)res.result;
}

/**
 * Zwrócenie warto¶ci jako zosta³a przypisana atrybutowi.
 * Je¿eli object jest NULL to zwróci NULL.
 * Je¿eli attrName jest NULL to zwróci warto¶æ przypisan± do atrybutu NULL.
 *
 * @param object wskazanie na obiekt.
 * @param attrName nazwa atrybutu.
 * @return przypisana warto¶æ.
 */
const char *fileObjectGetAttribute(
	GOC_FILEOBJECT *object, const char *attrName)
{
	GOC_FUNCTIONRETURN res = call(object, "get", attrName, 0);
	if ( res.code != GOC_ERR_OK )
		return NULL;
	res = call( res.result, "get", NULL, 0);
	if ( res.code != GOC_ERR_OK )
		return NULL;
	return (const char *)res.result;
}

/**
 * Zwraca pozycjê atrybutu w tablicy wewn±trz struktury obiektu.
 * Je¿eli object NULL to zwróci -1.
 * Je¿eli attrName NULL to zwróci warto¶æ przypisan± do atrybutu NULL.
 *
 * @param object wskazanie na obiekt.
 * @param attrName nazwa atrybutu.
 * @return pozycja atrybutu lub -1 je¶li nie znaleziono.
 */
int fileObjectGetAttributePos(
	GOC_FILEOBJECT *object, const char *attrName)
{
	int i;
	if ( object == NULL )
		return -1;
	for (i=0; i<object->attrAmmount; i++)
	{
		if ( goc_stringEquals( object->attrName[i], attrName ) )
			return i;
	}
	return -1;
}

/**
 * Zniszczenie obiektu GOC_FILEOBJECT i zwolnienie allokowanej przez niego pamiêci.
 *
 * @param object obiekt do zwolnienia.
 * @retun NULL
 */
GOC_FILEOBJECT *fileObjectDestroy(GOC_FILEOBJECT *object)
{
	GOC_FUNCTIONRETURN res = freeObject(object);
	if ( res.code != GOC_ERR_OK )
		return NULL;
	return (GOC_FILEOBJECT *)res.result;
}

void fileObjectSerialize(GOC_FILEOBJECT *object, FILE *fileptr)
{
	call(object, "serialize", fileptr, 0);
}

int fileObjectCount(GOC_FILEOBJECT *object)
{
	return object->attrAmmount;
}

const char *fileObjectGetAttributeNameAt(GOC_FILEOBJECT *object, int pos)
{
	return object->attrName[pos];
}

const char *fileObjectGetAttributeAt(GOC_FILEOBJECT *object, int pos)
{
	return call(object->attrValue[pos], "get", NULL, 0).result;
}

int fobGetInt(GOC_FILEOBJECT *object, const char *attrName)
{
	GOC_FUNCTIONRETURN ret;
	ret = call(object, "get", attrName, 0);
	if ( ret.code != GOC_ERR_OK )
		return -1;
	ret = call(ret.result, "get", NULL, 0);
	if ( ret.code != GOC_ERR_OK )
		return -1;
	return ret.result;
}

unsigned int fobGetUInt(GOC_FILEOBJECT *object, const char *attrName)
{
	return fobGetInt(object, attrName);
}

const char *fobGetStr(GOC_FILEOBJECT *object, const char *attrName)
{
	return fileObjectGetAttribute(object, attrName);
}

GOC_FILEOBJECT* fobSetInt(GOC_FILEOBJECT *object, const char *attrName, int value)
{
	call(object, "set", attrName, allocObject(CN_INTEGER, value, 0).result);
	return object;
}

GOC_FILEOBJECT* fobSetStr(GOC_FILEOBJECT *object, const char *attrName, const char *value)
{
	call(object, "set", attrName, allocObject(CN_STRING, value, 0).result);
	return object;
}
