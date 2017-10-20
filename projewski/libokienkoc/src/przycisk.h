#ifndef _GOC_BUTTON_H_
#define _GOC_BUTTON_H_

#include "element.h"

extern const char* GOC_ELEMENT_BUTTON;

#define GOC_STRUCT_BUTTON \
	GOC_STRUCT_ELEMENT; \
	char *tekst; \
	GOC_COLOR kolorRamka; \
	GOC_COLOR kolorRamkaAktywny; \
	GOC_COLOR kolorRamkaNieaktywny; \
	GOC_COLOR kolorPoleAktywny; \
	GOC_COLOR kolorPoleNieaktywny; \
	const char *typ

typedef struct GOC_StButton
{
	GOC_STRUCT_BUTTON;
} GOC_StButton;

#define GOC_BUTTONTYPE_BRACKET		"[ ][ ][ ]"
#define GOC_BUTTONTYPE_EMPTY			"         "
#define GOC_BUTTONTYPE_CHAR		"*-*| |*-*"
#define GOC_BUTTONTYPE_HASH			"#### ####"
#define GOC_BUTTONTYPE_SLASH			"/-\\| |\\-/"

int goc_buttonListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);
int goc_buttonSetText(GOC_HANDLER u, const char *tekst);

#endif // ifndef _GOC_BUTTON_H_
