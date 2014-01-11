#include "global-inc.h"
#include "global.h"
#include "mystr.h"
#include "plik.h"
#ifndef _DEBUG
#	include <string.h>
#endif

#include <libxml/parser.h>

#include <iconv.h>

#include "conffile.h"
#include "err.h"
#include "fobclass.h"
//#include "objectrequest.h"

static int writeObjectToOpenedFile(FILE *file, GOC_OBJECTHANDLER object)
{
	GOC_FUNCTIONRETURN res;
	const char *classname;
	res = getObjectClass(object);
	if ( res.code != GOC_ERR_OK )
		return res.code;
	classname = (const char *)res.result;
	fprintf(file, "<%s>", classname);
	res = call(object, "serialize", file, 0);
	if ( res.code != GOC_ERR_OK )
		printf("%s:%d:  Error: %s\n", __FILE__, __LINE__, res.description);
	fprintf(file, "</%s>\n", classname);
	return 0;
}

static int copyFun(GOC_OBJECTHANDLER fobj, void *arg)
{
	FILE *file = (FILE*)arg;
	writeObjectToOpenedFile(file, fobj);
	return 1;
}

/*
 * Obsluga plikow konfiguracyjnych w ogolnym schemacie
 * Brak obecnie przystosowania do wartosci atrybutow ze znakami nowej linii
 */
int writeObjectToFile(const char *filename, GOC_OBJECTHANDLER object)
{
	FILE *file = NULL;
	char *tmpfilename = NULL;

	if ( filename == NULL )
		return GOC_ERR_WRONGARGUMENT + 0;
	if ( object == NULL )
		return GOC_ERR_WRONGARGUMENT + 1;
	if ( !goc_isFileExists( filename ) )
	{
		file = fopen( filename, "w" );
		if ( file == NULL )
			return -1; // TODO: Error openong file
	}
	else
	{
		tmpfilename = tempnam(".", ".tmp_");
		file = fopen( tmpfilename, "w" );
		if ( file == NULL )
			return -1; // TODO: Error openong file
	}

	fprintf(file, "<?xml version='1.0' encoding='%s' ?>\n", GOC_ENCODING);
	fprintf(file, "<FileObjectDoc>\n");
	// copy existing elements
	if ( tmpfilename )
		listObjectsInFile(filename, copyFun, file);
	// add new object
	writeObjectToOpenedFile( file, object );
	// close
	fprintf(file, "</FileObjectDoc>\n");
	fclose(file);
	if ( tmpfilename )
		rename(tmpfilename, filename);
	return 0;
}

static char *convert(const xmlChar *ptr, int len)
{
	iconv_t icv;
	unsigned int inlen = len;
	char *inbuf = (char *)ptr;
	unsigned int outlen = len + 1;
	char *outbuf = malloc( outlen );
	char *retptr = outbuf;

	memset( outbuf, 0, outlen );

	icv = iconv_open( GOC_ENCODING, "utf-8" );
	if ( icv == (iconv_t)(-1) )
	{
		fprintf(stderr, "Cannot open iconv handler\n");
		free( retptr );
		return NULL;
	}
	if ( iconv( icv, &inbuf, &inlen, &outbuf, &outlen ) == -1 )
	{
		fprintf(stderr, "Error while converting\n");
		/*
		if ( errno == E2BIG )
			fprintf( stderr, "E2BIG\n" );
		else if ( errno == EILSEQ )
			fprintf( stderr, "EILSEQ\n" );
		else if ( errno == EINVAL )
			fprintf( stderr, "EINVAL\n" );
		*/
		free( retptr );
		return NULL;
	}
	iconv_close( icv );

	return retptr;
}

typedef struct GOC_RemStruct
{
	FILE *file;
	GOC_OBJECTHANDLER filter;
	int counter;
} GOC_RemStruct;

static int removeFun(GOC_OBJECTHANDLER fobj, void *arg)
{
/*
	GOC_RemStruct *remSt = (GOC_RemStruct*)arg;
	// zapisuj pliki, które nie spe³niaj± filtru
	if ( objectRequestCheckFileObject(remSt->filter, fobj) == 0 )
		writeObjectToOpenedFile(remSt->file, fobj);
	else
		remSt->counter++;
	return 1;
*/
	return 1;
}

