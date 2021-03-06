#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include "finfo.h"

#define SHUFFLE_RANDOM 1

int playlistInit();
void playlistCleanup();

void playlistClear();
int playlistAddList( const char *filename );
int playlistAddFile( const char *filename );
int playlistRemoveFile( int pos );

const char *playlistNext();
const char *playlistPrev();
void playlistShuffleMode(unsigned char mode);
int playlistStoreInFile( const char *filename );

int playlistIsQueue( int inx );
void playlistQueue( int inx );
void playlistRemQueue( int inx );

// int playlistGetSize();
struct FileInfo* playlistGet( int pos );
GOC_Array* playlistGetTable( );
int playlistGetActualPos();
void playlistUserSelect( int pos );

#endif /* ifndef _PLAYLIST_H_ */
