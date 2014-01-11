#include "arguments.h"
#include "log.h"
#include "screen.h"
#include "mystr.h"
#include "global.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

static GOC_Argument* goc_argumentAlloc()
{
	GOC_Argument* tmp = malloc(sizeof(GOC_Argument));
	memset(tmp, 0, sizeof(GOC_Argument));
	return tmp;
}

static GOC_Argument* goc_argumentFree(GOC_Argument* arg)
{
	if ( arg )
	{
		int i;
		for ( i=0; i<arg->nName; i++ )
			arg->pName[i] = goc_stringFree(arg->pName[i]);
		arg->pName = goc_tableClear(arg->pName, &arg->nName);
		arg->description = goc_stringFree(arg->description);
		arg->parameter = NULL;
		arg->function = NULL;
		free( arg );
	}
	return NULL;
}

GOC_Arguments* goc_argumentsAlloc()
{
	GOC_Arguments* res = NULL;
	res = (GOC_Arguments*)malloc(sizeof(GOC_Arguments));
	memset(res, 0, sizeof(GOC_Arguments));
	return res;
}

GOC_Arguments* goc_argumentsFree(
		GOC_Arguments* args)
{
	if ( args )
	{
		int i;
		for ( i=0; i<args->nArgument; i++ )
			args->pArgument[i] = goc_argumentFree( args->pArgument[i] );
		args->progDescription = goc_stringFree( args->progDescription );
		args->paramDescription = goc_stringFree( args->paramDescription );
		args->author = goc_stringFree( args->author );
		args->unknownFunction = NULL;
		free( args );
	}
	return NULL;
}

GOC_Arguments* goc_argumentsAdd(
	GOC_Arguments* args,
       	char* name,
       	char* description,
       	GOC_ArgumentFunction function,
       	void* parameter)
{
	GOC_Argument *arg = NULL;
	int i;
	if ( ! args )
		args = goc_argumentsAlloc();
	// czy istnieje juz taki argument (ta sama funkcja i modyfikowany parametr)
	// wtedy dodaj tylko nowa nazwe
	for ( i=0; i<args->nArgument; i++ )
	{
		arg = args->pArgument[i];
		if ( arg->parameter != parameter )
			continue;
		if ( arg->function != function )
			continue;
		arg->pName = goc_tableAdd(arg->pName, &arg->nName, sizeof(char*));
		arg->pName[arg->nName-1] = goc_stringCopy(NULL, name);
		return args;
	}
	// dodajemy nowy argument
	arg = goc_argumentAlloc();
	arg->pName = goc_tableAdd(arg->pName, &arg->nName, sizeof(char*));
	arg->pName[arg->nName-1] = goc_stringCopy(NULL, name);
	arg->description = goc_stringCopy(arg->description, description);
	arg->function = function;
	arg->parameter = parameter;
	args->pArgument = goc_tableAdd(args->pArgument, &args->nArgument, sizeof(GOC_Argument*));
	args->pArgument[args->nArgument-1] = arg;
	return args;
}

GOC_Arguments* goc_argumentsSetUnknownFunction(
	GOC_Arguments* args,
	GOC_ArgumentFunction fun)
{
	if ( ! args )
		args = goc_argumentsAlloc();
	args->unknownFunction = fun;
	return args;
}

// Jako czwarty argument przyjmuje strukturê GOC_Arguments
int goc_argumentHelpFunction(char **argv, int pos, int argc, void* param)
{
	GOC_Arguments* args = *((GOC_Arguments**)param);
	int i, j;
	char *tmpString = NULL;

	if ( args == NULL )
	{
		GOC_ERROR("No GOC_Arguments at parameter");
		return 0;
	}
	tmpString = goc_stringMultiCopy(tmpString,
		GOC_STRING_WHITE, "Usage: ", GOC_STRING_GREEN, argv[0],
	       	GOC_STRING_WHITE, " ", args->paramDescription, "\n",
		"\t", args->progDescription, "\n");
	for (i=0; i<args->nArgument; i++)
	{
		GOC_Argument* arg = args->pArgument[i];
		if ( arg == NULL )
			continue;
		tmpString = goc_stringAdd(tmpString, "\t");
		for ( j = 0; j < arg->nName; j++ )
		{
			tmpString = goc_stringMultiAdd(tmpString, j ? " | " : NULL, arg->pName[j]);
		}
		tmpString = goc_stringMultiAdd(tmpString, "\n\t\t", arg->description, "\n");
	}
	tmpString = goc_stringMultiAdd(tmpString,
		"\t\n\t\t\tCopyright: ",
	       	GOC_STRING_CYAN, args->author ? args->author : GOC_AUTHOR,
	       	GOC_STRING_WHITE, "\n");
	fputs(tmpString, stdout);
	tmpString = goc_stringFree(tmpString);
	// return 0 - it's sign for finish program and parsing
	return 0;
}

int goc_argumentsParse(
	GOC_Arguments* args,
	int argc,
	char** argv)
{
	int pos;
	for (pos=1; pos < argc; )
	{
		int res = 0;
		int i;
		unsigned char interpret = 0;
		for (i=0; i < args->nArgument; i++ )
		{
			// find a name of an agrument
			int j;
			for (j=0; j < args->pArgument[i]->nName; j++ )
			{
				if ( goc_stringEquals(args->pArgument[i]->pName[j], argv[pos]) )
				{
					res = args->pArgument[i]->function(argv, pos, argc, args->pArgument[i]->parameter);
					interpret = 1;
					break;
				}
			}
			if ( interpret )
				break;
		}

		if (( !interpret ) && ( args->unknownFunction ))
		{
			args->unknownFunction(argv, pos, argc, &args);
		}

		if ( res )
		{
			pos += res;
		}
		else
		{
			GOC_BERROR("Cannot interpret argument %s", argv[pos]);
			return -1;
		}
	}
	return 0;
}

// TODO: FUNCTIONS NOW ARE WITHOUT ANY KIND OF CHECK AND VERIFY !

int goc_argumentIntFunction(char **argv, int pos, int argc, void* param)
{
	*((int*)param) = atoi(argv[pos+1]);
	return 2;
}

int goc_argumentStringFunction(char **argv, int pos, int argc, void* param)
{
	*((char**)param) = goc_stringCopy(NULL, argv[pos+1]);
	return 2;
}

int goc_argumentVoidFunction(char **argv, int pos, int argc, void* param)
{
	*((unsigned char*)param) = 1;
	return 1;
}
