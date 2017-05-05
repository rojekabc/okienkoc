#ifndef _GOC_FRAME_H_
#define _GOC_FRAME_H_

#include "element.h"

extern const char *GOC_ELEMENT_FRAME;

#define GOC_STRUCT_FRAME \
	GOC_STRUCT_ELEMENT; \
	char znak[9]

typedef int (GOC_FUN_FRAMEDRAWER)(GOC_StElement *ramka, const char *znak);
typedef struct GOC_StFrame
{
	GOC_STRUCT_FRAME;
} GOC_StFrame;

int goc_frameListener(GOC_HANDLER, GOC_MSG, void*, uintptr_t);
int goc_frameSetPoints(GOC_HANDLER u, char *p);
int goc_frameDrawer(GOC_StElement *ramka, const char *znak);
GOC_FUN_FRAMEDRAWER *goc_elementGetFrameDrawer(GOC_HANDLER uchwyt);

#endif // ifndef _GOC_FRAME_H_
