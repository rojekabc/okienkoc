#ifndef _GOC_EASYMAP_H_
#define _GOC_EASYMAP_H_

#include "tablica.h"

#define GOC_MAPKEYTYPE void*
#define GOC_MAPVALUETYPE void*

typedef struct GOC_EasyMap
{
	GOC_MAPKEYTYPE *pKey;
	GOC_MAPVALUETYPE *pValue;
	_GOC_TABEL_SIZETYPE_ nMap;
	int (keyCompare*)(GOC_MAPKEYTYPE keyA, GOC_MAPKEYTYPE keyB);
	GOC_MAPKEYTYPE (keyFree*)(GOC_MAPKEYTYPE key);
	GOC_MAPVALUETYPE (valueFree*)(GOC_MAPVALUETYPE value);
} GOC_EasyMap;

GOC_EasyMap *goc_mapCreate();
GOC_EasyMap *goc_mapFree(GOC_EasyMap *map);
GOC_EasyMap *goc_mapKeyCmpFunction(GOC_EasyMap *map, int (fun*)(GOC_MAPKEYTYPE, GOC_MAPKEYTYPE));
GOC_EasyMap *goc_mapKeyFreeFunction(GOC_EasyMap *map, GOC_MAPKEYTYPE (fun*)(GOC_MAPKEYTYPE));
GOC_EasyMap *goc_mapValueFreeFunction(GOC_EasyMap *map, MAPVALUE (fun*)(MAPVALUE));
GOC_EasyMap *goc_mapSet(GOC_EasyMap *map, GOC_MAPKEYTYPE key, MAPKEYVALUE value);
GOC_MAPVALUETYPE goc_mapGet(GOC_EasyMap *map, MAPKEYTUPE key);

#endif // ifndef _GOC_EASYMAP_H_
