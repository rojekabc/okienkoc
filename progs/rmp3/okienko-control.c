/*
 * Control interface to generic front ends.
 * written/copyrights 1997/99 by Andreas Neuhaus (and Michael Hipp)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#define __USE_GNU
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>

#include <okienkoc/okienkoc.h>
#include <tools/mystr.h>
#define GOC_PRINTERROR
#include <tools/log.h>
#include <tools/istream.h>
#include <tools/screen.h>
#include <tools/term.h>
#include <tools/plik.h>
#include <tools/fileistream.h>

#include <sys/socket.h>
#include "mod_fun.h"

#include "finfo.h"

#define MODE_STOPPED 0
#define MODE_PLAYING 1
#define MODE_PAUSED 2

GOC_HANDLER nTytul;
GOC_HANDLER nMixer;
GOC_HANDLER pMixer;
GOC_HANDLER nPlik, nTitle, nArtist, nAlbum, nYear, nComment, nStatus;
GOC_HANDLER lLista;
GOC_HANDLER pLista;
GOC_HANDLER nLogo;

void lista_polecen()
{
	goc_gotoxy(1,9);
	printf("'q' - Quit program    'h' - Show this help\n");
	printf("'i' - Info about song 's' - Stop playing\n");
	printf("'p' - (Un)Pause song  'n' - Next song\n");
	printf("'b' - Previous song   '*' - Next aumix\n");
	printf("'+' - Increas aumix   '-' - Decreas aumix\n");
	printf("'Home' - Jum to begin '->' - Jump forward\n");
	printf("'r' - Enable shuffle  'R' - disable shuffle\n");
	printf("'l' - List songs      'a' - add folder\n");
	printf("'x' - Queue/Unqueue\n");
	fflush(stdout);
	//	printf("[l]oad <file> - Load a file to play\n");
	//	printf("[j]ump <+|->int - Jump inc/dec, or to position\n");
	//	printf("[j]ump <m>char  - Jump to mark position\n");
	//	printf("[m]ark char  - mark position\n");
}

void printInfo()
{
}

/*
struct mark_type *clear_mark(struct mark_type *mark) // ROJEK
{
	if (!mark)
		return mark;
	free(mark->znak);
	free(mark->framecnt);
	free(mark);
	mark = NULL;
	return mark;
}

struct mark_type *add_mark(struct mark_type *mark, char znak, int frmcnt) // ROJEK
{
	int i;
	// za³ó¿ je¶li nie ma struktury
	if (!mark) {
		mark = (mark_type *)malloc(sizeof(mark_type));
		memset(mark, 0, sizeof(mark_type));
		mark->cnt = 1;
		mark->znak = (char *)malloc(sizeof(char)*mark->cnt);
		mark->framecnt = (int *)malloc(sizeof(int)*mark->cnt);
		mark->znak[0] = znak;
		mark->framecnt[0] = frmcnt;
		printf("Add mark %c at %d\n", znak, frmcnt);
		return mark;
	}
	// szukaj, czy nie ma ju¿ zamarkowania
	for (i = 0; i < mark->cnt; i++)
		if (mark->znak[i] == znak) {
			mark->framecnt[i] = frmcnt;
			printf("Chang mark %c to %d\n", znak, frmcnt);
			return mark;
		}
	// dodaj nowe zamarkowanie
	(mark->cnt)++;
	mark->znak = (char *)realloc(mark->znak, sizeof(char)*mark->cnt);
	mark->framecnt = (int *)realloc(mark->framecnt, sizeof(int)*mark->cnt);
	mark->znak[i] = znak;
	mark->framecnt[i] = frmcnt;
	printf("Add mark %c at %d\n", znak, frmcnt);
	return mark;
}

int get_mark_pos(struct mark_type *mark, char znak) // ROJEK
{
	int i;
	if (!mark)
		return -1;
	for (i = 0; i < mark->cnt; i++)
		if (mark->znak[i] == znak) {
			printf("Go to mark %c\n", znak);
			return mark->framecnt[i];
		}
	return -1;
}

void list_mark(struct mark_type *mark) // ROJEK
{
	int i;
	if (!mark)
		return;
	for (i = 0; i < mark->cnt; i++)
		printf("Mark %c at %6d    ", mark->znak[i], mark->framecnt[i]);
	printf("\n");
	return;
}

*/
int (*doActionCall)(unsigned int, void*) = NULL;

