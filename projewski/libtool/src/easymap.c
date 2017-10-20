#include "easymap.h"

// klucze i wartosci nie sa kopiowane !

GOC_EasyMap *goc_mapCreate()
{
	GOC_EasyMap *tmp = malloc(sizeof(GOC_EasyMap));
	memset(tmp, 0, sizeof(GOC_EasyMap));
	return tmp;
}

/*
typedef struct GOC_EasyMap
{
	GOC_MAPKEYTYPE *pKey;
	GOC_MAPVALUETYPE *pValue;
	_GOC_TABEL_SIZETYPE_ nMap;
	int (keyCompare*)(GOC_MAPKEYTYPE keyA, GOC_MAPKEYTYPE keyB);
	GOC_MAPKEYTYPE (keyFree*)(GOC_MAPKEYTYPE key);
	GOC_MAPVALUETYPE (valueFree*)(GOC_MAPVALUETYPE value);
} GOC_EasyMap;
*/

GOC_EasyMap *goc_mapFree(GOC_EasyMap *map)
{
	int i;

	if ( map == NULL )
		return map;

	if ( map->keyFree )
	{
		for ( i=0; i<map->nMap; i++ )
			map->pKey[i] = map->keyFree( map->pKey[i] );
	}

	if ( map->valueFree )
	{
		for ( i=0; i<map->nMap; i++ )
			map->pValue[i] = map->valueFree( map->pValue[i] );
	}

	i = map->nMap;
	map->pKey = goc_tableClear(map->pKey, &(map->nMap));
	map->nMap = i;
	map->pValue = goc_tableClear(map->pValue, &(map->nMap));
	free(map);
	return NULL;
}

GOC_EasyMap *goc_mapKeyCmpFunction(GOC_EasyMap *map, int (fun*)(GOC_MAPKEYTYPE, GOC_MAPKEYTYPE))
{
	if ( map )
		map->keyCompare = fun;
	return map;
}

GOC_EasyMap *goc_mapKeyFreeFunction(GOC_EasyMap *map, GOC_MAPKEYTYPE (fun*)(GOC_MAPKEYTYPE))
{
	if ( map )
		map->keyFree = fun;
	return map;
}

GOC_EasyMap *goc_mapValueFreeFunction(GOC_EasyMap *map, MAPVALUE (fun*)(MAPVALUE))
{
	if ( map )
		map->valueFree = fun;
	return map;
}

static int getKeyPos(GOC_EasyMap *map, GOC_MAPKEYTYPE key)
{
	int i;

	if ( map->keyCompare )
	{
		for ( i=0; i<map->nMap; i++ )
		{
			if ( map->keyCompare( map->pKey[i], key ) == 0 )
				return i;
		}
	}
	else
	{
		for ( i=0; i<map->nMap; i++ )
		{
			if ( map->pKey[i] == key )
				return i;
		}
	}
	return -1;
}

GOC_EasyMap *goc_mapSet(GOC_EasyMap *map, GOC_MAPKEYTYPE key, MAPKEYVALUE value)
{
	int i;
	if ( map == NULL )
		return map;
	i = getKeyPos( key );
	if ( i == -1 )
	{
		i = map->nMap;
		map->pKey = goc_tableAdd(
			map->pKey, &(map->nMap), sizeof(GOC_MAPKEYTYPE));
		map->nMap--;
		map->pKey[i] = key;
		map->pValue = goc_tableAdd(
			map->pValue, &(map->nMap), sizeof(GOC_MAPVALUETYPE));
	}
	else
	{
		if ( map->valueFree )
			map->pValue[i] = map->valueFree( map->pValue[i] );
	}
	map->pValue[i] = value;
	return map;
}

GOC_MAPVALUETYPE goc_mapGet(GOC_EasyMap *map, MAPKEYTUPE key)
{
	int i;
	if ( map )
	{
		i = getKeyPos( map, key );
		if ( i != -1 )
			return map->pValue[i];
	}
	return NULL;
}
