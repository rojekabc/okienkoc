/* 
 * moved all playlist related functions to this new file
 * The "Shuffle" part is now also used for simple non random
 * playlists. 
 *
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <tools/tablica.h>
#include <tools/mystr.h>
#include <tools/random.h>
#ifdef _DEBUG_
#	define GOC_PRINTDEBUG
#endif
#define GOC_PRINTERROR
#include <tools/log.h>
#include <tools/array.h>
#include <tools/malloc.h>

#include "playlist.h"

/* 
 * the playlist struct 
 */

typedef struct {
	GOC_Array* files;
	int *pShuffle;
	_GOC_TABEL_SIZETYPE_ nShuffle;
	int *pQueue;
	_GOC_TABEL_SIZETYPE_ nQueue;
	unsigned int currFile;
	unsigned int currShuffle;
	unsigned char shufflemode;
	int userSelect;
} stPlaylist;

#define RANDPULLSIZE 64
stPlaylist *playlist = NULL;

int playlistIsQueue( int inx )
{
	int pos;
	if ( playlist->pQueue == NULL )
		return 0;
	for ( pos = 0; pos < playlist->nQueue; pos++ )
	{
		if ( playlist->pQueue[pos] == inx )
			return 1;
	}
	return 0;
}

void playlistQueue( int inx )
{
	if ( inx > goc_arraySize(playlist->files) )
		return;
	if ( playlistIsQueue( inx ) )
		return;
	playlist->pQueue = goc_tableAdd(playlist->pQueue, &playlist->nQueue,
		sizeof(int));
	playlist->pQueue[playlist->nQueue - 1] = inx;
}

void playlistRemQueue( int inx )
{
	int pos;
	if ( playlist->pQueue == NULL )
		return;
	for ( pos = 0; pos < playlist->nQueue; pos++ )
	{
		if ( playlist->pQueue[pos] == inx )
		{
			playlist->pQueue = goc_tableRemove(
				playlist->pQueue, &playlist->nQueue,
				sizeof(int), pos );
			break;
		}
	}
}

const char *playlistNext()
{
	playlist->userSelect = -1;
	if ( goc_arrayIsEmpty(playlist->files) ) {
		return NULL;
	}

	if ( playlist->nQueue )	{
		int inx = playlist->pQueue[0];
		playlistRemQueue( inx );
		return goc_arrayGet(playlist->files, inx);
	}

	if ( playlist->shufflemode & 1 )
	{
		if ( playlist->nShuffle != goc_arraySize(playlist->files) ) {
			playlistShuffleMode(playlist->shufflemode);
		}
		playlist->currShuffle++;
		playlist->currShuffle %= playlist->nShuffle;
		playlist->currFile = playlist->pShuffle[playlist->currShuffle];
	}
	else
	{
		playlist->currFile++;
		playlist->currFile %= goc_arraySize(playlist->files);
	}
	FileInfo* finfo = goc_arrayGet(playlist->files, playlist->currFile);
	return finfo->filename;
}

const char *playlistPrev( )
{
	playlist->userSelect = -1;
	if ( goc_arrayIsEmpty(playlist->files) ) {
		return NULL;
	}

	if ( playlist->shufflemode & 1 )
	{
		if ( playlist->nShuffle != goc_arraySize(playlist->files) ) {
			playlistShuffleMode(playlist->shufflemode);
		}
		if ( playlist->currShuffle ) {
			playlist->currShuffle--;
		} else {
			playlist->currShuffle = playlist->nShuffle - 1;
		}
		playlist->currFile = playlist->pShuffle[playlist->currShuffle];
	}
	else
	{
		if ( playlist->currFile )
			playlist->currFile--;
		else
			playlist->currFile = goc_arraySize(playlist->files) - 1;
	}
	FileInfo* finfo = goc_arrayGet(playlist->files, playlist->currFile);
	return finfo->filename;
}

void playlistShuffleMode(unsigned char mode)
{
	playlist->shufflemode = mode;
	if ( mode == SHUFFLE_RANDOM )
	{
		int i;
		// wyszukaj najblizsza liczbe pierwsza
		if ( goc_arrayIsEmpty( playlist->files ) ) {
			return;
		}

		// ustaw parametry losowania
		srand(time(0u));
		// usun stara liste shuffle
		if ( playlist->pShuffle )
		{
			playlist->pShuffle = goc_tableClear( playlist->pShuffle,
					&playlist->nShuffle);
		}
		// zaloz nowa liste shuffle
		playlist->nShuffle = goc_arraySize(playlist->files);
		playlist->pShuffle = (int*)malloc(sizeof(int)*playlist->nShuffle);
		for ( i=0; i<playlist->nShuffle; i++ )
			playlist->pShuffle[i] = i;

		// generuj liste shuffle
		for ( i = 0; i < playlist->nShuffle; i++ )
		{
			int tmp = playlist->pShuffle[i];
			int x = goc_random(playlist->nShuffle);
			playlist->pShuffle[i] = playlist->pShuffle[x];
			playlist->pShuffle[x] = tmp;

		}
	}
}

