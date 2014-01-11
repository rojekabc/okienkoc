#ifndef _MOD_FUN_H_
#define _MOD_FUN_H_

#ifdef MOD_EXTERN
#define EXTERN extern
#define MOD_PROGRAM
#else
#define EXTERN
#endif

/*
 * Modu³ playlist
 */
#define SHUFFLE_RANDOM 1

#ifdef MOD_PROGRAM
EXTERN void *hanPlaylist;
EXTERN int (*mod_playlistInit)();
EXTERN void (*mod_playlistCleanup)();
EXTERN const char *(*mod_playlistNext)();
EXTERN const char *(*mod_playlistPrev)();
EXTERN void (*mod_playlistShuffleMode)(unsigned char);
EXTERN int (*mod_playlistAddList)(const char*);
EXTERN int (*mod_playlistAddFile)(const char*);
EXTERN void (*mod_playlistClear)();
EXTERN int (*mod_playlistGetSize)();
EXTERN const char *(*mod_playlistGetFile)( int pos );
EXTERN const char **(*mod_playlistGetTable)();
EXTERN int (*mod_playlistRemoveFile)( int pos );
EXTERN int (*mod_playlistGetActualPos)();
EXTERN void (*mod_playlistQueue)( int pos );
EXTERN void (*mod_playlistRemQueue)( int pos );
EXTERN int (*mod_playlistIsQueue)( int pos );
EXTERN int (*mod_playlistStoreInFile)( const char *filename );
#endif

#ifdef MOD_PLAYLIST
int playlistInit();
void playlistCleanup();
int playlistAddList( const char *filename );
int playlistAddFile( const char *filename );
const char *playlistNext();
const char *playlistPrev();
void playlistShuffleMode(unsigned char mode);
void playlistClear();
int playlistGetSize();
const char *playlistGetFile( int pos );
int playlistRemoveFile( int pos );
const char **playlistGetTable();
int playlistGetActualPos();
void playlistQueue( int pos );
void playlistRemQueue( int pos );
int playlistIsQueue( int pos );
int playlistStoreInFile( const char *filename );
#endif

/* 
 * Moudu³ kontolera
 */
#define ACTION_PLAY 1
#define ACTION_STOP 2
#define ACTION_NEXT 3
#define ACTION_QUIT 4
#define ACTION_PREV 5
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

#define INFO_FILE 1
#define INFO_TITLE 2
#define INFO_ARTIST 3
#define INFO_ALBUM 4
#define INFO_YEAR 5
#define INFO_COMMENT 6
#define INFO_GENRE 7
#define INFO_POSITION_CUR 8
#define INFO_POSITION_MAX 9
#define INFO_TIME_CUR 10
#define INFO_TIME_MAX 11

#define AUMIX_PLUS 1
#define AUMIX_MINUS 2
#define AUMIX_NEXTDEV 3
#define AUMIX_PREVDEV 4
#define AUMIX_CURRDEV 5
#define AUMIX_CURRNAME 6

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

#ifdef MOD_PROGRAM
EXTERN void *hanControl;
EXTERN int (*mod_controlInit)(int (*)(unsigned int, void*));
EXTERN void (*mod_controlCleanup)();
EXTERN int (*mod_controlStart)();
EXTERN int (*mod_controlEvent)(unsigned int);
#endif

#ifdef MOD_CONTROL
int controlInit(int (*)(unsigned int, void*));
void controlCleanup();
int controlStart();
int controlEvent(unsigned int);
#endif

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

#ifdef MOD_PROGRAM
//EXTERN void *hanOutput;
//EXTERN int (*mod_outputInit)();
//EXTERN int (*mod_outputPlaySample)(const unsigned char *pBuf, unsigned int cBuf);
//EXTERN void (*mod_outputCleanup)();
//EXTERN void (*mod_outputSetParameter)(int param, void *value);
//EXTERN int (*mod_outputGetParameter)(int param);
#endif
#ifdef MOD_OUTPUT
int outputInit();
int outputPlaySample(const unsigned char *pBuf, unsigned int cBuf);
void outputCleanup();
void outputSetParameter(int param, void *value);
int outputGetParameter(int param);
#endif

#undef EXTERN

#endif
