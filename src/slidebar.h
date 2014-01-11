#ifndef _GOC_SLIDEBAR_H_
#define _GOC_SLIDEBAR_H_

#include "element.h"

extern const char* GOC_ELEMENT_SLIDEBAR;

#define GOC_STRUCT_SLIDEBAR \
	GOC_STRUCT_ELEMENT; \
	GOC_POSITION position; \
	char character; \
	char emptycharacter;

// slidebar element
//
// When focused it allow to move its pointer position.
// Change position of pointer make its own pointer.
	
typedef struct GOC_StSlideBar
{
	GOC_STRUCT_SLIDEBAR;
} GOC_StSlideBar;

int goc_slidebarListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf);
int goc_slidebarSetPosition(GOC_HANDLER h, GOC_POSITION position);
int goc_slidebarSetChar(GOC_HANDLER h, char ch, char emptych);
GOC_POSITION goc_slidebarGetPosition(GOC_HANDLER h);

#endif // ifndef _GOC_SLIDEBAR_H_