static void freeFileInfo(void* ptr) {
	struct FileInfo* finfo = (FileInfo*)ptr;
	goc_stringFree(finfo->filename);
	goc_stringFree(finfo->title);
	goc_stringFree(finfo->artist);
	goc_stringFree(finfo->album);
	goc_stringFree(finfo->year);
	goc_stringFree(finfo->comment);
	goc_stringFree(finfo->genre);
	free(finfo);
}

int playlistInit()
{
	playlist = (stPlaylist *) malloc(sizeof(stPlaylist));
	if(!playlist) {
		return -1;
	}
	memset(playlist,0,sizeof(stPlaylist));
	playlist->currFile = -1;
	playlist->currShuffle = -1;
	playlist->files = goc_arrayAlloc();
	playlist->files->freeElement = freeFileInfo;
	playlist->userSelect = -1;
	return 0;
}

void playlistClear()
{
	GOC_DEBUG("-> Playlist clear");
	playlist->files = goc_arrayFree( playlist->files );
	playlist->pShuffle = goc_tableClear(
		playlist->pShuffle, &playlist->nShuffle );
	playlist->pQueue = goc_tableClear(
		playlist->pQueue, &playlist->nQueue );
	playlist->userSelect = -1;
}

void playlistCleanup()
{
	GOC_DEBUG("-> Playlist cleanup");
	playlistClear();
	if ( playlist )
		free( playlist );
	playlist = NULL;
}

int playlistGetSize()
{
	if ( playlist ) {
		return goc_arraySize( playlist->files );
	}
	return 0;
}

int playlistGetActualPos()
{
	if ( playlist ) {
		if ( playlist->userSelect == -1 ) {
			return playlist->currFile;
		} else {
			return playlist->userSelect;
		}
	}
	return -1;
}

FileInfo* playlistGet( int pos )
{
	if (( playlist ) && (pos >=0) && ( pos < goc_arraySize( playlist->files) )) {
		return goc_arrayGet(playlist->files, pos);
	}
	return NULL;
}

GOC_Array* playlistGetTable( )
{
	if ( playlist ) {
		return playlist->files;
	}
	return NULL;
}

int playlistRemoveFile( int pos )
{
	int i, p = -1;
	if (( !playlist ) || ( pos < 0 ) || ( pos >= goc_arraySize(playlist->files) )) {
		return -1;
	}
	goc_arrayRemoveAt( playlist->files, pos );
	if ( playlist->pShuffle )
	{
		for ( i=0; i<playlist->nShuffle; i++ )
		{
			if ( playlist->pShuffle[i] > pos )
				playlist->pShuffle[i]--;
			else if ( playlist->pShuffle[i] == pos )
				p = i;
		}
		if ( p != -1 ) {
			playlist->pShuffle = goc_tableRemove(
				playlist->pShuffle, &playlist->nShuffle, sizeof(int),
				p );
		}
	}
	playlistRemQueue( pos );
	return 0;
}
/*
 * To co nie uwzglednilem:
 * wejscie stdin - nazwa '-'
 * wejscie http - nazwa 'http://'
 * wejscie ftp - nazwa 'ftp://'
 */
int playlistAddFile( const char* filename )
{
	ALLOC(struct FileInfo, fileInfo);
	if ( finfoInfo(filename, fileInfo) == FINFO_CODE_OK ) {
		goc_arrayAdd( playlist->files, fileInfo );
	}
	return 0;
}

int playlistStoreInFile( const char *filename )
{
	FILE *plik = NULL;
	int i;
	GOC_BDEBUG("Stroing playlist at %s", filename);
	if ( ! filename )
		return -1;
	plik = fopen(filename, "wb");
	if ( ! plik )
		return -1;
	for ( i=0; i<goc_arraySize(playlist->files); i++ )
	{
		FileInfo* fileInfo = goc_arrayGet(playlist->files, i);
		fprintf(plik, "%s\n", fileInfo->filename);
	}
	fclose( plik );
	return 0;
}

int playlistAddList( const char *filename )
{
	FILE *plik = NULL;
	char buf[1024];
	
	if ( !filename )
		return -1;

	plik = fopen(filename, "r");
	
	while ( fgets(buf, 1024, plik) ) {
		char* pos = NULL;
		pos = strchr(buf, '\n');
		if (pos) {
			*pos = 0;
		}
		pos = strchr(buf, '\r');
		if ( pos) {
			*pos = 0;
		}
		playlistAddFile(buf);
	}

	fclose(plik);
	return 0;
}

void playlistUserSelect( int pos ) {
	playlist->userSelect = pos;
}
