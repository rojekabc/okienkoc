#include <string.h>
#include <stdio.h>
#include <dirent.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/types.h>
#include "plista.h"

#include "global-inc.h"
#include "screen.h"
#include "mystr.h"
#include "tablica.h"
#include "plik.h"
//#include "napis.h"
//#include "ramka.h"
//#include "hotkey.h"
//
// get_current_dir_name() is not under memwatch controll
#ifdef _DEBUG
#	define get_current_dir_name() mwMark(get_current_dir_name(), "get_current_dir_name", __FILE__, __LINE__)
#endif

const char* GOC_ELEMENT_FILELIST = "GOC_FileList";
const char* GOC_MSG_FILELISTISFOLDER = "GOC_MsgFileListIsFolder";
const char* GOC_MSG_FILELISTADDFILE = "GOC_MsgFileListAddFile";
const char* GOC_MSG_FILELISTLISTFOLDER = "GOC_MsgFileListListFolder";
const char* GOC_MSG_FILELISTSETFOLDER = "GOC_MsgFileListSetFolder";
const char* GOC_MSG_FILELISTISUPFOLDER = "GOC_MsgFileListIsUpFolder";
const char* GOC_MSG_FILELISTGETFOLDERNAME = "GOC_MsgFileListGetFolderName";

static int filelistDestroy(GOC_HANDLER uchwyt)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	plista->folder = goc_stringFree( plista->folder );
	return goc_listListener(uchwyt, GOC_MSG_DESTROY, NULL, 0);
}

static int filelistIsUpFolder(GOC_HANDLER uchwyt, const char *name)
{
	if ( goc_stringEquals(name, "../") )
		return GOC_ERR_OK;
	return GOC_ERR_FALSE;
}

static int filelistGetFolderName(GOC_HANDLER uchwyt, char **name)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	char *p;
	int len;
	if ( name == NULL )
		return GOC_ERR_FALSE;
	len = strlen(plista->folder);

	if (len > 1)
	{
		if ( plista->folder[len-1] == '/' )
			plista->folder[len-1] = 0;

		p = strrchr( plista->folder, '/');
		*name = goc_stringCopy( NULL, p + 1 );
		*name = goc_stringAdd(*name, "/");
		return GOC_ERR_OK;
	}

	return GOC_ERR_FALSE;
}

static int filelistHotKeyFolder(
	GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, unsigned int nBuf)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	char *name = goc_listGetUnderCursor(uchwyt);
	int wiersz = -1;
	char *prevname = NULL;

	// sprawdz, czy katalog
/*	if ( name == NULL )
		return GOC_ERR_OK;
	if ( name[strlen(name)-1] != '/' )
		return GOC_ERR_OK;*/
	if ( goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTISFOLDER,
		(unsigned char *)name, plista->kursor)
		!= GOC_ERR_OK )
		return GOC_ERR_FALSE;
	if ( goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTISUPFOLDER,
		(unsigned char *)name, plista->kursor)
		== GOC_ERR_OK )
	{
		if ( goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTGETFOLDERNAME,
			(unsigned char *)(&prevname), 0)
			!= GOC_ERR_OK )
			prevname = NULL;
	}
		
//	if ( name == NULL )
//		return GOC_ERR_OK;
//	if ( name[strlen(name)-1] != '/' )
//		return GOC_ERR_OK;
	// wyodrebnij poprzedni folder, jezeli nastapila zmiana do gory
//	if ( goc_systemSendMsg(uchwyt, WIESC_PLISTA_PRZEJDZDOKATALOGU,
//		(unsigned char *)name, 0)
//		!= GOC_ERR_OK )
//		return GOC_ERR_FALSE;
/*	if ( strcmp(name, "../") == 0 )
	{
		char *p;
		int len = strlen(plista->folder);

		if (len > 1)
		{
			if ( plista->folder[len-1] == '/' )
				plista->folder[len-1] = 0;

			p = strrchr( plista->folder, '/');
			prevname = goc_stringCopy( NULL, p + 1 );
			prevname = goc_stringAdd(prevname, "/");
		}
	}*/
	// przejdz
	goc_filelistSetFolder(uchwyt, name);
	if ( prevname != NULL )
	{
		wiersz = goc_listFindText(uchwyt, prevname);
		prevname = goc_stringFree(prevname);
	}
	if ( wiersz != -1 )
		goc_listSetCursor(uchwyt, wiersz);
	return goc_systemSendMsg(uchwyt, GOC_MSG_PAINT, 0, 0);
}

