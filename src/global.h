#ifndef _GOC_GLOBAL_H_
#define _GOC_GLOBAL_H_

// GOC_ENCODING - dla projektu fob
#define GOC_ENCODING "iso-8859-2"
#define GOC_AUTHOR "Piotr Rojewski <piotr4rojewski@gmail.com>"

// typedef unsigned int GOC_HANDLER;
#include <stdint.h>
typedef uintptr_t GOC_HANDLER;
typedef const char* GOC_MSG;
typedef const char* GOC_IDENTIFIER;
typedef short GOC_POSITION;
//typedef unsigned char GOC_POSITION;
typedef unsigned char GOC_COLOR;
typedef unsigned int GOC_FLAGS;
typedef unsigned char GOC_BOOL;
typedef int (GOC_FUN_LISTENER)(GOC_HANDLER, GOC_MSG, void*, uintptr_t);

#define GOC_TRUE 1
#define GOC_FALSE 0

#define GOC_HANDLER_SYSTEM 0

/*
 * Funkcja:
 *
 *
 * Argumenty:
 *
 *
 * Zwraca:
 *
 *
 */
#define GOC_ERR_OK 0
#define GOC_ERR_UNKNOWNMSG 1
#define GOC_ERR_UNKNOWNID 2
#define GOC_ERR_CREATE 3
#define GOC_ERR_WRONGARGUMENT 4
#define GOC_ERR_END 5
#define GOC_ERR_REFUSE 6
#define GOC_ERR_FALSE 7
#define GOC_ERR_PARENT 8
#define GOC_ERR_NEXT 9
/*
 * flag dzieli siê na dwie czê¶ci:
 * starsza 0xFFFF0000, w której s± ustawienia dla ka¿dego elementu
 * odpowiednich flag
 * m³odsza 0x0000FFFF, w której s± ustawienia charakterystyczne dla tego
 * elementu
 */
#define GOC_EFLAGA_PAINTED	0x80000000
#define GOC_EFLAGA_ENABLE	0x40000000
#define GOC_EFLAGA_SHOWN	0x20000000
/*
 * Flagi dla napisu
 */
// Wyrównanie: ostatnie dwa bity
#define COC_EFLAGA_G_JUSTIFY	0x00000003
#define GOC_EFLAGA_LEFT		0x00000000
#define GOC_EFLAGA_CENTER		0x00000001
#define GOC_EFLAGA_RIGHT		0x00000002
// Flagi dla listy
#define GOC_LISTFLAG_SORT	0x00000010
#define GOC_LISTFLAG_NOREPEAT	0x00000020
#define GOC_LISTFLAG_EXTERNAL	0x00000040
// Flagi dla formy
#define GOC_FORMFLAGFOCUS	0x00000010
// Flagi dla wpisu
#define GOC_FLG_EDITSELECTED	0x00000010
// Flagi dla HotKey
#define GOC_HKFLAG_SYSTEM		0x00000010
#define GOC_HKFLAG_USER	0x00000020
// Flagi dla plisty
#define GOC_FILELISTFLAG_FOLDER	0x00000010

// Identyfikatory zarejestrowanych typow funkcji uzywanych przez elementy
// jak zewnetrzne
extern const char *GOC_FUNID_LISTENER;
extern const char *GOC_FUNID_LABELDRAWER;
extern const char *GOC_FUNID_FRAMEDRAWER;

#endif // ifndef _GOC_GLOBAL_H_
