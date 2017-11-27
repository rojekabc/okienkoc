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

#include "playlist.h"

/* 
 * the playlist struct 
 */

typedef struct {
	char **pFilelist;
	_GOC_TABEL_SIZETYPE_ nFilelist;
	int *pShuffle;
	_GOC_TABEL_SIZETYPE_ nShuffle;
	int *pQueue;
	_GOC_TABEL_SIZETYPE_ nQueue;
	unsigned int currFile;
	unsigned int currShuffle;
	unsigned char shufflemode;
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
	if ( inx > playlist->nFilelist )
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
	if ( !playlist->pFilelist )
		return NULL;

	if ( playlist->nQueue )
	{
		int inx = playlist->pQueue[0];
		playlistRemQueue( inx );
		return playlist->pFilelist[ inx ];
	}

	if ( playlist->shufflemode & 1 )
	{
		if ( playlist->nShuffle != playlist->nFilelist )
			playlistShuffleMode(playlist->shufflemode);
		playlist->currShuffle++;
		playlist->currShuffle %= playlist->nShuffle;
		playlist->currFile = playlist->pShuffle[playlist->currShuffle];
		return playlist->pFilelist[ playlist->currFile ];
	}
	else
	{
		playlist->currFile++;
		playlist->currFile %= playlist->nFilelist;
		return playlist->pFilelist[ playlist->currFile ];
	}
}

const char *playlistPrev( )
{
	if ( !playlist->pFilelist )
		return NULL;

	if ( playlist->shufflemode & 1 )
	{
		if ( playlist->nShuffle != playlist->nFilelist )
			playlistShuffleMode(playlist->shufflemode);
		if ( playlist->currShuffle )
			playlist->currShuffle--;
		else
			playlist->currShuffle = playlist->nShuffle - 1;
		playlist->currFile = playlist->pShuffle[playlist->currShuffle];
		return playlist->pFilelist[ playlist->currFile ];
	}
	else
	{
		if ( playlist->currFile )
			playlist->currFile--;
		else
			playlist->currFile = playlist->nFilelist - 1;
		return playlist->pFilelist[ playlist->currFile ];
	}
}

void playlistShuffleMode(unsigned char mode)
{
	playlist->shufflemode = mode;
	if ( mode & 1 )
	{
		int i;
		// wyszukaj najblizsza liczbe pierwsza
		if ( playlist->nFilelist == 0 )
			return;

		// ustaw parametry losowania
		srand(time(0u));
		// usun stara liste shuffle
		if ( playlist->pShuffle )
		{
			playlist->pShuffle = goc_tableClear( playlist->pShuffle,
					&playlist->nShuffle);
		}
		// zaloz nowa liste shuffle
		playlist->pShuffle = (int*)malloc(
			sizeof(int)*playlist->nFilelist);
		playlist->nShuffle = playlist->nFilelist;
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

int playlistInit()
{
	playlist = (stPlaylist *) malloc(sizeof(stPlaylist));
	if(!playlist)
		return -1;
	memset(playlist,0,sizeof(stPlaylist));
	playlist->currFile = -1;
	playlist->currShuffle = -1;
	return 0;
}

void playlistClear()
{
	unsigned int i;
	if ( playlist->pFilelist )
		for ( i = 0; i < playlist->nFilelist; i++ )
			free( playlist->pFilelist[ i ] );
	playlist->pFilelist = goc_tableClear(
		playlist->pFilelist, &playlist->nFilelist);
	playlist->pShuffle = goc_tableClear(
		playlist->pShuffle, &playlist->nShuffle );
	playlist->pQueue = goc_tableClear(
		playlist->pQueue, &playlist->nQueue );
}

void playlistCleanup()
	// func do przebudowania
{
	playlistClear();
	if ( playlist )
		free( playlist );
	playlist = NULL;
}

int playlistGetSize()
{
	if ( playlist )
		return playlist->nFilelist;
	return 0;
}

int playlistGetActualPos()
{
	if ( playlist )
		return playlist->currFile;
	return -1;
}

const char *playlistGetFile( int pos )
{
	if (( playlist ) && (pos >=0) && ( pos < playlist->nFilelist ))
		return playlist->pFilelist[pos];
	return NULL;
}

const char **playlistGetTable( )
{
	if ( playlist )
		return (const char **)playlist->pFilelist;
	return NULL;
}

int playlistRemoveFile( int pos )
{
	int i, p = -1;
	if (( !playlist ) || ( pos < 0 ) || ( pos >= playlist->nFilelist ))
		return -1;
	playlist->pFilelist = goc_tableRemove(
		playlist->pFilelist, &playlist->nFilelist, sizeof(char *),
		pos);
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
int playlistAddFile( const char *filename )
{
	char *tmp, *p;
	
	if ( !filename )
		return -1;

	tmp = NULL;
	tmp = goc_stringCopy(tmp, filename);

	if ( (p = strchr(tmp, '\n')) )
		tmp = goc_stringDelAtPnt(tmp, p);
	if ( (p = strchr(tmp, '\r')) )
		tmp = goc_stringDelAtPnt(tmp, p);
	if ( strlen(tmp) == 0 )
	{
		free( tmp );
		return -1;
	}

	playlist->pFilelist = goc_tableAdd(
		playlist->pFilelist, &playlist->nFilelist, sizeof(char *));
	playlist->pFilelist[playlist->nFilelist-1] = tmp;
		
	return 0;
}

int playlistStoreInFile( const char *filename )
{
	FILE *plik = NULL;
	int i;
	printf("Storing at %s\n", filename);
	fflush(stdout);
	if ( ! filename )
		return -1;
	plik = fopen(filename, "wb");
	if ( ! plik )
		return -1;
	for ( i=0; i<playlist->nFilelist; i++ )
	{
		fprintf(plik, "%s\n", playlist->pFilelist[i]);
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
	
	while ( fgets(buf, 1024, plik) )
		playlistAddFile(buf);

	fclose(plik);
#ifdef _VERBOSE_
	printf("Now id %d files on playlist.\n", playlist->nFilelist);
#endif
	return 0;
}
