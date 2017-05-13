#ifndef _GOC_LABEL_H_
#define _GOC_LABEL_H_

#include "element.h"
#include "tablica.h"

extern const char* GOC_ELEMENT_LABEL;

typedef struct GOC_StColorLine
{
	char *line;
	GOC_COLOR color;
	GOC_FLAGS flag;
} GOC_StColorLine;

typedef int (GOC_FUN_LABELDRAWER)(GOC_StElement *elem, const char *tekst);

#define GOC_STRUCT_LABEL \
	GOC_STRUCT_ELEMENT; \
	GOC_StColorLine *tekst; \
	_GOC_TABEL_SIZETYPE_ nText; \
	unsigned short start

typedef struct GOC_StLabel
{
	GOC_STRUCT_LABEL;
} GOC_StLabel;

int goc_labelListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);
int goc_labelAddLine(GOC_HANDLER u, const char *Tekst);
int goc_labelSetFlag(GOC_HANDLER u, unsigned short line, GOC_FLAGS flag);
int goc_labelSetColor(GOC_HANDLER u, unsigned short line, GOC_COLOR color);
GOC_COLOR goc_labelGetColor(GOC_HANDLER u, unsigned short line);
int goc_labelDrawer(GOC_StElement *elem, const char *tekst);
int goc_labelSetText(GOC_HANDLER u, char *tekst, GOC_BOOL licz);
int goc_labelRemLines(GOC_HANDLER u);
int goc_labelRemLine(GOC_HANDLER u, unsigned int numer);
GOC_FUN_LABELDRAWER *goc_elementGetLabelDrawer(GOC_HANDLER uchwyt);

#endif // ifndef _GOC_LABEL_H_
