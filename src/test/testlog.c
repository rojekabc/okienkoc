/*
 * Testowanie operacji logowania przy pomocy oferowanego systemu.
 */
#include <stdio.h>

#define GOC_DEBUGSTREAM debugFile
#define GOC_INFOSTREAM stdout
#define GOC_PRINTDEBUG
#define GOC_PRINTINFO
#define GOC_PRINTERROR
#include <okienkoc/log.h>

#include <okienkoc/mystr.h>

FILE *debugFile = NULL;

int main()
{
	debugFile = fopen("debug.log", "w");

	GOC_INFO("start");
	GOC_DEBUG("start");
	GOC_ERROR("start");
	GOC_PRINT(stderr, "INNY", "start");
	
	GOC_FINFO(goc_stringCopy(NULL, "copystart"));
	GOC_FDEBUG(goc_stringCopy(NULL, "copystart"));
	GOC_FERROR(goc_stringCopy(NULL, "copystart"));
	GOC_FPRINT(stderr, "INNY",goc_stringCopy(NULL, "copystart"));

	fclose(debugFile);
	return 0;
}