static int filelistInit(GOC_HANDLER uchwyt)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	goc_listListener(uchwyt, GOC_MSG_INIT, plista, 0);
	plista->folder = NULL;
	plista->flag |= GOC_LISTFLAG_SORT;
	goc_hkAdd( uchwyt, 0xD, GOC_EFLAGA_ENABLE | GOC_HKFLAG_SYSTEM,
			filelistHotKeyFolder );
	return GOC_ERR_OK;
}

// zmiana katalogu
// wylistowanie nowego katalogu
int goc_filelistSetFolder(GOC_HANDLER uchwyt, const char *dirname)
{
	int ret;
	char *name = goc_stringCopy(NULL, dirname);
	ret = goc_systemSendMsg(uchwyt, GOC_MSG_LISTCLEAR, NULL, 0);
	if ( ret != GOC_ERR_OK )
	{
		name = goc_stringFree(name);
		return ret;
	}
	ret = goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTSETFOLDER,
		(unsigned char *)name, 0);
	if ( ret != GOC_ERR_OK )
	{
		name = goc_stringFree(name);
		return ret;
	}
	name = goc_stringFree(name);
	ret = goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTLISTFOLDER,
		NULL, 0);
	if ( ret != GOC_ERR_OK )
		return ret;
//	ret = goc_listSetCursor(uchwyt, 0);
	return ret;
}

static int filelistListFolder(GOC_HANDLER uchwyt, const char *name)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	DIR *dir = NULL;
	struct dirent *ent;
	if ( plista == NULL )
		return GOC_ERR_WRONGARGUMENT;
	if ( plista->folder == NULL )
		plista->folder = get_current_dir_name();
	if ( name == NULL )
		name = plista->folder;
	if ( goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTISFOLDER,
		(unsigned char *)name, -1)
		!= GOC_ERR_OK )
		return GOC_ERR_FALSE;
	dir = opendir( name );
	if ( dir == NULL )
		return GOC_ERR_FALSE;
	while ( (ent = readdir( dir )) != NULL )
	{
		char *buf;
		unsigned int flag = 0;

		if (( strcmp(ent->d_name, "..") == 0 ) && ( strlen(name) == 1 ))
			continue;

		buf = goc_stringCopy( NULL, name );
		
		if ( name[strlen(name)-1] != '/' )
			buf = goc_stringAdd(buf, "/");
		buf = goc_stringAdd(buf, ent->d_name);
		if ( goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTISFOLDER, buf, -1)
			== GOC_ERR_OK )
		{
			flag = GOC_FILELISTFLAG_FOLDER;
			buf = goc_stringCopy( buf, ent->d_name );
			buf = goc_stringAdd( buf, "/" );
		}
		else
		{
			buf = goc_stringCopy( buf, ent->d_name );
		}
		if ( goc_systemSendMsg(uchwyt, GOC_MSG_FILELISTADDFILE, ent->d_name,
			flag) == GOC_ERR_OK )
		{
			goc_systemSendMsg(uchwyt, GOC_MSG_LISTADDTEXT, buf, 0);
		}
		buf = goc_stringFree(buf);
	}
	closedir( dir );
	return GOC_ERR_OK;

}

static int filelistIsFolder(
	GOC_HANDLER uchwyt, const char *name, unsigned int pos)
{
	if ( pos == -1 )
	{
		if ( goc_isFolder(name) )
			return GOC_ERR_OK;
		else
			return GOC_ERR_FALSE;
	}
	else
	{
		if (( name != NULL ) && ( name[strlen(name)-1] == '/' ))
			return GOC_ERR_OK;
		else
			return GOC_ERR_FALSE;
	}
}

