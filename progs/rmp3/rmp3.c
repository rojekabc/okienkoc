#ifdef _DEBUG
#	include <mpatrol.h>
#endif // ifdef _DEBUG

#include <stdlib.h>
#include <sys/types.h>
#if !defined(WIN32) && !defined(GENERIC)
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>
#include <pwd.h>
#include <tools/plik.h>
#define GOC_PRINTINFO
#define GOC_PRINTERROR
#ifdef _DEBUG_
	#define GOC_PRINTDEBUG
#endif
#include <tools/log.h>
#include <tools/memostream.h>
#include <tools/mystr.h>
#include <tools/term.h>

#if 0
#	define SET_RT 
#endif
#ifdef SET_RT
#	include <sched.h>
#endif

#include "mixer.h"
#ifdef OSS_MIXER_ENABLE
#	include "oss-mixer.h"
#endif
#ifdef ALSA_MIXER_ENABLE
#	include "alsa-mixer.h"
#endif
#include "mod_fun.h"
#include "player.h"
#include "decoder.h"
#include "finfo.h"
#include "control.h"
#include "playlist.h"
#ifdef HAS_LIBSOX
#	include <sox.h>
#endif

#define PREFIX "/usr/local"
#define PROGNAME "rmp3"

char *prgName = NULL;
MixerHandler mixer = 0;

int OutputDescriptor;

static void set_shuffle (char arg)
{
	if ( arg == 'z' )
		playlistShuffleMode(SHUFFLE_RANDOM);
	else if ( arg == 'Z' )
		playlistShuffleMode(SHUFFLE_RANDOM);
	else
		playlistShuffleMode(0);
}


pthread_t threadPlay = 0;
pthread_mutex_t threadMutex;
pthread_mutex_t mutexPlay;

void endProgram(int c)
{
	/*
	if ( outbuf )
	{
		free( outbuf );
		outbuf = NULL;
	}
	*/
	GOC_INFO("Cleaning control module");
	controlCleanup();
	printf("Cleaning playlist handler\n");
	playlistCleanup();

	exit(c);
}


#define MODE_STOP 0
#define MODE_PLAY 1
#define MODE_PAUSE 2
#define MODE_QUIT 3
#define MODE_PREV 4
#define MODE_NEXT 5
int mode = MODE_STOP; // TODO: DODAC MUTEX !!!!!!!!!!!!!!!!!

int doAction(unsigned int action, const void *param);

void eventChangeSongPlaying()
{
	controlEvent(EVENT_CHANGE_SONG);
}
const char *filename = NULL;

static void closeAudioFile()
{
	GOC_DEBUG("-> closeAudioFile");
	filename = NULL;
	decoderClose();
	playerCloseOutput();
	GOC_DEBUG("<- closeAudioFile");
}

#define N_SAMPLES 512
#define N_CHANNELS 2
int16_t playBuf[N_SAMPLES * N_CHANNELS];

// Otwiera plik, ustala dla jego format oraz kodek, jaki bêdzie u¿yty
// Zwraca -1 jesli blad
static int openAudioFile(const char *cfilename)
{
	GOC_BDEBUG("-> openAudioFile: %s", cfilename);

	// wstepne inicjowanie i sprzatanie
	closeAudioFile();

	// czy wskazano jaki¶ plik
	if ( cfilename == NULL )
		return -1;
	// czy plik istnieje
	if ( ! goc_isFileExists( cfilename ) )
		return -1;
	if ( goc_isFolder( cfilename ) )
		return -1;
	// ustawienie nowego pliku odtwarzanego
	filename = cfilename;
	// otworz plik
	GOC_CHEOP(decoderOpen( filename, N_SAMPLES * N_CHANNELS ) == DECODER_CODE_OK, filename = NULL; return -1);
	GOC_BDEBUG("<- openAudioFile: %s", cfilename);
	eventChangeSongPlaying();
	if ( playerOpenOutput() == PLAYER_CODE_OK ) {
		return 0;
	} else {
		return -1;
	}
}

GOC_OStream* ostream = NULL;

// zwraca 1, gdy ok, 0 gdy koniec
int playOneFrame() {
	size_t size;
	if (decoderRead(playBuf, &size) != DECODER_CODE_OK) {
		GOC_ERROR("Bad decoder read");
		closeAudioFile();
		return 0;
	}
	if ( size == 0 ) {
		closeAudioFile();
		return 0;
	}
	if (playerPlay(playBuf, size) != PLAYER_CODE_OK) {
		GOC_ERROR("Error play");
		closeAudioFile();
		return 0;
	}
	return 1;
}

#define NUMBER_OF_TRIES 10
#define TRY_OPEN(nextfun) \
{ \
	int i=0; \
	while ( openAudioFile( nextfun() ) && (i++ < NUMBER_OF_TRIES) ); \
}

