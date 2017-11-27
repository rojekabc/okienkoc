#ifndef _MOD_FUN_H_
#define _MOD_FUN_H_

/* 
 * Moudu³ kontolera
 */
#define ACTION_PLAY 1
#define ACTION_STOP 2
#define ACTION_NEXT 3
#define ACTION_QUIT 4
#define ACTION_PREV 5
/**
 * Fill information structure {@link FileInfo}.
 */
#define ACTION_INFO 6
#define ACTION_PAUSE 7
#define ACTION_AUMIX 8
#define ACTION_START 9
#define ACTION_SHUFFLE 10
#define ACTION_PLAYLIST_ADDFILE 11
#define ACTION_PLAYLIST_ADDLIST 12
#define ACTION_PLAYLIST_CLEAR 13
#define ACTION_PLAYLIST_REMOVEFILE 14
#define ACTION_PLAYLIST_GETSIZE 15
#define ACTION_PLAYLIST_GETFILE 16
#define ACTION_PLAYLIST_GETTABLE 17
#define ACTION_PLAYLIST_GETACTUAL 18
#define ACTION_PLAYLIST_ADDQUEUE 19
#define ACTION_PLAYLIST_REMQUEUE 20
#define ACTION_PLAYLIST_ISQUEUE 21
#define ACTION_PLAYLIST_STORE 22
/* audio mixer actions */
// increase the volume
#define ACTION_AUMIX_PLUS	41
// decrease the volume
#define ACTION_AUMIX_MINUS	42
// the next device
#define ACTION_AUMIX_NEXTDEV	43
// the previous device
#define ACTION_AUMIX_PREVDEV	44
// get a volume level from the current device
// param - pointer to int
#define ACTION_AUMIX_CURRDEV	45
// get a name of the current device
// param - pointer to char*
#define ACTION_AUMIX_CURRNAME	46

#define allocFileInfo() \
	memset(malloc(sizeof(FileInfo)), 0, sizeof(FileInfo));

#define freeFileInfo( fileInfoPtr ) \
	fileInfoPtr->filename = goc_stringFree( fileInfoPtr->filename ); \
	fileInfoPtr->title = goc_stringFree( fileInfoPtr->title ); \
	fileInfoPtr->artist = goc_stringFree( fileInfoPtr->artist ); \
	fileInfoPtr->album = goc_stringFree( fileInfoPtr->album ); \
	fileInfoPtr->year = goc_stringFree( fileInfoPtr->year ); \
	fileInfoPtr->comment = goc_stringFree( fileInfoPtr->comment ); \
	fileInfoPtr->genre = goc_stringFree( fileInfoPtr->genre ); \
	free( fileInfoPtr );


/* NOT IN USE
#define INFO_POSITION_CUR 8
#define INFO_POSITION_MAX 9
#define INFO_TIME_CUR 10
#define INFO_TIME_MAX 11
*/

#define SHUFFLE_YES 1
#define SHUFFLE_NO 2
#define SHUFFLE_REINIT 3
typedef struct
{
	char *pBuf;
	unsigned int uBuf;
	unsigned int uType;
} stInfo;

#define ERR_UNKNOWN_ACTION 1
#define ERR_NO_FILENAME 2

#define EVENT_FINISH_SONG 1
#define EVENT_CHANGE_SONG 2

#define OUTPARAM_RATE 0
#define OUTPARAM_CHANNELS 1
#define OUTPARAM_FORMAT 2
#define OUTPARAM_QUEUEFLUSH 3
#define OUTPARAM_GETFORMATS 4

#define AUDIO_FORMAT_MASK	  0x100
#define AUDIO_FORMAT_16		  0x100
#define AUDIO_FORMAT_8		  0x000

#define AUDIO_FORMAT_SIGNED_16    0x110
#define AUDIO_FORMAT_UNSIGNED_16  0x120
#define AUDIO_FORMAT_UNSIGNED_8   0x1
#define AUDIO_FORMAT_SIGNED_8     0x2
#define AUDIO_FORMAT_ULAW_8       0x4
#define AUDIO_FORMAT_ALAW_8       0x8

/* 3% rate tolerance */
#define AUDIO_RATE_TOLERANCE	  3

#endif
