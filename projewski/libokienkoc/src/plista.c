#include <string.h>
#include <stdio.h>
#include <dirent.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/types.h>
#include "plista.h"

#include "global-inc.h"
#include <tools/screen.h>
#include <tools/mystr.h>
#include <tools/tablica.h>
#include <tools/plik.h>
#include "system.h"
//#include "napis.h"
//#include "ramka.h"
//#include "hotkey.h"
//
// get_current_dir_name() is not under memwatch controll
#ifdef _DEBUG
#	define get_current_dir_name() mwMark(get_current_dir_name(), "get_current_dir_name", __FILE__, __LINE__)
#endif

const char* GOC_ELEMENT_FILELIST = "GOC_FileList";

const char* GOC_MSG_FILELISTISFOLDER_ID = "GOC_MsgFileListIsFolder";
const char* GOC_MSG_FILELISTADDFILE_ID = "GOC_MsgFileListAddFile";
const char* GOC_MSG_FILELISTLISTFOLDER_ID = "GOC_MsgFileListListFolder";
const char* GOC_MSG_FILELISTSETFOLDER_ID = "GOC_MsgFileListSetFolder";
const char* GOC_MSG_FILELISTISUPFOLDER_ID = "GOC_MsgFileListIsUpFolder";
const char* GOC_MSG_FILELISTGETFOLDERNAME_ID = "GOC_MsgFileListGetFolderName";

static int filelistDestroy(GOC_HANDLER uchwyt)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	plista->folder = goc_stringFree( plista->folder );
	GOC_MSG_DESTROY( msg );
	return goc_listListener(uchwyt, msg);
}

static int filelistIsUpFolder(GOC_HANDLER uchwyt, const char *name)
{
	if ( goc_stringEquals(name, "../") )
		return GOC_ERR_OK;
	return GOC_ERR_FALSE;
}

static int filelistGetFolderName(GOC_HANDLER uchwyt, GOC_StMsgFileListFilename* msg)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	char *p;
	int len;
	len = strlen(plista->folder);

	if (len > 1)
	{
		if ( plista->folder[len-1] == '/' )
			plista->folder[len-1] = 0;

		p = strrchr( plista->folder, '/');
		msg->pFilename = goc_stringCopy( NULL, p + 1 );
		msg->pFilename = goc_stringAdd(msg->pFilename, "/");
		return GOC_ERR_OK;
	}

	return GOC_ERR_FALSE;
}