void printMixer()
{
	char* deviceName = NULL;
	int volumeLevel;
	doActionCall(ACTION_AUMIX_CURRNAME, &deviceName);
	goc_labelSetText(nMixer, deviceName, GOC_FALSE);
	doActionCall(ACTION_AUMIX_CURRDEV, &volumeLevel);
	volumeLevel &= 0x7F;
	goc_precentSetPosition(pMixer, volumeLevel);
	GOC_MSG_PAINT( msgPaint );
	goc_systemSendMsg(nMixer, msgPaint);
	goc_systemSendMsg(pMixer, msgPaint);
	fflush(stdout);
}

static int nasluch(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( uchwyt == GOC_HANDLER_SYSTEM )
	{
		if ( msg->id == GOC_MSG_CHAR_ID )
		{
			GOC_StMsgChar* msgChar = (GOC_StMsgChar*)msg;
			if ( msgChar->charcode == 27 ) {
				return GOC_ERR_OK;
			}
		}
	}
	else if ( uchwyt == lLista )
	{
		if ( msg->id == GOC_MSG_CHAR_ID )
		{
			GOC_StMsgChar* msgChar = (GOC_StMsgChar*)msg;
			if ( msgChar->charcode == 0x0D )
			{
				char *selFile = goc_listGetUnderCursor( lLista );
				if ( selFile ) {
					doActionCall(ACTION_PLAY, selFile);
				}
				return GOC_ERR_OK;
			}
			else if ( msgChar->charcode == 0x116 )
			{
				int pos = goc_listGetCursor(lLista);
				if ( pos > 0 )
				{
					int size = 0;
					const char **ptr;
					doActionCall(ACTION_PLAYLIST_REMOVEFILE, &pos);
					doActionCall(ACTION_PLAYLIST_GETSIZE, &size);
					doActionCall(ACTION_PLAYLIST_GETTABLE, &ptr);
					goc_listSetExtTable(lLista, ptr, size);
					doActionCall(ACTION_PLAYLIST_GETACTUAL, &size);
					goc_sellistSelect(lLista, size);
					goc_listSetCursor(lLista, pos);
//		goc_elementOrFlag(lLista, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE);
//		goc_systemFocusOn(lLista);
					GOC_MSG_PAINT(msgPaint);
					goc_systemSendMsg(lLista, msgPaint);
				}
				return GOC_ERR_OK;
			}
		}
		else if ( msg->id == GOC_MSG_LISTCHANGE_ID )
		{
			// TODO: Opis zaznaczonej pozycji
			return GOC_ERR_OK;
		}
	}
/*	else if ( uchwyt == pLista )
	{
		// selekcjonuj tylko katalogi do dodawania
		if ( wiesc == GOC_MSG_FILELISTADDFILE )
		{
			if ( nBuf != GOC_FILELISTFLAG_FOLDER )
				return GOC_ERR_REFUSE;
		}
	}*/
	return goc_systemDefaultAction(uchwyt, msg);
}

static int hotKeyNext(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_NEXT, NULL);
	return GOC_ERR_OK;
}
static int hotKeyPrev(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_PREV, NULL);
	return GOC_ERR_OK;
}
static int hotKeyInfo(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	printInfo();
	return GOC_ERR_OK;
}
static int hotKeyPause(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_PAUSE, NULL);
	return GOC_ERR_OK;
}
static int hotKeyHelp(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	lista_polecen();
	return GOC_ERR_OK;
}
static int hotKeyQuit(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_QUIT, NULL);
	GOC_MSG_FINISH( msgFinish, 0 );
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, msgFinish);
	return GOC_ERR_OK;
}
static int hotKeyStop(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_STOP, NULL);
	return GOC_ERR_OK;
}
static int hotKeyHome(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_PLAY, NULL);
	return GOC_ERR_OK;
}
static int hotKeyAumixPlus(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_AUMIX_PLUS, NULL);
	printMixer();
	return GOC_ERR_OK;
}
static int hotKeyAumixMinus(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_AUMIX_MINUS, NULL);
	printMixer();
	return GOC_ERR_OK;
}
static int hotKeyAumixNext(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	doActionCall(ACTION_AUMIX_NEXTDEV, NULL);
	printMixer();
	return GOC_ERR_OK;
}
static int hotKeyShuffleOn(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	int arg=SHUFFLE_YES;
	doActionCall(ACTION_SHUFFLE, &arg);
	goc_gotoxy(1,9);
	goc_clearline();
	printf("W³±czono tryb mieszania.\n");
	return GOC_ERR_OK;
}
static int hotKeyShuffleOff(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	int arg=SHUFFLE_NO;
	doActionCall(ACTION_SHUFFLE, &arg);
	goc_gotoxy(1,9);
	goc_clearline();
	printf("Wy³±czono tryb mieszania.\n");
	return GOC_ERR_OK;
}