void *playFile(void *arg)
{
//	int old = MODE_PLAY;

	// graj dopóty nie ma nakazano koñca
	while ( mode != MODE_QUIT )
	{
		pthread_mutex_lock(&mutexPlay);
		if ( mode == MODE_PLAY )
		{
			// próba wej¶cia w tryb grania
			while (( mode == MODE_PLAY ) && (playOneFrame()));
			pthread_mutex_unlock(&mutexPlay);
			if ( mode == MODE_PLAY )
			{
				doAction(ACTION_NEXT, NULL);
			}
		}
		else
		{
			pthread_mutex_unlock(&mutexPlay);
		}
	}

	return NULL;
}

void next_aumix_port()
{
	int amount = 0;
	MixerElement* active = NULL;
	MixerElement* element = NULL;
	int activepos;

	mixerGetElementsAmount( mixer, &amount );
	mixerGetSelectedElement( mixer, &active );

	// Get an active's position
	for (activepos=0; activepos<amount; activepos++) {
		mixerGetElementInfo( mixer, activepos, &element );
		if ( element == active ) {
			break;
		}
	}
	if ( activepos == amount ) {
		// Cannot find active position
		return;
	}

	// Find next
	int i;
	for (i=(activepos+1) % amount; i != activepos; i = (i+1) % amount) {
		mixerGetElementInfo( mixer, i, &element );
		if ( element->type == MIXER_ELEMENT_PLAYBACK ) {
			break;
		}
	}
	if ( i == activepos ) {
		// Nothing changed
		return;
	}
	mixerSelectElement( mixer, i );
}

int doAction(unsigned int action, const void *param)
{
	GOC_BDEBUG("Call action %d", action);
	int old;
	switch ( action )
	{
		case ACTION_AUMIX_PLUS: // Increase volume
			mixerChangeVolume(mixer, 1, NULL);
			break;
		case ACTION_AUMIX_MINUS: // Decrease volume
			mixerChangeVolume(mixer, -1, NULL);
			break;
		case ACTION_AUMIX_NEXTDEV:
			next_aumix_port();
			break;
		case ACTION_AUMIX_PREVDEV:
			break;
		case ACTION_AUMIX_CURRDEV: {
			MixerElement* active = NULL;
			mixerGetSelectedElement(mixer, &active);
			*(int*)param = (int)active->volume;
			break;
		}
		case ACTION_AUMIX_CURRNAME: {
			MixerElement* active = NULL;
			mixerGetSelectedElement(mixer, &active);
			*(char**)param = active->name;
			break;
		}
		case ACTION_NEXT:
			old = mode;
			// zatrzymaj granie
			if ( mode == MODE_PLAY )
			{
				mode = MODE_STOP;
				pthread_mutex_lock(&mutexPlay);
			}
			// zmieñ piosenkê
			TRY_OPEN( playlistNext );
			mode = old;
			if ( filename )
			{
				// pu¶æ granie dalej
				if ( mode == MODE_PLAY )
					pthread_mutex_unlock(&mutexPlay);
			}
			else
				mode = MODE_STOP;
			return 0;
		case ACTION_PREV:
			old = mode;
			// zatrzymaj granie
			if ( mode == MODE_PLAY )
			{
				mode = MODE_NEXT;
				pthread_mutex_lock(&mutexPlay);
			}
			// zmieñ piosenkê
			TRY_OPEN( playlistPrev );
			mode = old;
			if ( filename )
			{
				// pu¶æ granie dalej
				if ( mode == MODE_PLAY )
					pthread_mutex_unlock(&mutexPlay);
			}
			else
				mode = MODE_STOP;
			return 0;
		case ACTION_PAUSE:
			if ( mode == MODE_PLAY )
			{
				// zatrzymaj granie
				mode = MODE_PAUSE;
				pthread_mutex_lock(&mutexPlay);
			}
			else
			{
				// je¶li wcze¶niej nie wybrano piosenki, wybierz
				if ( filename == NULL )
					TRY_OPEN( playlistNext );
				if ( filename )
				{
					// pu¶æ granie dalej
					mode = MODE_PLAY;
					pthread_mutex_unlock(&mutexPlay);
				}
			}
			return 0;
		case ACTION_PLAY:
			if ( mode == MODE_PLAY )
			{
				// zatrzymaj granie
				mode = MODE_STOP;
				pthread_mutex_lock(&mutexPlay);
			}
			// zmieñ piosenkê
			if ( param != NULL ) {
				openAudioFile((const char*)param); //TODO: -1
			} else {
				openAudioFile(filename); //TODO: -1
			}
			if ( filename )
			{
				mode = MODE_PLAY;
				// pu¶æ granie dalej
				pthread_mutex_unlock(&mutexPlay);
			}
			else
			{
				mode = MODE_STOP;
			}
			return 0;
		case ACTION_START:
			pthread_mutex_init(&threadMutex, NULL);
			pthread_mutex_init(&mutexPlay, NULL);
			pthread_mutex_lock(&mutexPlay);
			TRY_OPEN( playlistNext );
			if ( filename )
			{
				mode = MODE_PLAY;
				pthread_mutex_unlock(&mutexPlay);
			}
			else
			{
				mode = MODE_STOP;
			}
			pthread_create(&threadPlay, NULL, &playFile, NULL);
			return 0;
		case ACTION_QUIT:
			printf("Quit and clean thread.\n");
			fflush( stdout );
			// zatrzymaj granie
			if ( mode != MODE_STOP )
			{
				mode = MODE_STOP;
				pthread_mutex_lock(&mutexPlay);
				mode = MODE_QUIT;
				pthread_mutex_unlock(&mutexPlay);
			}
			else
			{
				mode = MODE_QUIT;
				pthread_mutex_unlock(&mutexPlay);
			}
			pthread_mutex_destroy(&threadMutex);
			pthread_mutex_destroy(&mutexPlay);
			pthread_join( threadPlay, NULL );
			return 0;
		case ACTION_STOP:
			if ( mode == MODE_PLAY )
			{
				// zatrzymaj granie
				mode = MODE_STOP;
				pthread_mutex_lock(&mutexPlay);
			}
			else
			{
				if ( filename )
					// odtwórz od pocz±tku
					openAudioFile(filename); // TODO: -1
				else
					// znajd¼ plik do otworzenia
					TRY_OPEN(playlistNext);
				if ( filename )
				{
					// pu¶æ granie dalej
					mode = MODE_PLAY;
					pthread_mutex_unlock(&mutexPlay);
				}
				else
				{
					mode = MODE_STOP;
				}
			}
			return 0;
		case ACTION_INFO:
		{
			FileInfo* fileInfo = (FileInfo*)param;
			fileInfo->filename = goc_stringCopy(fileInfo->filename, filename);
			finfoInfo(filename, fileInfo);
			return 0;
		}
		default:
			return ERR_UNKNOWN_ACTION;
	}
	return 0;
}

