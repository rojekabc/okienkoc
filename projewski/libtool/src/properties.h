#ifndef _GOC_PROPERTIES_H_
#	define _GOC_PROPERTIES_H_

#	include "tablica.h"
#	include "iterator.h"
#	include "istream.h"
#   include "ostream.h"

// default category
#define GOC_PROPERTIES_CATEGORY_DEFAULT "default"

// The property
// - name
// - value
#	define GOC_STRUCT_PROPERTY \
		char *name; \
		char *value

typedef struct GOC_Property
{
	GOC_STRUCT_PROPERTY;
} GOC_Property;

// Category
// - name
// - properties
#	define GOC_STRUCT_CATEGORY \
		char *name; \
		GOC_Property **pProperty; \
		_GOC_TABEL_SIZETYPE_ nProperty

typedef struct GOC_Category
{
	GOC_STRUCT_CATEGORY;
} GOC_Category;

// Categorized properties
#	define GOC_STRUCT_PROPERTIES \
		GOC_Category **pCategory; \
		_GOC_TABEL_SIZETYPE_ nCategory

typedef struct GOC_Properties
{
	GOC_STRUCT_PROPERTIES;
} GOC_Properties;


// alloc empty properties
GOC_Properties *goc_propertiesAlloc();
// free properties
GOC_Properties *goc_propertiesFree(GOC_Properties *properties);


// load properties
//
// properties - the properties structure
// is - the input stream
//
// return the properties structure
//
// if properties argue NULL - create ones, in other hand append categorized properties
// if stream is NULL - nothing will be changed (if properties NULL too, create empty)
GOC_Properties *goc_propertiesLoad(GOC_Properties* properties, GOC_IStream* is);


void goc_propertiesSave(GOC_Properties* properties, GOC_OStream* os);
//  Wyczy�� wszystkie kategorie i w�a�ciwo�ci
GOC_Properties* goc_propertiesClear(GOC_Properties*);
// Wyczy�� w�a�ciwo�ci wybranej kategorii
// GOC_Properties* goc_propertiesCategoryClear(GOC_Properties*, char*);


// wylistowanie w�a�ciwo�ci w domy�lnej kategorii
// return GOC_Iterator of GOC_Property
GOC_Iterator *goc_propertiesList(GOC_Properties*);
// wtlistowanie kategorii
// return GOC_Iterator of GOC_Category
GOC_Iterator *goc_propertiesListCategories(GOC_Properties*);
// wylistowanie w�a�ciwo�ci w kategorii
GOC_Iterator *goc_propertiesListCategory(GOC_Properties*, char*);
// podaj warto�� w�a�ciwo�ci dla klucza z domy�lnej kategorii
const char *goc_propertiesGetValue(GOC_Properties*, const char*);
// podaj warto�� w�a�ciwo�ci dla klucza z wybranej kategorii
const char *goc_propertiesGetCategoryValue(GOC_Properties*, const char*, const char*);


GOC_Properties *goc_propertiesAddCategory(GOC_Properties*, char*);
GOC_Properties *goc_propertiesSet(GOC_Properties*, const char*, const char*);
GOC_Properties *goc_propertiesCategorySet(GOC_Properties*, const char*, const char*, const char*);
GOC_Properties *goc_propertiesRemCategory(GOC_Properties*, char*);

#endif // ifndef _GOC_PROPERTIES_H_
