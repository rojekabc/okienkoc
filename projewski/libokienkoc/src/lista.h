#ifndef _GOC_LIST_H_
#define _GOC_LIST_H_

#include "element.h"

/** struktury **/
typedef struct GOC_StColumn
{
	char **pText;
	int nText;
	GOC_POSITION position;
} GOC_COLUMN;

typedef struct GOC_StListRow
{
	const char **pText; // teksty do kolumn - kolejnych
	int nText; // liczba zamieszczonych tekstow
	int nRow; // jesli -1 ustawi po wykonaniu funkcji nadana
	// pozcje, jesli != -1 to umiesci we wskazanej pozcji.
} GOC_LISTROW;

/** id elementu **/
extern const char* GOC_ELEMENT_LIST;

/**
 * Change selection on lust.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	const char* pText;
	int position;
} GOC_StMsgListChange;
#define GOC_MSG_LISTCHANGE(variable, _text_, _position_) \
	GOC_MSG(GOC_StMsgListChange, variable, GOC_MSG_LISTCHANGE_ID); \
	variable##Full.pText = _text_; \
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
	const char* pText;
} GOC_StMsgListAddText;
#define GOC_MSG_LISTADDTEXT(variable, _text_) \
	GOC_MSG(GOC_StMsgListAddText, variable, GOC_MSG_LISTADDTEXT_ID); \
	variable##Full.pText = _text_
extern const char* GOC_MSG_LISTADDTEXT_ID;
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
/*
 * Append row.
 */
#define GOC_MSG_LISTADDROW(variable, _row_) \
	GOC_MSG(GOC_StMsgListAddRow, variable, GOC_MSG_LISTADDROW_ID); \
	variable##Full.pRow = _row_
typedef struct {
	GOC_STRUCT_MESSAGE;
	GOC_LISTROW* pRow;
} GOC_StMsgListAddRow;
extern const char* GOC_MSG_LISTADDROW_ID;


#define GOC_STRUCT_LIST \
	GOC_STRUCT_ELEMENT; \
	GOC_COLOR kolorRamka; \
	GOC_COLOR kolorRamkaNieaktywny; \
	GOC_COLOR kolorRamkaAktywny; \
	GOC_COLOR kolorPoleAktywny; \
	GOC_COLOR kolorPoleNieaktywny; \
	GOC_COLOR kolorPoleKursor; \
	GOC_COLUMN **pKolumna; \
	char *pTytul; \
	int nKolumna; \
	int kursor; \
	int start

typedef struct GOC_StList
{
	GOC_STRUCT_LIST;
} GOC_StList;

int goc_listListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);
int goc_listAddText(GOC_HANDLER u, const char *tekst);
int goc_listAddColumnText(GOC_HANDLER u, const char *tekst, int kolumna);
int goc_listSetColumnText(GOC_HANDLER u, const char *tekst, int kolumna);
int goc_listAddColumn(GOC_HANDLER u, GOC_POSITION width);
int goc_listSetCursor(GOC_HANDLER uchwyt, int pos);
#define goc_listGetCursor(uchwyt) ((GOC_StList*)uchwyt)->kursor
char *goc_listGetUnderCursor(GOC_HANDLER u);
int goc_listClear(GOC_HANDLER uchwyt);
int goc_listSetExtTable(
	GOC_HANDLER uchwyt, const char **pTable, unsigned int size);
const char *goc_listGetText(GOC_HANDLER u, int pos);
int goc_listGetRowsAmmount(GOC_HANDLER u);
int goc_listFindText(GOC_HANDLER u, const char *tekst);
int goc_listSetTitle(GOC_HANDLER u, const char *tekst);
#define goc_listGetTitle(uchwyt) ((GOC_StList*)uchwyt)->pTytul
const char *goc_listGetColumnText(GOC_HANDLER u, int pos, int kol);

#endif // ifndef _GOC_LIST_H_
