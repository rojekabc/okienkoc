#ifndef _GOC_PROPERTIES_H_
#	define _GOC_PROPERTIES_H_

#	include "tablica.h"
#	include "iterator.h"
#	include "istream.h"

// kategoria domy�lna - u�ywana, gdy nie ma �adnej kategorii
#define GOC_PROPERTIES_CATEGORY_DEFAULT "default"

// W�a�ciwo��
// name - nazwa w�a�ciwo�ci
// value - warto�� w�a�ciwo�ci
#	define GOC_STRUCT_PROPERTY \
		char *name; \
		char *value

typedef struct GOC_Property
{
	GOC_STRUCT_PROPERTY;
} GOC_Property;

// Kategoria
// name - nazwa kategorii
// pProperty - zbi�r w�a�ciwo�ci
// nProperty - liczba w�a�ciwo�ci w zbiorze
#	define GOC_STRUCT_CATEGORY \
		char *name; \
		GOC_Property **pProperty; \
		_GOC_TABEL_SIZETYPE_ nProperty

typedef struct GOC_Category
{
	GOC_STRUCT_CATEGORY;
} GOC_Category;

// Zbi�r w�a�ciwo�ci skategoryzowanych
// pCategory - zbi�r kategorii
// pCategory - liczba kategorii w zbiorze
#	define GOC_STRUCT_PROPERTIES \
		GOC_Category **pCategory; \
		_GOC_TABEL_SIZETYPE_ nCategory

typedef struct GOC_Properties
{
	GOC_STRUCT_PROPERTIES;
} GOC_Properties;


// Stw�rz pust� struktur�
GOC_Properties *goc_propertiesAlloc();
// Zwolnij struktur�
GOC_Properties *goc_propertiesFree(GOC_Properties *properties);


// za�adowanie ze strumienia wej�ciowego is w�a�ciwo�ci properties
//
// properties - w�a�ciwo�ci
// is - strumie� wej�ciowy
//
// return - struktura w�a�ciwo�ci
//
// je�eli properties jest NULL to utworzy struktur� properties
// je�eli properties istnieje to doda do� odczytane w�a�ciwo�ci
// 	oraz kategorie
// Je�eli strumie� jest null, to zwr�ci struktur� properties bez zmian,
// 	a je�li by�a NULL to utworzy pust�
GOC_Properties *goc_propertiesLoad(GOC_Properties* properties, GOC_IStream* is);
// TODO: zachowanie do wskazanego strumienia
// void propertiesSave(GOC_Properties*, OStream*);
//  Wyczy�� wszystkie kategorie i w�a�ciwo�ci
GOC_Properties* goc_propertiesClear(GOC_Properties*);
// Wyczy�� w�a�ciwo�ci wybranej kategorii
GOC_Properties* propertiesCategoryClear(GOC_Properties*, char*);


// wylistowanie w�a�ciwo�ci w domy�lnej kategorii
// return GOC_Iterator of GOC_Property
GOC_Iterator *goc_propertiesList(GOC_Properties*);
// wtlistowanie kategorii
// return GOC_Iterator of GOC_Category
GOC_Iterator *goc_propertiesListCategories(GOC_Properties*);
// wylistowanie w�a�ciwo�ci w kategorii
GOC_Iterator *goc_propertiesListCategory(GOC_Properties*, char*);
// podaj warto�� w�a�ciwo�ci dla klucza z domy�lnej kategorii
char *goc_propertiesGetValue(GOC_Properties*, char*);
// podaj warto�� w�a�ciwo�ci dla klucza z wybranej kategorii
char *goc_propertiesGetCategoryValue(GOC_Properties*, char*, char*);


GOC_Properties *goc_propertiesAddCategory(GOC_Properties*, char*);
GOC_Properties *propertiesSet(GOC_Properties*, char*, char*);
GOC_Properties *propertiesCategorySet(GOC_Properties*, char*, char*, char*);
GOC_Properties *propertiesRemCategory(GOC_Properties*, char*);

#endif // ifndef _GOC_PROPERTIES_H_