static int hotKeyShowList(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_FLAGS f = goc_elementGetFlag(lLista);
	if ( f & GOC_EFLAGA_PAINTED )
	{
		goc_listClear(lLista);
		goc_elementIsFlag(lLista, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE);
		//goc_elementSetFlag(lLista, f);
		goc_systemRepaintUnder(lLista);
		fflush(stdout);
	}
	else
	{
		int size = 0;
		const char **ptr;
		doActionCall(ACTION_PLAYLIST_GETSIZE, &size);
		doActionCall(ACTION_PLAYLIST_GETTABLE, &ptr);
		goc_listSetExtTable(lLista, ptr, size);
		doActionCall(ACTION_PLAYLIST_GETACTUAL, &size);
		goc_sellistSelect(lLista, size);
		goc_listSetCursor(lLista, size);
		goc_elementOrFlag(lLista, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE);
		goc_systemFocusOn(lLista);
	}
	GOC_MSG_PAINT(msgPaint);
	goc_systemSendMsg(lLista, msgPaint);
	return GOC_ERR_OK;
}

// TODO: Dodanie odswiezenia listy, jesli jest widoczna po dodaniu pliku lub
// katalogu z zachowaniem pozycji kursora
static int hotKeyAddFolder(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	GOC_FLAGS f = goc_elementGetFlag(pLista);
	if ( f & GOC_EFLAGA_PAINTED )
	{
		goc_labelRemLines(nStatus);
		goc_elementIsFlag(pLista, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE);
		goc_systemRepaintUnder(pLista);
		goc_systemRepaintUnder(nStatus);
		fflush(stdout);
	}
	else
	{
		goc_elementOrFlag(pLista, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE);
		goc_systemFocusOn(pLista);
		GOC_MSG_PAINT(msgPaint);
		goc_systemSendMsg(pLista, msgPaint);
	}
	return GOC_ERR_OK;
}

// find audio stream in the file
static void checkAddFile(const char *fullname)
{
	struct FileInfo fileInfo;
	memset( &fileInfo, 0, sizeof(struct FileInfo) );
	if ( finfoInfo(fullname, &fileInfo) == FINFO_CODE_OK ) {
		doActionCall(ACTION_PLAYLIST_ADDFILE, (void*)fullname);
		goc_stringFree(fileInfo.filename);
		goc_stringFree(fileInfo.title);
		goc_stringFree(fileInfo.artist);
		goc_stringFree(fileInfo.album);
		goc_stringFree(fileInfo.year);
		goc_stringFree(fileInfo.comment);
		goc_stringFree(fileInfo.genre);
	}
	return;
}

// listuj katalog i dodawaj pliki w nim zawarte, ktróre s± mp3
static void listAddFolder(const char *fullname)
{
	DIR *dir = opendir( fullname );
	struct dirent *ent;
	if ( dir == NULL )
		return;
	while ( (ent = readdir( dir )) != NULL )
	{
		char *buf = NULL;
		if ( goc_stringEquals( ent->d_name, ".." ) )
			continue;
		if ( goc_stringEquals( ent->d_name, "." ) )
			continue;
		buf = goc_stringCopy( buf, fullname );
		if ( fullname[strlen(fullname)-1] != '/' )
			buf = goc_stringAdd( buf, "/" );
		buf = goc_stringAdd( buf, ent->d_name );
		if ( goc_isFolder( buf ) )
			listAddFolder( buf );
		else
			checkAddFile( buf );
		buf = goc_stringFree( buf );
	}
	closedir( dir );
}

static int hotKeySelectFolder(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	char *element = goc_stringCopy(NULL, goc_filelistGetFolder(uchwyt));
	element = goc_stringAdd(element, "/");
	element = goc_stringAdd(element, goc_listGetUnderCursor(uchwyt));
	if ( goc_isFileExists( element ) )
	{
		if ( goc_isFolder( element ) )
		{
			listAddFolder( element );
		}
		else
		{ // a wiec to plik
			checkAddFile( element );
		}
		char *tmp = goc_stringAdd(goc_stringCopy(NULL, "Dodano: "), element);
		goc_labelSetText(nStatus, tmp, 0);
		tmp = goc_stringFree(tmp);
		GOC_MSG_PAINT(msgPaint);
		goc_systemSendMsg(nStatus, msgPaint);
	}
	goc_stringFree( element );
	return GOC_ERR_OK;
}

