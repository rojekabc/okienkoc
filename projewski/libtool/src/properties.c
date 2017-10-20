#include "global-inc.h"
#include "properties.h"
#include "tableiterator.h"
#include "mystr.h"
#define GOC_PRINTERROR
#include "log.h"
#include <stdio.h>
#ifndef _DEBUG
#	include <string.h>
#endif

//
// Funkcje *Internal obowi±zuj± bez sprawdzania parametrów i s± static
//

static GOC_Category* goc_categoryAlloc(char *name)
{
	GOC_Category *c;

	GOC_DEBUG("-> goc_categoryAlloc");

	c = (GOC_Category*)malloc(sizeof(GOC_Category));
	if ( c )
	{
		memset(c, 0, sizeof(GOC_Category));
		c->name = goc_stringTrim(goc_stringCopy(c->name, name));
	}
	else
	{
		GOC_ERROR("Memory: Error allocating GOC_Category structure");
	}

	GOC_DEBUG("<- goc_categoryAlloc");
	return c;
}

static GOC_Category* propertiesGetCategoryInternal(GOC_Properties* p, char *name)
{
	int i = p->nCategory;
	GOC_Category* c = NULL;

	GOC_DEBUG("-> propertiesGetCategoryInternal");

	while (i--)
	{
		if ( goc_stringEquals(p->pCategory[i]->name, name) )
		{
			c = p->pCategory[i];
			break;
		}
	}

	GOC_DEBUG("<- propertiesGetCategoryInternal");
	return c;
}

static GOC_Category* propertiesAddCategoryInternal(GOC_Properties* p, char* name)
{
	GOC_Category* c;

	GOC_DEBUG("-> propertiesAddCategoryInternal");

	// czy kategoria ju¿ istnieje
	c = propertiesGetCategoryInternal(p, name);
	if ( !c )
	{
		// nowa kategoria
		c = goc_categoryAlloc(name);
		p->pCategory = goc_tableAdd(p->pCategory, &p->nCategory, sizeof(GOC_Category*));
		p->pCategory[p->nCategory-1] = c;
	}
	GOC_DEBUG("<- propertiesAddCategoryInternal");
	return c;
}

GOC_Properties* goc_propertiesAddCategory(GOC_Properties* p, char *name)
{
	GOC_DEBUG("-> goc_propertiesAddCategory");
	if ( p )
		propertiesAddCategoryInternal(p, name);
	GOC_DEBUG("<- goc_propertiesAddCategory");
	return p;
}

GOC_Properties* goc_propertiesAlloc()
{
	GOC_Properties* p = NULL;
	GOC_DEBUG("-> goc_propertiesAlloc");
	p = (GOC_Properties*)malloc(sizeof(GOC_Properties));
	if ( p )
	{
		memset(p, 0, sizeof(GOC_Properties));
		propertiesAddCategoryInternal(p, GOC_PROPERTIES_CATEGORY_DEFAULT);
	}
	else
	{
		GOC_ERROR("Memory: Error allocating GOC_Properties structure");
	}
	GOC_DEBUG("<- goc_propertiesAlloc");
	return p;
}

static GOC_Property* propertyFree(GOC_Property *p)
{
	GOC_DEBUG("-> propertyFree");
	if ( p )
	{
		goc_stringFree(p->name);
		goc_stringFree(p->value);
		free(p);
	}
	GOC_DEBUG("<- propertyFree");
	return NULL;
}

static GOC_Category* goc_categoryFree(GOC_Category *c)
{
	int i;
	GOC_DEBUG("-> goc_categoryFree");

	if ( c )
	{
		goc_stringFree(c->name);

		i = c->nProperty;
		while ( i-- )
			propertyFree( c->pProperty[i] );
		c->pProperty = goc_tableClear(c->pProperty, &c->nProperty);

		free(c);
	}
	GOC_DEBUG("<- goc_categoryFree");
	return NULL;
}

GOC_Properties* goc_propertiesClear(GOC_Properties* p)
{
	int i;
	GOC_DEBUG("-> goc_propertiesClear");

	if ( p )
	{
		i = p->nCategory;
		while ( i-- )
			goc_categoryFree(p->pCategory[i]);
		p->pCategory = goc_tableClear(p->pCategory, &p->nCategory);
	}
	GOC_DEBUG("<- goc_propertiesClear");
	return p;
}

GOC_Properties* goc_propertiesFree(GOC_Properties* p)
{
	GOC_DEBUG("-> goc_propertiesFree");
	if ( p )
	{
		free(goc_propertiesClear(p));
	}
	GOC_DEBUG("<- goc_propertiesFree");
	return NULL;
}

static GOC_Property* propertyAlloc(char* name, char* value)
{
	GOC_Property* p;
	GOC_BDEBUG("-> propertyAlloc(name:[%s], value:[%s])", name, value);

	p = malloc(sizeof(GOC_Property));
	memset(p, 0, sizeof(GOC_Property));
	p->name = goc_stringTrim(goc_stringCopy(p->name, name));
	p->value = goc_stringTrim(goc_stringCopy(p->value, value));

	GOC_DEBUG("<- propertyAlloc");
	return p;
}

static GOC_Property* goc_categoryGetProperty(GOC_Category* c, char* name)
{
	int i;
	GOC_Property* p = NULL;

	GOC_DEBUG("-> categoryGetPropery");

	i = c->nProperty;
	while ( i-- )
	{
		if ( goc_stringEquals(c->pProperty[i]->name, name) )
		{
			p = c->pProperty[i];
			break;
		}
	}

	GOC_DEBUG("<- categoryGetPropery");
	return p;
}