static int filelistSetFolder(GOC_HANDLER uchwyt, const char *dirname)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	char *folder = NULL;
	if ( dirname == NULL )
		return GOC_ERR_WRONGARGUMENT;
	// przejscie od foldera root
	if ( *dirname == '/' )
	{
		folder = goc_stringCopy(folder, dirname);
	}
	// przejscie od katalogu listy
	else
	{
		folder = goc_stringCopy(folder, plista->folder);
		if (( folder ) && ( folder[strlen(folder)-1] != '/' ))
			folder = goc_stringAdd(folder, "/");
		folder = goc_stringAdd(folder, dirname);
	}

	// jesli katalog, wykonaj zmiane
	if ( goc_isFolder( folder ) )
	{
		// zachowaj biezacy
		plista->folder = goc_stringSet(plista->folder,
			get_current_dir_name() );
		if ( chdir( folder ) == -1 )
		{
			folder = goc_stringFree( folder );
			return GOC_ERR_REFUSE;
		}
		// pobierz docelowy
		folder = goc_stringSet( folder,
			get_current_dir_name() );
		// przywroc rzeczywisty
		chdir( plista->folder );
		plista->folder = goc_stringCopy( plista->folder, folder );
		folder = goc_stringFree( folder );
		return GOC_ERR_OK;
	}

	folder = goc_stringFree( folder );
	return GOC_ERR_REFUSE;
}


static int filelistAddFile(
	GOC_HANDLER uchwyt, const char *name, unsigned int flag)
{
	int len;
	if ( name == NULL )
		return GOC_ERR_REFUSE;
	len = strlen(name);
	if ( len == 0 )
		return GOC_ERR_REFUSE;
	// nie dodawaj folderu biezacego
	if (( len == 1 ) && ( *name == '.' ))
		return GOC_ERR_REFUSE;
	return GOC_ERR_OK;
}

const char *goc_filelistGetFolder(
	GOC_HANDLER uchwyt)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	return plista->folder;
}

static GOC_COLOR filelistGetColor(GOC_HANDLER uchwyt, unsigned int nBuf)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	if ( plista->kursor == nBuf )
		return GOC_WHITE | GOC_FBOLD;
	return GOC_WHITE;
}

int goc_filelistListener(GOC_HANDLER uchwyt, GOC_MSG wiesc, void* pBuf, unsigned int nBuf)
{
	if ( wiesc == GOC_MSG_CREATE )
	{
		GOC_StElement* elem = (GOC_StElement*)pBuf;
		GOC_HANDLER* retuchwyt = (GOC_HANDLER*)nBuf;
		GOC_StFileList* plista = (GOC_StFileList*)malloc(sizeof(GOC_StFileList));
		memcpy(plista, elem, sizeof(GOC_StElement));
		*retuchwyt = (GOC_HANDLER)plista;
		return goc_systemSendMsg(*retuchwyt, GOC_MSG_INIT, plista, 0);
	}
	else if ( wiesc == GOC_MSG_DESTROY )
	{
		return filelistDestroy(uchwyt);
	}
	else if ( wiesc == GOC_MSG_INIT )
	{
		return filelistInit(uchwyt);
	}
//	else if ( wiesc == GOC_MSG_CHAR )
//	{
//		return plistaUzyskajZnak( uchwyt, nBuf );
//	}
	else if ( wiesc == GOC_MSG_LISTSETCOLOR )
	{
		*((GOC_COLOR*)pBuf) = filelistGetColor(uchwyt, nBuf);
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_FILELISTISFOLDER )
	{
		return filelistIsFolder(uchwyt, pBuf, nBuf);
	}
	else if ( wiesc == GOC_MSG_FILELISTADDFILE )
	{
		return filelistAddFile(uchwyt, pBuf, nBuf);
	}
	else if ( wiesc == GOC_MSG_FILELISTSETFOLDER )
	{
		return filelistSetFolder(uchwyt, pBuf);
	}
	else if ( wiesc == GOC_MSG_FILELISTLISTFOLDER )
	{
		return filelistListFolder(uchwyt, pBuf);
	}
	else if ( wiesc == GOC_MSG_FILELISTISUPFOLDER )
	{
		return filelistIsUpFolder(uchwyt, pBuf);
	}
	else if ( wiesc == GOC_MSG_FILELISTGETFOLDERNAME )
	{
		return filelistGetFolderName(uchwyt, pBuf);
	}
	else
		return goc_listListener(uchwyt, wiesc, pBuf, nBuf);
	return GOC_ERR_UNKNOWNMSG;
}
