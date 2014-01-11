#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include <okienkoc/tablica.h>

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

/*
	 struct playlist *new_playlist(int argc,char **argv,char *listname, int loptind);
	 */
int playlistInit();
void playlistCleanup();
//int playlistCreate(	int argc, char **argv, char *listname,int lopid );
int playlistAddList( const char *filename );
int playlistAddFile( const char *filename );
const char *playlistNext();
const char *playlistPrev();
void playlistShuffleMode(unsigned char mode);
int playlistStoreInFile( const char *filename );
#endif /* ifndef _PLAYLIST_H_ */