static GOC_Property* goc_categorySetInternal(GOC_Category* c, char* name, char* value)
{
	GOC_Property* p = NULL;

	GOC_DEBUG("-> goc_categorySetInternal");

	// czy w³a¶ciwo¶æ ju¿ istnieje
	p = goc_categoryGetProperty(c, name);
	if ( p )
	{
		p->value = goc_stringCopy(p->value, value);

		GOC_DEBUG("<- goc_categorySetInternal");
		return p;
	}

	// utwórz now± w³a¶ciwo¶æ
	p = propertyAlloc(name, value);
	if ( p )
	{
		c->pProperty = goc_tableAdd(c->pProperty, &c->nProperty, sizeof(GOC_Property));
		c->pProperty[c->nProperty-1] = p;
	}
	else
	{
		GOC_ERROR("Memory: Error allocating GOC_Property structure");
	}

	GOC_DEBUG("<- goc_categorySetInternal");
	return p;
}

// za³adowanie ze strumienia wskazanego
GOC_Properties *goc_propertiesLoad(GOC_Properties *p, GOC_IStream *is)
{
	char *line = NULL;
	char *pos = NULL;
	GOC_Category *category = NULL;

	GOC_DEBUG("-> goc_propertiesLoad");

	// allokacja
	if ( !p )
		p = goc_propertiesAlloc();

	// brak strumienia
	if ( is == NULL )
	{
		GOC_DEBUG("<- goc_propertiesLoad");
		return p;
	}

	// domy¶lna kategoria to zawsze ta pierwsza z brzegu
	if ( !p->nCategory )
		p = goc_propertiesAddCategory(p, GOC_PROPERTIES_CATEGORY_DEFAULT);
	category = p->pCategory[0];

	// odczytywanie pliku
	while ( (line=goc_stringFree(line), line=goc_isReadLine(is)) )
	{
		line = goc_stringTrim( line );

		// nic nie ma
		if ( goc_stringLength(line) == 0 )
			continue;

		// komentarz
		if ( *line == '#' )
			continue;

		// kategoria
		if ( *line == '[' )
		{
			pos = strchr(line, ']');
			// brak domkniêcia
			if ( !pos )
				continue;
			*pos = 0;
			category = propertiesAddCategoryInternal(p, line+1);
			continue;
		}

		pos = strchr(line, '=');

		// brak znaku =
		if ( !pos )
			continue;

		*pos = 0;
		goc_categorySetInternal(category, line, pos+1);
	}

	GOC_DEBUG("<- goc_propertiesLoad");
	return p;
}

// wylistowanie w³a¶ciwo¶ci w domy¶lnej kategorii
GOC_Iterator *goc_propertiesList(GOC_Properties *p)
{
	GOC_Iterator* it = NULL;

	GOC_DEBUG("-> goc_propertiesList");

	if ( p && p->nCategory )
	{
		it = goc_tableIteratorAlloc(p->pCategory[0]->pProperty, p->pCategory[0]->nProperty);
	}

	GOC_DEBUG("<- goc_propertiesList");
	return it;
}

// wtlistowanie kategorii
GOC_Iterator *goc_propertiesListCategories(GOC_Properties *p)
{
	GOC_Iterator* it = NULL;

	GOC_DEBUG("-> goc_propertiesListCategories");

	if ( p )
	{
		it = goc_tableIteratorAlloc(p->pCategory, p->nCategory);
	}

	GOC_DEBUG("<- goc_propertiesListCategories");
	return it;
}

// wylistowanie w³a¶ciwo¶ci w kategorii
GOC_Iterator *goc_propertiesListCategory(GOC_Properties *p, char *cName)
{
	GOC_Category *c;
	GOC_Iterator* it = NULL;
	GOC_DEBUG("-> goc_propertiesListCategory");

	if ( p && (c = propertiesGetCategoryInternal(p, cName)) )
		it = goc_tableIteratorAlloc(c->pProperty, c->nProperty);

	GOC_DEBUG("<- goc_propertiesListCategory");
	return it;
}

// podaj warto¶æ w³a¶ciwo¶ci dla klucza z domy¶lnej kategorii
char *goc_propertiesGetValue(GOC_Properties *p, char *kName)
{
	GOC_DEBUG("-> goc_propertiesGetValue");
	GOC_Category *cat = NULL;
	GOC_Property *prop = NULL;
	char *result = NULL;
	while ( result == NULL )
	{
		if (( p == NULL ) || ( kName == NULL ))
			break;
		cat = propertiesGetCategoryInternal(
			p, GOC_PROPERTIES_CATEGORY_DEFAULT);
		if ( cat == NULL )
			break;
		prop = goc_categoryGetProperty(cat, kName);
		if ( prop == NULL )
			break;
		result = prop->value;
		break;
	}
	GOC_DEBUG("<- goc_propertiesGetValue");
	return result;
}

// podaj warto¶æ w³a¶ciwo¶ci dla klucza z wybranej kategorii
char *goc_propertiesGetCategoryValue(GOC_Properties *p, char *cName, char *kName)
{
	GOC_DEBUG("-> goc_propertiesGetCategoryValue");
	GOC_DEBUG("<- goc_propertiesGetCategoryValue");
	return NULL;
}