static void initializeMixerSystem(MixerHandler handler) {
	int camount = 0;
	int eamount = 0;
	int i, j;

	GOC_CHEOP_DEBUG(mixerGetCardsAmount(handler, &camount) == MIXER_CODE_OK, return);
	GOC_CHEOP_DEBUG(camount > 0, return);
	for ( i=0; i<camount; i++ ) {
		GOC_CHEOP_DEBUG(mixerSelectCard(handler, i) == MIXER_CODE_OK, continue );
		GOC_CHEOP_DEBUG(mixerGetElementsAmount(handler, &eamount) == MIXER_CODE_OK, continue);
		GOC_CHEOP_DEBUG(eamount > 0, continue);
		for ( j=0; j<eamount; j++ ) {
			MixerElement* element = NULL;
			GOC_CHEOP_DEBUG(mixerGetElementInfo(handler, j, &element) == MIXER_CODE_OK, continue);
			if ( element->type == MIXER_ELEMENT_PLAYBACK ) {
				GOC_CHEOP_DEBUG(mixerSelectElement(handler, j) == MIXER_CODE_OK, continue);
				GOC_INFO("Select mixer system");
				mixer = handler;
				return;
			}
		}
	}
}

static void initializeMixer() {
#ifdef OSS_MIXER_ENABLE
	if ( mixer == 0 ) {
		GOC_INFO("Checking OSS Mixer");
		MixerHandler handler = ossMixerAlloc();
		initializeMixerSystem( handler );
		if ( mixer == 0 ) {
			mixerFree(handler);
		}
	}
#endif

#ifdef ALSA_MIXER_ENABLE
	if ( mixer == 0 ) {
		GOC_INFO("Checking ALSA Mixer");
		MixerHandler handler = alsaMixerAlloc();
		initializeMixerSystem( handler );
		if ( mixer == 0 ) {
			mixerFree(handler);
		}
	}
#endif
	GOC_MSG_CHEOP("Cannot find working mixer system", mixer != 0, endProgram(1));
}

