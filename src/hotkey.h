#ifndef _GOC_HOTKEY_H_
#define _GOC_HOTKEY_H_

#include "global.h"

typedef struct GOC_StHotKey
{
	GOC_FLAGS flag;
	unsigned int key;
	GOC_FUN_LISTENER *func;
} GOC_StHotKey, *GOC_HOTKEY;

typedef struct GOC_StHotKeys
{
	GOC_HOTKEY *pHotKey;
	int nHotKey;
} GOC_StHotKeys, *GOC_HOTKEYS;

int goc_hkAdd(
	GOC_HANDLER uchwyt, unsigned int key, GOC_FLAGS flag, GOC_FUN_LISTENER *fun);
int goc_hkRemove(
	GOC_HANDLER uchwyt, const GOC_HOTKEY hotkey);
int goc_hkFlag(
	const GOC_HOTKEY hotkey, GOC_FLAGS flag);
int goc_hkFunc(
	const GOC_HOTKEY hotkey, GOC_FUN_LISTENER *fun);
GOC_HOTKEY goc_hkList(
	GOC_HANDLER uchwyt, const GOC_HOTKEY last);
GOC_HOTKEY goc_hkListKey(
	GOC_HANDLER uchwyt, const GOC_HOTKEY last, unsigned int key);
unsigned int goc_hkGetKey(GOC_HOTKEY hotkey);
GOC_FLAGS goc_hkGetFlag(GOC_HOTKEY flag);
GOC_FUN_LISTENER *goc_hkGetFunc(GOC_HOTKEY hotkey);
int goc_hkClear(
	GOC_HANDLER uchwyt, GOC_FLAGS flag);

#endif // ifndef _GOC_HOTKEY_H_