static int hotKeyQueue(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	int arg;
	GOC_FLAGS f = goc_elementGetFlag(lLista);
	if ( f & GOC_EFLAGA_PAINTED )
	{
		arg = goc_listGetCursor(lLista);
		doActionCall(ACTION_PLAYLIST_ISQUEUE, &arg);
		if ( arg )
		{
			arg = goc_listGetCursor(lLista);
			doActionCall(ACTION_PLAYLIST_REMQUEUE, &arg);
		}
		else
		{
			arg = goc_listGetCursor(lLista);
			doActionCall(ACTION_PLAYLIST_ADDQUEUE, &arg);
		}
	}
	return GOC_ERR_OK;
}

#define LOGO_FILE "/usr/local/share/rmp3/rmp3.logo"

int controlInit(int (*fun)(unsigned int, void*))
{
	finfoInitialize();
	if ( fun == NULL )
	{
		fprintf(stderr, "Nie podano funkcji kontrolnej.\n%s:%d\n",
				__FILE__, __LINE__);
		exit(1);
	}
	doActionCall = fun;
	// zainicjowanie systemu okienkoc
	goc_systemSetListenerFunc( &nasluch );
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'n', GOC_EFLAGA_ENABLE, hotKeyNext);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'N', GOC_EFLAGA_ENABLE, hotKeyNext);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'b', GOC_EFLAGA_ENABLE, hotKeyPrev);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'B', GOC_EFLAGA_ENABLE, hotKeyPrev);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'i', GOC_EFLAGA_ENABLE, hotKeyInfo);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'I', GOC_EFLAGA_ENABLE, hotKeyInfo);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'p', GOC_EFLAGA_ENABLE, hotKeyPause);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'P', GOC_EFLAGA_ENABLE, hotKeyPause);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'h', GOC_EFLAGA_ENABLE, hotKeyHelp);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'H', GOC_EFLAGA_ENABLE, hotKeyHelp);
	goc_hkAdd(GOC_HANDLER_SYSTEM, '?', GOC_EFLAGA_ENABLE, hotKeyHelp);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'q', GOC_EFLAGA_ENABLE, hotKeyQuit);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'Q', GOC_EFLAGA_ENABLE, hotKeyQuit);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 's', GOC_EFLAGA_ENABLE, hotKeyStop);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'S', GOC_EFLAGA_ENABLE, hotKeyStop);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 0x114, GOC_EFLAGA_ENABLE, hotKeyHome);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 0x120, GOC_EFLAGA_ENABLE, hotKeyAumixPlus);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 0x11f, GOC_EFLAGA_ENABLE, hotKeyAumixMinus);
	goc_hkAdd(GOC_HANDLER_SYSTEM, '+', GOC_EFLAGA_ENABLE, hotKeyAumixPlus);
	goc_hkAdd(GOC_HANDLER_SYSTEM, '-', GOC_EFLAGA_ENABLE, hotKeyAumixMinus);
	goc_hkAdd(GOC_HANDLER_SYSTEM, '*', GOC_EFLAGA_ENABLE, hotKeyAumixNext);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'r', GOC_EFLAGA_ENABLE, hotKeyShuffleOn);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'R', GOC_EFLAGA_ENABLE, hotKeyShuffleOff);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'l', GOC_EFLAGA_ENABLE, hotKeyShowList);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'L', GOC_EFLAGA_ENABLE, hotKeyShowList);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'a', GOC_EFLAGA_ENABLE, hotKeyAddFolder);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'A', GOC_EFLAGA_ENABLE, hotKeyAddFolder);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'x', GOC_EFLAGA_ENABLE, hotKeyQueue);
	goc_hkAdd(GOC_HANDLER_SYSTEM, 'X', GOC_EFLAGA_ENABLE, hotKeyQueue);
	
	nTytul = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 1, 0, 1,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_WHITE, GOC_HANDLER_SYSTEM);
	goc_labelSetText(nTytul, "Rojkowy kontroler muzyki. Wersja 0.3a :+)",GOC_FALSE);
	nMixer = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 8, 8, 1,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_LEFT, GOC_WHITE, GOC_HANDLER_SYSTEM);
	pMixer = goc_elementCreate(GOC_ELEMENT_PRECENT, 9, 8, 0, 1,
		GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM);
	goc_precentSetMin(pMixer, 0);
	goc_precentSetMax(pMixer, 100);
	nPlik = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 3, 0, 2,
		GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM);
	nTitle = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 5, 0, 1,
		GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM);
	nArtist = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 6, goc_screenGetWidth()>>1, 1,
		GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM);
	nAlbum = goc_elementCreate(GOC_ELEMENT_LABEL, (goc_screenGetWidth()>>1) + 1, 6, 0, 1,
		GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM);
	nComment = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 7, goc_screenGetWidth()>>1, 1,
		GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM);
	nStatus = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 0, 0, 1,
		GOC_EFLAGA_PAINTED, GOC_GREEN, GOC_HANDLER_SYSTEM);
	nYear = goc_elementCreate(GOC_ELEMENT_LABEL, (goc_screenGetWidth()>>1) + 1, 7, 20, 1,
		GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM);
	lLista = goc_elementCreate(GOC_ELEMENT_SELLIST, 1, 10, 0, -1,
		0, GOC_WHITE, GOC_HANDLER_SYSTEM);
	pLista = goc_elementCreate(GOC_ELEMENT_FILELIST, 1, 10, 0, -1,
		0, GOC_WHITE, GOC_HANDLER_SYSTEM);
	if ( goc_isFileExists(LOGO_FILE) )
	{
		char *line = NULL;
		nLogo = goc_elementCreate(GOC_ELEMENT_LABEL, -20, -10, 20, 8,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_CYAN, GOC_HANDLER_SYSTEM);
		GOC_IStream* streamLogo = goc_fileIStreamOpen(LOGO_FILE);
		while ( (line = goc_isReadLine( streamLogo )) )
		{
			goc_labelAddLine(nLogo, line);
			line = goc_stringFree( line );
		}
		goc_isClose( streamLogo );
	}
	goc_filelistSetFolder(pLista, ".");
	goc_hkAdd(pLista, ' ', GOC_EFLAGA_ENABLE, hotKeySelectFolder);
	goc_elementSetFunc(lLista, GOC_FUNID_LISTENER, nasluch);

	printMixer();
	return 0;
}