int main(int argc, char *argv[])
{
	unsigned char isLoadFromCmd = 0;

	/* Inicjowanie mixera */
	initializeMixer();

	// Ustal warto¶ci z pliku konfiguracyjnego dla po³o¿enia modu³ów
	{
		struct passwd *pwd = NULL;
		char *konfname = NULL;
		FILE *plik;
		char pBuf[256];

		pwd = getpwuid( getuid() );
		if ( !pwd )
		{
			GOC_ERROR("B³±d wykonywania getpwuid");
			endProgram(1);
		}
		konfname = goc_stringCopy(konfname, pwd->pw_dir);
		konfname = goc_stringAdd(konfname, "/.rmp3");
		if ( !goc_isFileExists(konfname) )
		{
			plik = fopen(konfname, "w");
			if ( plik == NULL )
			{
				GOC_BERROR("Nie mogê za³o¿yæ pliku ustawieñ [%s]", konfname);
				endProgram(1);
			}
			fclose(plik);
		}
		plik = fopen(konfname, "r");
		if ( plik == NULL )
		{
			GOC_BERROR("Nieudany odczyt pliku ustawieñ [%s]", konfname);
			endProgram(1);
		}
		while ( fgets(pBuf, sizeof(pBuf), plik) )
		{
			char *val;
			if ( strchr(pBuf, '\n') )
				*(strchr(pBuf, '\n')) = 0;
			if ( strchr(pBuf, '\r') )
				*(strchr(pBuf, '\r')) = 0;
			val = strchr(pBuf, '=');
			if ( !val )
				continue;
			*val = 0;
			val++;
			if ( *val == 0 )
				continue;
			/*
			else if ( strncmp("aumix:", pBuf, strlen("aumix:"))==0 )
			{
				if ( SetCurrentDev(pBuf+strlen("aumix:"))!=-1 )
					AdjustLevel(current_dev, 0, atoi(val));
				else
					GOC_ERROR("Wskazano z³e urz±dzenie aumix");
			}
			*/
		}
		fclose(plik);
	}
#ifdef HAS_LIBSOX
//	GOC_CHEOP(sox_init() == SOX_SUCCESS, endProgram(1));
#endif
	/* Wstepne inicjowanie modulu playlist */
	// ---------------------------------------------------------------
	if ( playlistInit() )
	{
		GOC_ERROR("Inicjowanie modu³u playlist zakoñczone b³êdem");
		endProgram(1);
	}
	/* Inicjowanie modulu kontrolera */
	// ---------------------------------------------------------------
	if ( controlInit( &doAction ) )
	{
		GOC_ERROR("Inicjowanie modu³u control zakoñczone b³êdem");
		endProgram(1);
	}

	// Obs³uga parametrów wywo³ania programu
	{
		int i;
		(prgName = strrchr(argv[0], '/')) ? prgName++ : (prgName = argv[0]);
		for (i=1; i<argc; i++)
		{
			if ( argv[i][0] == '-' )
			{
				switch ( argv[i][1] )
				{
					case 'z':
					case 'Z':
						set_shuffle('z');
						break;
					case '@':
						playlistAddList( argv[i]+2 );
						isLoadFromCmd = 1;
						GOC_BINFO("Adding playlist [%s]", argv[i]+2);
						break;
					default:
						// TODO: Unknown option
						break;
				}
			}
			else
			{
				playlistAddFile( argv[i] );
				isLoadFromCmd = 1;
			}
		}
	}

	// Je¶li za³adowano z pliku konfiguracyjnego - nie ³aduj z konfiguracji
	if ( ! isLoadFromCmd )
	{
		struct passwd *pwd = NULL;
		char *konfname = NULL;

		pwd = getpwuid( getuid() );
		if ( ! pwd )
			GOC_ERROR("B³±d wykonywania getpwuid");
		konfname = goc_stringCopy(konfname, pwd->pw_dir);
		konfname = goc_stringAdd(konfname, "/.rmp3playlist");
		if ( goc_isFileExists( konfname ) )
		{
			playlistAddList( konfname );
		}
		konfname = goc_stringFree( konfname );
	}

	// init player
	GOC_CHEOP(playerInitialize() == PLAYER_CODE_OK, endProgram(-1));
	// init decoder
	GOC_CHEOP(decoderInitialize() == DECODER_CODE_OK, endProgram(-1));
	GOC_CHEOP(finfoInitialize() == FINFO_CODE_OK, endProgram(-1));
	// rozpoczecie grania
	controlStart();

	mixerFree( mixer );
	// save playlist
	if ( ! isLoadFromCmd )
	{
		struct passwd *pwd = NULL;
		char *konfname = NULL;

		pwd = getpwuid( getuid() );
		if ( ! pwd )
			GOC_ERROR("B³±d wykonywania getpwuid");
		konfname = goc_stringCopy(konfname, pwd->pw_dir);
		konfname = goc_stringAdd(konfname, "/.rmp3playlist");
		playlistStoreInFile( konfname );
		konfname = goc_stringFree( konfname );
	}
	endProgram(0);
}
