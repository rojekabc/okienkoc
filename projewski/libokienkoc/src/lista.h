#ifndef _GOC_LIST_H_
#define _GOC_LIST_H_

#include "element.h"
#include <tools/array.h>

/** struktury **/
typedef struct GOC_StColumn
{
	struct GOC_Array elements;
	GOC_POSITION position;
} GOC_COLUMN;

/** id elementu **/
extern const char* GOC_ELEMENT_LIST;

/**
 * Change selection on lust.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	void* element;
	int position;
} GOC_StMsgListChange;
#define GOC_MSG_LISTCHANGE(variable, _element_, _position_) \
	GOC_MSG(GOC_StMsgListChange, variable, GOC_MSG_LISTCHANGE_ID); \
	variable##Full.element = _element_; \
	variable##Full.position = _position_
extern const char* GOC_MSG_LISTCHANGE_ID;
/*
 * Get color.to set for chosen position.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	int position;
	GOC_COLOR color;
} GOC_StMsgListSetColor;
#define GOC_MSG_LISTSETCOLOR(variable, _position_) \
	GOC_MSG(GOC_StMsgListSetColor, variable, GOC_MSG_LISTSETCOLOR_ID); \
	variable##Full.position = _position_
extern const char* GOC_MSG_LISTSETCOLOR_ID;
/**
 * Add text to list.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	void* element;
} GOC_StMsgListAdd;
#define GOC_MSG_LISTADD(variable, _element_) \
	GOC_MSG(GOC_StMsgListAdd, variable, GOC_MSG_LISTADD_ID); \
	variable##Full.element = _element_
extern const char* GOC_MSG_LISTADD_ID;
/*
 * Remove all elements.
 */
#define GOC_MSG_LISTCLEAR(variable) GOC_MSG(GOC_StMessage, variable, GOC_MSG_LISTCLEAR_ID)
extern const char* GOC_MSG_LISTCLEAR_ID;
/**
 * Set data from external source.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	const char** pTextArray;
	unsigned int nTextArray;
} GOC_StMsgListSetExternal;
#define GOC_MSG_LISTSETEXTERNAL(variable, _textarray_, _countarray_) \
	GOC_MSG(GOC_StMsgListSetExternal, variable, GOC_MSG_LISTSETEXTERNAL_ID); \
	variable##Full.pTextArray = _textarray_; \
	variable##Full.nTextArray = _countarray_
extern const char* GOC_MSG_LISTSETEXTERNAL_ID;

#define GOC_STRUCT_LIST \
	GOC_STRUCT_ELEMENT; \
	GOC_COLOR kolorRamka; \
	GOC_COLOR kolorRamkaNieaktywny; \
	GOC_COLOR kolorRamkaAktywny; \
	GOC_COLOR kolorPoleAktywny; \
	GOC_COLOR kolorPoleNieaktywny; \
	GOC_COLOR kolorPoleKursor; \
	struct GOC_Array columns; \
	char* pTitle; \
	int kursor; \
	int start; \
	const char* (*elementToText)(void*); \
	void (*elementFree)(void*)

typedef struct GOC_StList
{
	GOC_STRUCT_LIST;
} GOC_StList;

int goc_listListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);

int goc_listAdd(GOC_HANDLER u, void* element);
int goc_listAddToColumn(GOC_HANDLER u, void* element, int column);
int goc_listSetToColumn(GOC_HANDLER u, void* element, int column);
int goc_listInsertInColumn(GOC_HANDLER u, void* element, int column, int row);
void* goc_listGet(GOC_HANDLER u);
void* goc_listGetAt(GOC_HANDLER u, int row, int column);
int goc_listRemove(GOC_HANDLER u, int row);

int goc_listAddText(GOC_HANDLER u, const char *tekst);
int goc_listAddTextToColumn(GOC_HANDLER u, const char *tekst, int kolumna);
int goc_listSetTextToColumn(GOC_HANDLER u, const char *tekst, int kolumna);
const char* goc_listGetText(GOC_HANDLER u);
const char *goc_listGetTextAt(GOC_HANDLER u, int row, int column);

int goc_listAddColumn(GOC_HANDLER u, GOC_POSITION width);
int goc_listSetCursor(GOC_HANDLER uchwyt, int pos);
#define goc_listGetCursor(uchwyt) ((GOC_StList*)uchwyt)->kursor

int goc_listClear(GOC_HANDLER uchwyt);
int goc_listSetExtTable(
	GOC_HANDLER uchwyt, const char **pTable, unsigned int size);
int goc_listGetRowsAmmount(GOC_HANDLER u);
int goc_listFindText(GOC_HANDLER u, const char *tekst);

int goc_listSetTitle(GOC_HANDLER u, const char *tekst);
#define goc_listGetTitle(uchwyt) ((GOC_StList*)uchwyt)->pTytul
int goc_listSetElementFreeFunction(GOC_HANDLER handler, void (*free)(void*));

#endif // ifndef _GOC_LIST_H_
