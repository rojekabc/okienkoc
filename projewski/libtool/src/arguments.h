#ifndef _GOC_ARGUMENTS_H_
#	define _GOC_ARGUMENTS_H_
#	include "tablica.h"

typedef int (*GOC_ArgumentFunction)(char **, int, int, void*);

#	define GOC_STRUCT_ARGUMENT \
		char** pName; \
		_GOC_TABEL_SIZETYPE_ nName; \
		char* description; \
		void* parameter; \
		GOC_ArgumentFunction function


typedef struct GOC_Argument
{
	GOC_STRUCT_ARGUMENT;
} GOC_Argument;

#	define GOC_STRUCT_ARGUMENTS \
		GOC_Argument **pArgument; \
		_GOC_TABEL_SIZETYPE_ nArgument; \
		char* progDescription; \
		char* paramDescription; \
		char *author; \
		GOC_ArgumentFunction unknownFunction

typedef struct GOC_Arguments
{
	GOC_STRUCT_ARGUMENTS;
} GOC_Arguments;

GOC_Arguments* goc_argumentsAlloc();

GOC_Arguments* goc_argumentsFree(
		GOC_Arguments*);

GOC_Arguments* goc_argumentsAdd(
	GOC_Arguments*,
       	char* name,
       	char* description,
       	GOC_ArgumentFunction function,
       	void* parameter);

GOC_Arguments* goc_argumentsSetUnknownFunction(
	GOC_Arguments*,
	GOC_ArgumentFunction);

int goc_argumentsParse(
	GOC_Arguments*,
	int argc,
	char** argv);

// GOC_AergumentFunction
// Jako czwarty argument przyjmuje strukture GOC_Arguments
int goc_argumentHelpFunction(char **, int, int, void*);
int goc_argumentIntFunction(char **, int, int, void*);
int goc_argumentStringFunction(char **, int, int, void*);
int goc_argumentVoidFunction(char **, int, int, void*);

#endif // ifndef _GOC_ARGUMENTS_H_