static int filelistHotKeyFolder(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
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
	GOC_MSG_FILELISTISFOLDER( msgisfolder, name, plista->kursor );
	if ( goc_systemSendMsg(uchwyt, msgisfolder) != GOC_ERR_OK ) {
		return GOC_ERR_FALSE;
	}
	GOC_MSG_FILELISTISUPFOLDER( msgisup, name, plista->kursor );
	if ( goc_systemSendMsg(uchwyt, msgisup ) == GOC_ERR_OK )
	{
		GOC_MSG_FILELISTGETFOLDERNAME( msggetfoldername );
		if ( goc_systemSendMsg(uchwyt, msggetfoldername ) == GOC_ERR_OK ) {
			prevname = msggetfoldernameFull.pFilename;
		} else {
			prevname = NULL;
		}
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
	GOC_MSG_PAINT( msgpaint );
	return goc_systemSendMsg(uchwyt, msgpaint);
}

static int filelistInit(GOC_HANDLER uchwyt)
{
	GOC_StFileList *plista = (GOC_StFileList*)uchwyt;
	GOC_MSG_INIT( msginit );
	goc_listListener(uchwyt, msginit);
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
	GOC_MSG_LISTCLEAR( msgclear );
	ret = goc_systemSendMsg(uchwyt, msgclear);
	if ( ret != GOC_ERR_OK )
	{
		name = goc_stringFree(name);
		return ret;
	}
	GOC_MSG_FILELISTSETFOLDER( msgsetfolder, name );
	ret = goc_systemSendMsg(uchwyt, msgsetfolder);
	if ( ret != GOC_ERR_OK )
	{
		name = goc_stringFree(name);
		return ret;
	}
	name = goc_stringFree(name);
	GOC_MSG_FILELISTLISTFOLDER( msglistfolder, NULL );
	ret = goc_systemSendMsg(uchwyt, msglistfolder );
	if ( ret != GOC_ERR_OK )
		return ret;
//	ret = goc_listSetCursor(uchwyt, 0);
	return ret;
}

static int filelistListFolder(GOC_HANDLER uchwyt, char *name)
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
	GOC_MSG_FILELISTISFOLDER( msgisfolder, name, -1 );
	if ( goc_systemSendMsg(uchwyt, msgisfolder ) != GOC_ERR_OK ) {
		return GOC_ERR_FALSE;
	}
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
		GOC_MSG_FILELISTISFOLDER( msgsubisfolder, buf, -1 );
		if ( goc_systemSendMsg(uchwyt, msgsubisfolder) == GOC_ERR_OK ) {
			flag = GOC_FILELISTFLAG_FOLDER;
			buf = goc_stringCopy( buf, ent->d_name );
			buf = goc_stringAdd( buf, "/" );
		} else {
			buf = goc_stringCopy( buf, ent->d_name );
		}
		GOC_MSG_FILELISTADDFILE( msgaddfile, ent->d_name, flag );
		if ( goc_systemSendMsg(uchwyt, msgaddfile ) == GOC_ERR_OK ) {
			GOC_MSG_LISTADDTEXT( msgaddtext, buf );
			goc_systemSendMsg(uchwyt, msgaddtext);
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

int goc_filelistListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT( GOC_StFileList, plista, msg);
		GOC_MSG_INIT( msginit );
		return goc_systemSendMsg((GOC_HANDLER)plista, msginit);
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		return filelistDestroy(uchwyt);
	}
	else if ( msg->id == GOC_MSG_INIT_ID )
	{
		return filelistInit(uchwyt);
	}
//	else if ( wiesc == GOC_MSG_CHAR )
//	{
//		return plistaUzyskajZnak( uchwyt, nBuf );
//	}
	else if ( msg->id == GOC_MSG_LISTSETCOLOR_ID )
	{
		GOC_StMsgListSetColor* msgcolor = (GOC_StMsgListSetColor*)msg;
		msgcolor->color = filelistGetColor(uchwyt, msgcolor->position);
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_FILELISTISFOLDER_ID )
	{
		GOC_StMsgFileListPositionFilename* msgfilename = (GOC_StMsgFileListPositionFilename*)msg;
		return filelistIsFolder(uchwyt, msgfilename->pFilename, msgfilename->position);
	}
	else if ( msg->id == GOC_MSG_FILELISTADDFILE_ID )
	{
		GOC_StMsgFileListAddFile* msgaddfile = (GOC_StMsgFileListAddFile*)msg;
		return filelistAddFile(uchwyt, msgaddfile->pFilename, msgaddfile->flag);
	}
	else if ( msg->id == GOC_MSG_FILELISTSETFOLDER_ID )
	{
		GOC_StMsgFileListFilename* msgfilename = (GOC_StMsgFileListFilename*)msg;
		return filelistSetFolder(uchwyt, msgfilename->pFilename);
	}
	else if ( msg->id == GOC_MSG_FILELISTLISTFOLDER_ID )
	{
		GOC_StMsgFileListFilename* msgfilename = (GOC_StMsgFileListFilename*)msg;
		return filelistListFolder(uchwyt, msgfilename->pFilename);
	}
	else if ( msg->id == GOC_MSG_FILELISTISUPFOLDER_ID )
	{
		GOC_StMsgFileListFilename* msgfilename = (GOC_StMsgFileListFilename*)msg;
		return filelistIsUpFolder(uchwyt, msgfilename->pFilename);
	}
	else if ( msg->id == GOC_MSG_FILELISTGETFOLDERNAME_ID )
	{
		return filelistGetFolderName(uchwyt, (GOC_StMsgFileListFilename*)msg);
	}
	else {
		return goc_listListener(uchwyt, msg);
	}
	return GOC_ERR_UNKNOWNMSG;
}
