#ifndef _GOC_PROPERTIES_H_
#	define _GOC_PROPERTIES_H_

#	include "tablica.h"
#	include "iterator.h"
#	include "istream.h"

// kategoria domy¶lna - u¿ywana, gdy nie ma ¿adnej kategorii
#define GOC_PROPERTIES_CATEGORY_DEFAULT "default"

// W³a¶ciwo¶æ
// name - nazwa w³a¶ciwo¶ci
// value - warto¶æ w³a¶ciwo¶ci
#	define GOC_STRUCT_PROPERTY \
		char *name; \
		char *value

typedef struct GOC_Property
{
	GOC_STRUCT_PROPERTY;
} GOC_Property;

// Kategoria
// name - nazwa kategorii
// pProperty - zbiór w³a¶ciwo¶ci
// nProperty - liczba w³a¶ciwo¶ci w zbiorze
#	define GOC_STRUCT_CATEGORY \
		char *name; \
		GOC_Property **pProperty; \
		_GOC_TABEL_SIZETYPE_ nProperty

typedef struct GOC_Category
{
	GOC_STRUCT_CATEGORY;
} GOC_Category;

// Zbiór w³a¶ciwo¶ci skategoryzowanych
// pCategory - zbiór kategorii
// pCategory - liczba kategorii w zbiorze
#	define GOC_STRUCT_PROPERTIES \
		GOC_Category **pCategory; \
		_GOC_TABEL_SIZETYPE_ nCategory

typedef struct GOC_Properties
{
	GOC_STRUCT_PROPERTIES;
} GOC_Properties;


// Stwórz pust± strukturê
GOC_Properties *goc_propertiesAlloc();
// Zwolnij strukturê
GOC_Properties *goc_propertiesFree(GOC_Properties *properties);


// za³adowanie ze strumienia wej¶ciowego is w³a¶ciwo¶ci properties
//
// properties - w³a¶ciwo¶ci
// is - strumieñ wej¶ciowy
//
// return - struktura w³a¶ciwo¶ci
//
// je¿eli properties jest NULL to utworzy strukturê properties
// je¿eli properties istnieje to doda doñ odczytane w³a¶ciwo¶ci
// 	oraz kategorie
// Je¿eli strumieñ jest null, to zwróci strukturê properties bez zmian,
// 	a je¶li by³a NULL to utworzy pust±
GOC_Properties *goc_propertiesLoad(GOC_Properties* properties, GOC_IStream* is);
// TODO: zachowanie do wskazanego strumienia
// void propertiesSave(GOC_Properties*, OStream*);
//  Wyczy¶æ wszystkie kategorie i w³a¶ciwo¶ci
GOC_Properties* goc_propertiesClear(GOC_Properties*);
// Wyczy¶æ w³a¶ciwo¶ci wybranej kategorii
GOC_Properties* propertiesCategoryClear(GOC_Properties*, char*);


// wylistowanie w³a¶ciwo¶ci w domy¶lnej kategorii
// return GOC_Iterator of GOC_Property
GOC_Iterator *goc_propertiesList(GOC_Properties*);
// wtlistowanie kategorii
// return GOC_Iterator of GOC_Category
GOC_Iterator *goc_propertiesListCategories(GOC_Properties*);
// wylistowanie w³a¶ciwo¶ci w kategorii
GOC_Iterator *goc_propertiesListCategory(GOC_Properties*, char*);
// podaj warto¶æ w³a¶ciwo¶ci dla klucza z domy¶lnej kategorii
char *goc_propertiesGetValue(GOC_Properties*, char*);
// podaj warto¶æ w³a¶ciwo¶ci dla klucza z wybranej kategorii
char *goc_propertiesGetCategoryValue(GOC_Properties*, char*, char*);


GOC_Properties *goc_propertiesAddCategory(GOC_Properties*, char*);
GOC_Properties *propertiesSet(GOC_Properties*, char*, char*);
GOC_Properties *propertiesCategorySet(GOC_Properties*, char*, char*, char*);
GOC_Properties *propertiesRemCategory(GOC_Properties*, char*);

#endif // ifndef _GOC_PROPERTIES_H_
