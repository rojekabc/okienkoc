#ifndef _GOC_ELEMENT_H_
#define _GOC_ELEMENT_H_

#include "global.h"
#include "hotkey.h"
#include "tablica.h"
#include "baseelement.h"

#define GOC_STRUCT_ELEMENT \
	GOC_STRUCT_BASEELEMENT; \
	GOC_HANDLER ojciec; \
	GOC_POSITION x; \
	GOC_POSITION y; \
	GOC_POSITION width; \
	GOC_POSITION height; \
	GOC_COLOR color; \
	GOC_FLAGS flag; \
	GOC_HOTKEYS hotkeys

typedef struct GOC_StElement
{
	GOC_STRUCT_ELEMENT;
} GOC_StElement;

#define GOC_CREATE_ELEMENT(_struct_, _variable_, _msg_) \
	_struct_* _variable_ = (_struct_*)malloc(sizeof(_struct_)); \
	memset(_variable_, 0, sizeof(_struct_)); \
	memcpy(_variable_, &((GOC_StMsgCreate*)_msg_)->element, sizeof(GOC_StElement)); \
	((GOC_StMsgCreate*)_msg_)->created = (GOC_HANDLER)_variable_

int goc_elementSetColor(GOC_HANDLER u, GOC_COLOR k);
int goc_elementSetX(GOC_HANDLER u, GOC_POSITION x);
int goc_elementSetY(GOC_HANDLER u, GOC_POSITION y);
int goc_elementSetWidth(GOC_HANDLER u, GOC_POSITION d);
int goc_elementSetHeight(GOC_HANDLER u, GOC_POSITION w);
GOC_COLOR goc_elementGetColor(GOC_HANDLER u);
int goc_elementSetFlag(GOC_HANDLER u, GOC_FLAGS f);
GOC_FLAGS goc_elementGetFlag(GOC_HANDLER u);
int goc_elementOrFlag(GOC_HANDLER u, GOC_FLAGS f);
int goc_elementIsFlag(GOC_HANDLER u, GOC_FLAGS f);
int goc_elementDestroy(GOC_HANDLER u);
int goc_elementSetFunc(GOC_HANDLER uchwyt, const char *funId, void *fun);
void *goc_elementGetFunc(GOC_HANDLER uchwyt, const char *funkcjaId);
GOC_POSITION goc_elementGetScreenX(GOC_HANDLER u);
GOC_POSITION goc_elementGetScreenY(GOC_HANDLER u);
GOC_POSITION goc_elementGetHeight(GOC_HANDLER u);
GOC_POSITION goc_elementGetWidth(GOC_HANDLER u);

void goc_elementRegister();

#endif // ifndef _GOC_ELEMENT_H_
