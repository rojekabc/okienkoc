#define GOC_PRINTINFO
#define GOC_PRINTERROR

#include <okienkoc/log.h>
#include <okienkoc/plik.h>
#include <okienkoc/fileistream.h>
#include <okienkoc/mystr.h>
#include <okienkoc/properties.h>
#include <okienkoc/arguments.h>
#include <okienkoc/tablica.h>

#include <okienkoc/okienkoc.h>

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <dirent.h>
#include <string.h>

#define PROPERTY_PACKAGELISTFOLDER "package.folder"

char **listInstalledPkgs(char** pList, _GOC_TABEL_SIZETYPE_ *nList, char* folder)
{
	DIR* dir = NULL;
	struct dirent* ent;
	dir = opendir( folder );
	if ( dir == NULL )
	{
		GOC_BERROR("Cannot open folder [%s]", folder);
		return 0;
	}
	while ( (ent = readdir( dir )) != NULL )
	{
		char* name = NULL;
		if ( goc_stringOrEquals(ent->d_name, ".", "..") )
			continue;
		name = goc_stringCopy(name, ent->d_name);
		pList = goc_tableAdd(pList, nList, sizeof(char*));
		pList[(*nList)-1] = name;
	}
	return pList;
}

static char **freeStringTable(char** pList, _GOC_TABEL_SIZETYPE_ *nList)
{
	_GOC_TABEL_SIZETYPE_ i;
	if ( ! pList )
		return NULL;
	if ( ! nList )
		return NULL;
	for (i=0; i<*nList; i++)
		goc_stringFree(pList[i]);

	return goc_tableClear(pList, nList);
}

int argumentConsole(char** argv, int pos, int argc, void* param)
{
	GOC_MSG wiesc;
	GOC_HANDLER lista;
	GOC_HANDLER info;
	char* value;
	GOC_Properties* properties = *((GOC_Properties**)param);

	lista = goc_elementCreate(GOC_ELEMENT_LIST, 1, 1, 0, -1,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE | GOC_LISTFLAG_SORT,
		GOC_WHITE, GOC_HANDLER_SYSTEM);
	info = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 0, 0, 0,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
		GOC_WHITE, GOC_HANDLER_SYSTEM);

	goc_listAddColumn(lista, 35);
	goc_listAddColumn(lista, 5);
	goc_listAddColumn(lista, -1);

	goc_labelAddLine(info, "[I]nfo");

	// Listing installed packages
	value = goc_propertiesGetValue(properties, PROPERTY_PACKAGELISTFOLDER);
	if ( value == NULL )
	{
		GOC_BERROR("No %s configured", PROPERTY_PACKAGELISTFOLDER);
		return 0;
	}
	if ( !goc_isFolder(value) )
	{
		GOC_BERROR("No folder exists [%s]. Check the configuration property [%s]", value, PROPERTY_PACKAGELISTFOLDER);
		return 0;
	}

	{
		char** pPkgs = NULL; // TODO: Set to global and free after all
		_GOC_TABEL_SIZETYPE_ nPkgs = 0;
		_GOC_TABEL_SIZETYPE_ i;
		pPkgs = listInstalledPkgs(pPkgs, &nPkgs, value);
		for ( i=0; i<nPkgs; i++ )
		{
			char* version = NULL;
			char* name = NULL;
			name = goc_stringCopy(name, pPkgs[i]);
			version = name;
			while ( version != NULL )
			{
				version = strchr(version, '-');
				if ( !version )
					break;
				version++;
				if (( *version >= '0' ) && ( *version <= '9' ))
				{
					version--;
					*version = 0;
					version++;
					break;
				}
			}
			goc_listSetColumnText(lista, name, 0);
			goc_listSetColumnText(lista, "I", 1);
			if ( version )
				goc_listSetColumnText(lista, version, 2);
			else
				goc_listSetColumnText(lista, "", 2);
			name = goc_stringFree(name);
		}
	}
	
	while (goc_systemCheckMsg( &wiesc ));
	return 1;
}