int removeObjectsInFile(
	const char *filename,
	GOC_OBJECTHANDLER filter)
{
	int result = -1;
	FILE *file = NULL;
	char *tmpfilename = NULL;

	if ( filename == NULL )
		return GOC_ERR_WRONGARGUMENT + 0;
	if ( !goc_isFileExists( filename ) )
	{
		file = fopen( filename, "w" );
		if ( file == NULL )
			return result; // TODO: Error openong file
	}
	else
	{
		tmpfilename = tempnam(".", ".tmp_");
		file = fopen( tmpfilename, "w" );
		if ( file == NULL )
			return result; // TODO: Error openong file
	}

	fprintf(file, "<?xml version='1.0' encoding='%s' ?>\n", GOC_ENCODING);
	fprintf(file, "<FileObjectDoc>\n");
	// copy existing elements
	if ( tmpfilename )
	{
		GOC_RemStruct remSt;
		remSt.file = file;
		remSt.filter = filter;
		remSt.counter = 0;
		listObjectsInFile(filename, removeFun, &remSt);
		result = remSt.counter;
	}
	// close
	fprintf(file, "</FileObjectDoc>\n");
	fclose(file);
	if ( tmpfilename )
		rename(tmpfilename, filename);
	return result;
}

int deserializeObjectsInFile(
	const char *filename,
	int (*callFun)(const char *classname, void *node, void *arg), void *arg)
{
	xmlDoc *doc = NULL;
	xmlNode *root = NULL;
	xmlNode *child = NULL;

	if ( filename == NULL )
		return GOC_ERR_WRONGARGUMENT + 0;
	if ( callFun == NULL )
		return GOC_ERR_WRONGARGUMENT + 1;
	if ( !goc_isFileExists( filename ) )
		return GOC_ERR_FILEEXIST + 0;

	doc = xmlReadFile( filename, NULL, 0 );
	if ( doc == NULL )
		printf("Nieudane odczytywanie pliku\n");
	root = xmlDocGetRootElement( doc );
	if ( root == NULL )
		printf("Nie otrzymano galezi root\n");
	for ( child = root->children; child; child = child->next )
	{
		if ( child->type != XML_ELEMENT_NODE )
			continue;
		if ( callFun )
		{
			callFun( (const char *)child->name, child, arg );
		}
	}
	xmlFreeDoc( doc );
	return 0;
}

// jezeli callFun zwroci wartosc 0 to obiekt nie bedzie usuwany, jesli 1 to tak
int listObjectsInFile(
	const char *filename,
	int (*callFun)(GOC_OBJECTHANDLER object, void *arg), void *arg)
{
	xmlDoc *doc = NULL;
	xmlNode *root = NULL;
	xmlNode *child = NULL;

	if ( filename == NULL )
		return GOC_ERR_WRONGARGUMENT + 0;
	if ( callFun == NULL )
		return GOC_ERR_WRONGARGUMENT + 1;
	if ( !goc_isFileExists( filename ) )
		return GOC_ERR_FILEEXIST + 0;

	doc = xmlReadFile( filename, NULL, 0 );
	if ( doc == NULL )
		printf("Nieudane odczytywanie pliku\n");
	root = xmlDocGetRootElement( doc );
	if ( root == NULL )
		printf("Nie otrzymano galezi root\n");
	for ( child = root->children; child; child = child->next )
	{
		GOC_FUNCTIONRETURN res;
		if ( child->type != XML_ELEMENT_NODE )
			continue;
		res = callStatic((const char*)child->name, "deserialize", child, NULL );
		if ( res.code == GOC_ERR_OK )
		{
			if ( callFun )
			{
				callFun( (GOC_OBJECTHANDLER)res.result, arg );
			}
		}
		else
		{
			printf("Obiekt nie zostal wygenerowany %s [%s]\n", child->name, res.description);
		}
	}
	xmlFreeDoc( doc );
	return 0;
}
