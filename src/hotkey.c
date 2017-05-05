#include "global-inc.h"
#include "hotkey.h"
#include "element.h"
#include "tablica.h"
#ifndef _DEBUG
#	include <string.h>
#endif

// Klucze systemu !
static GOC_HOTKEYS _hotkeys = NULL;
// dodaj nowego hotkey'a do listy
// TOTHINK: Moze zwracaæ GOC_HOTKEY, jaki zostaje dodany do listy
GOC_HOTKEY goc_hkAdd(
	GOC_HANDLER uchwyt, unsigned int key, GOC_FLAGS flag, GOC_FUN_LISTENER *fun)
{
	GOC_HOTKEYS *hk;
	if ( uchwyt == GOC_HANDLER_SYSTEM )
		hk = &_hotkeys;
	else
		hk = &(((GOC_StElement*)uchwyt)->hotkeys);
	if ( !(*hk) )
	{
		*hk = malloc( sizeof(GOC_StHotKeys) );
		memset( *hk, 0, sizeof(GOC_StHotKeys) );
	}
	(*hk)->pHotKey = goc_tableAdd(
		(*hk)->pHotKey, &((*hk)->nHotKey), sizeof(GOC_HOTKEY) );
	GOC_HOTKEY hotkey = malloc(sizeof(GOC_StHotKey));
	(*hk)->pHotKey[(*hk)->nHotKey-1] = hotkey;
	hotkey->key = key;
	if ( !(flag & (GOC_HKFLAG_USER | GOC_HKFLAG_SYSTEM)) )
		flag |= GOC_HKFLAG_USER;
	hotkey->flag = flag;
	hotkey->func = fun;
	return hotkey;
}

int goc_hkClear(
	GOC_HANDLER uchwyt, GOC_FLAGS flag)
{
	GOC_HOTKEYS hk;
	int i;
	if ( uchwyt == GOC_HANDLER_SYSTEM )
		hk = _hotkeys;
	else
		hk = ((GOC_StElement*)uchwyt)->hotkeys;
	if ( !hk )
		return GOC_ERR_OK;
	flag &= GOC_HKFLAG_SYSTEM | GOC_HKFLAG_USER;
	for ( i=0; i<hk->nHotKey; i++ )
	{
		if ( hk->pHotKey[i]->flag & flag )
		{
			free( hk->pHotKey[i] );
			hk->pHotKey = goc_tableRemove(
				hk->pHotKey,
				&(hk->nHotKey),
				sizeof(GOC_HOTKEY),	i);
			i--;
		}
	}
	if ( hk->nHotKey == 0 )
	{
		free( hk );
		if ( uchwyt == GOC_HANDLER_SYSTEM )
			_hotkeys = NULL;
		else
			((GOC_StElement*)uchwyt)->hotkeys = NULL;
	}
	return GOC_ERR_OK;
}

// usun hotkey'a z listy
// jesli nie znajdzie na liscie key zwraca GOC_ERR_WRONGARGUMENT
int goc_hkRemove(
	GOC_HANDLER uchwyt, const GOC_HOTKEY hotkey)
{
	int i;
	GOC_HOTKEYS hk;
	if ( uchwyt == GOC_HANDLER_SYSTEM )
		hk = _hotkeys;
	else
		hk = ((GOC_StElement*)uchwyt)->hotkeys;
	if ( !hk )
		return GOC_ERR_WRONGARGUMENT;
	if ( !hotkey )
		return GOC_ERR_WRONGARGUMENT;
	for ( i=0; i<hk->nHotKey; i++ )
	{
		if ( hk->pHotKey[i] == hotkey )
		{
			free( hk->pHotKey[i] );
			hk->pHotKey = goc_tableRemove(
				hk->pHotKey,
				&hk->nHotKey,
				sizeof(GOC_HOTKEY), i);
			if ( hk->nHotKey == 0 )
			{
				free( hk );
				if ( uchwyt == GOC_HANDLER_SYSTEM )
					_hotkeys = NULL;
				else
					((GOC_StElement*)uchwyt)->hotkeys = NULL;
			}
			return GOC_ERR_OK;
		}
	}
	return GOC_ERR_WRONGARGUMENT;
}

// zmien flage hotkey'a na liscie
int goc_hkFlag(
	const GOC_HOTKEY hotkey, GOC_FLAGS flag)
{
	if ( !hotkey )
		return GOC_ERR_WRONGARGUMENT;
	hotkey->flag = flag;
	return GOC_ERR_OK;
}

// zmien funkcje hotkey'a na liscie
int goc_hkFunc(
	const GOC_HOTKEY hotkey, GOC_FUN_LISTENER *fun)
{
	if ( !hotkey )
		return GOC_ERR_WRONGARGUMENT;
	hotkey->func = fun;
	return GOC_ERR_OK;
}

// listowanie hotkey'ow w strukturze
// hotkey'e sa zwracane w kolejnosci od osatniego dodanego
// wskazanie na ostatni mowi, jaki hotkey byl zwrocony wczesniej
GOC_HOTKEY goc_hkList(
	GOC_HANDLER uchwyt, const GOC_HOTKEY last)
{
	GOC_HOTKEYS hk;
	GOC_HOTKEY prev = NULL;
	int i;
	if ( uchwyt == GOC_HANDLER_SYSTEM )
		hk = _hotkeys;
	else
		hk = ((GOC_StElement*)uchwyt)->hotkeys;
	if ( !hk )
		return NULL;
	for (i=hk->nHotKey-1; i>=0; i--)
	{
		if ( last == prev )
			return hk->pHotKey[i];
		else
			prev = hk->pHotKey[i];
	}
	return NULL;
}

GOC_HOTKEY goc_hkListKey(
	GOC_HANDLER uchwyt, const GOC_HOTKEY last, unsigned int key)
{
	GOC_HOTKEYS hk;
	GOC_HOTKEY prev = NULL;
	int i;
	if ( uchwyt == GOC_HANDLER_SYSTEM )
		hk = _hotkeys;
	else
		hk = ((GOC_StElement*)uchwyt)->hotkeys;
	if ( !hk )
		return NULL;
	for (i=hk->nHotKey-1; i>=0; i--)
	{
		if ( hk->pHotKey[i]->key == key )
		{
			if ( prev == last )
				return hk->pHotKey[i];
			else
				prev = hk->pHotKey[i];
		}
	}
	return NULL;
}

// podaj klawisz jaki ustawiony jest dla danego hotkey
unsigned int goc_hkGetKey(GOC_HOTKEY hotkey)
{
	if ( !hotkey )
		return 0;
	else
		return hotkey->key;
}

// podaj flagi ustawione dla danego hotkeya
GOC_FLAGS goc_hkGetFlag(GOC_HOTKEY hotkey)
{
	if ( !hotkey )
		return 0;
	else
		return hotkey->flag;
}

// podaj funkcje ustawiona dla danego hotkeya
GOC_FUN_LISTENER *goc_hkGetFunc(GOC_HOTKEY hotkey)
{
	if ( !hotkey )
		return 0;
	else
		return hotkey->func;
}