int argumentList(char** argv, int pos, int argc, void* param)
{
	char* value;
	GOC_Properties* properties = *((GOC_Properties**)param);

	// Listing installed packages
	value = goc_propertiesGetValue(properties, PROPERTY_PACKAGELISTFOLDER);
	if ( value == NULL )
	{
		GOC_BERROR("No %s configured", PROPERTY_PACKAGELISTFOLDER);
		return 0;
	}
	if ( !goc_isFolder(value) )
	{
		GOC_BERROR("No folder exists [%s]. Check the configuration property [%s]", value, PROPERTY_PACKAGELISTFOLDER);
		return 0;
	}

	{
		char** pPkgs = NULL;
		_GOC_TABEL_SIZETYPE_ nPkgs = 0;
		_GOC_TABEL_SIZETYPE_ i;
		pPkgs = listInstalledPkgs(pPkgs, &nPkgs, value);
		for ( i=0; i<nPkgs; i++ )
			printf("%s\n", pPkgs[i]);
		pPkgs = freeStringTable(pPkgs, &nPkgs);
	}

	return 1;
}

int main(int argc, char **argv)
{
	struct passwd* pwd = NULL;
	char* confFilename = NULL;
	GOC_IStream* fileStream = NULL;
	GOC_Properties* properties = NULL;
	GOC_Arguments* arguments = NULL;

	// Load configuration file
	pwd = getpwuid( getuid() );
	if ( !pwd )
	{
		GOC_ERROR("Cannot call getpwuid");
		return -1;
	}
	confFilename = goc_stringCopy(confFilename, pwd->pw_dir);
	confFilename = goc_stringAdd(confFilename, "/.config/rslackget");
	if ( !goc_isFileExists(confFilename) )
	{
		// Create default configuration file if doesn't exist
		GOC_BINFO("Creating configuration file [%s]", confFilename);
		FILE* fileConf = fopen(confFilename, "wb");
		if ( !fileConf )
		{
			GOC_BERROR("Cannot create the file [%s]", confFilename);
			return -1;
		}
		fprintf(fileConf, "%s=%s", PROPERTY_PACKAGELISTFOLDER, "/var/log/packages");
		fclose(fileConf);
	}

	fileStream = goc_fileIStreamOpen(confFilename);
	if ( !fileStream )
	{
		GOC_BERROR("Cannot open the file [%s]", confFilename);
		return -1;
	}
	properties = goc_propertiesLoad(properties, fileStream);
	goc_isClose(fileStream);

	// Preparing program arguments
	arguments = goc_argumentsSetUnknownFunction(arguments, goc_argumentHelpFunction);
	arguments = goc_argumentsAdd(arguments, "help", "Print this help", goc_argumentHelpFunction, &arguments);
	arguments = goc_argumentsAdd(arguments, "--help", "Print this help", goc_argumentHelpFunction, &arguments);
	arguments = goc_argumentsAdd(arguments, "-h", "Print this help", goc_argumentHelpFunction, &arguments);
	arguments = goc_argumentsAdd(arguments, "-?", "Print this help", goc_argumentHelpFunction, &arguments);

	arguments = goc_argumentsAdd(arguments, "list", "List installed packages", argumentList, &properties);
	arguments = goc_argumentsAdd(arguments, "-l", "List installed packages", argumentList, &properties);
	arguments = goc_argumentsAdd(arguments, "-ls", "List installed packages", argumentList, &properties);
	arguments = goc_argumentsAdd(arguments, "--list", "List installed packages", argumentList, &properties);

	arguments = goc_argumentsAdd(arguments, "console", "Run console version", argumentConsole, &properties);
	arguments = goc_argumentsAdd(arguments, "-c", "Run console version", argumentConsole, &properties);

	goc_argumentsParse(arguments, argc, argv);

	return 0;
}