void controlCleanup()
{
//	myterm_restore();
}

// -- do ustalenia, co bedzie decydowalo co zrobic po zakonczeniu piosenki
// -- czy grac dalej, czy przerwac
int controlStart()
{
	GOC_StMessage msg;
	doActionCall(ACTION_START, NULL);
	GOC_MSG_PAINT(msgPaint);
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, msgPaint);
	while ((goc_systemCheckMsg( &msg )));
	return 0;
}

int controlEvent(unsigned int event)
{
	if ( event == EVENT_CHANGE_SONG ) 
	{
		GOC_MSG_PAINT(msgPaint);

		FileInfo* fileInfo = allocFileInfo();
		doActionCall(ACTION_INFO, fileInfo);

		goc_labelSetText(nPlik, fileInfo->filename, 0);
		goc_systemSendMsg(nPlik, msgPaint);

		goc_labelSetText(nTitle, fileInfo->title, 0);
		goc_systemSendMsg(nTitle, msgPaint);

		goc_labelSetText(nArtist, fileInfo->artist, 0);
		goc_systemSendMsg(nArtist, msgPaint);
		
		goc_labelSetText(nAlbum, fileInfo->album, 0);
		goc_systemSendMsg(nAlbum, msgPaint);
		
		goc_labelSetText(nYear, fileInfo->year, 0);
		goc_systemSendMsg(nYear, msgPaint);
		
		goc_labelSetText(nComment, fileInfo->comment, 0);
		goc_systemSendMsg(nComment, msgPaint);

		freeFileInfo( fileInfo );

		if ( goc_elementGetFlag(lLista) & GOC_EFLAGA_PAINTED )
		{
			int pos = 0;
			pos = goc_sellistGetSelectPos(lLista, 0);
			goc_sellistUnselect(lLista, pos);
			pos = 0;
			doActionCall(ACTION_PLAYLIST_GETACTUAL, &pos);
			goc_sellistSelect(lLista, pos);
			goc_systemSendMsg(lLista, msgPaint);
		}
	}
	return 0;
}

/* EOF */
