#include <stdio.h>
#include <string.h>
#define GOC_PRINTINFO
#define GOC_PRINTERROR
#include <okienkoc/log.h>
#include <okienkoc/mystr.h>
#include <okienkoc/fileistream.h>
#include <okienkoc/properties.h>

const char *wordChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890";

char *findNextWordBegin(char *string, char *pos)
{
	char *begin;
	if ( pos == NULL )
		begin = string;
	else
		begin = pos;
	while ( *begin )
	{
		if ( strchr(wordChars, *begin) )
		{
			return begin;
		}
		begin++;
	}
	return NULL;
}

char *getWord(const char *string)
{
	const char *end = string;
	while ( *end )
	{
		if ( strchr(wordChars, *end) == NULL )
			break;
		end++;
	}
	return strndup(string, end-string);
}

int main(int argc, char **argv)
{
	GOC_IStream *wordStream = NULL;
	GOC_IStream *fileStream = NULL;
	GOC_Properties *props = NULL;
	GOC_Iterator *propIt = NULL;
	char *line;
	char *pos = NULL;

	if ( argc < 3 )
	{
		GOC_BINFO("Usage: %s wordconvert.properties filetochange", argv[0]);
		GOC_INFO("\tApplication changes word in file to another");
		return 0;
	}
	wordStream = goc_fileIStreamOpen(argv[1]);
	if ( wordStream == NULL )
	{
		GOC_BERROR("Cannot open file %s", argv[1]);
		return -1;
	}
	fileStream = goc_fileIStreamOpen(argv[2]);
	if ( fileStream == NULL )
	{
		GOC_BERROR("Cannot open file %s", argv[2]);
		return -1;
	}
	props = goc_propertiesAlloc();
	props = goc_propertiesLoad(props, wordStream);
	while ( line = goc_isReadLine(fileStream) )
	{
		char *bline = NULL;
		char *last = line;
		pos = NULL;
		while ( pos = findNextWordBegin(line, pos) )
		{
			char *word = getWord( pos );
			char *v = goc_propertiesGetValue( props, word );

			*pos = 0;
			bline = goc_stringAdd(bline, last);

			if ( v != NULL )
			{
				bline = goc_stringAdd(bline, v);
			}
			else
			{
				bline = goc_stringAdd(bline, word);
			}
			pos += strlen(word);
			word = goc_stringFree(word);
			last = pos;
		}
		bline = goc_stringAdd(bline, last);
		puts(bline);
		line = goc_stringFree(line);
		bline = goc_stringFree(bline);
	}
	free(propIt);
	goc_isClose(wordStream);
	goc_isClose(fileStream);

	return 0;
}
