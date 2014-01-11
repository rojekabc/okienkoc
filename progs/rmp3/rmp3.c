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

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <ao/ao.h>

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>
#include <pwd.h>
#include <okienkoc/plik.h>
#define GOC_PRINTERROR
#include <okienkoc/log.h>
#include <okienkoc/memostream.h>

#if 0
#define SET_RT 
#endif


#ifdef SET_RT
#include <sched.h>
#endif

#include <okienkoc/term.h>
#include "aumix.h"
/*#include "playlist.h"*/
#define MOD_PROGRAM
#include "mod_fun.h"
#include <okienkoc/mystr.h>

#define PREFIX "/usr/local"
#define PROGNAME "rmp3"

char *prgName = NULL;

int OutputDescriptor;

static void set_shuffle (char arg)
{
	if ( arg == 'z' )
		mod_playlistShuffleMode(SHUFFLE_RANDOM);
	else if ( arg == 'Z' )
		mod_playlistShuffleMode(SHUFFLE_RANDOM);
	else
		mod_playlistShuffleMode(0);
}


pthread_t threadPlay = 0;
pthread_mutex_t threadMutex;
pthread_mutex_t mutexPlay;
// Bufory dla zdekodowanych informacji
static int16_t *outbuf = NULL;
static int16_t *outbufAlign = NULL;
/*
static int16_t *outbufResampler = NULL;
*/
static size_t outsize;

void endProgram(int c)
{
	if ( outbuf )
	{
		free( outbuf );
		outbuf = NULL;
	}
	if ( hanControl )
	{
		printf("Cleaning control handler\n");
		mod_controlCleanup();
		dlclose( hanControl );
		hanControl = 0;
	}
	if ( hanPlaylist )
	{
		printf("Cleaning playlist handler\n");
		mod_playlistCleanup();
		dlclose( hanPlaylist );
		hanPlaylist = 0;
	}
	// zakoñcz dzia³anie programu
	exit(c);
}


#define MODE_STOP 0
#define MODE_PLAY 1
#define MODE_PAUSE 2
#define MODE_QUIT 3
#define MODE_PREV 4
#define MODE_NEXT 5
int mode = MODE_STOP; // TODO: DODAC MUTEX !!!!!!!!!!!!!!!!!

int doAction(unsigned int action, void *param);

void eventChangeSongPlaying()
{
	mod_controlEvent(EVENT_CHANGE_SONG);
}
const char *filename = NULL;

// ------------------ LIBAO BEGIN ------------------------ //
// AV
AVFormatContext *pFormatCtx = NULL;
AVCodecContext *pCodecCtx = NULL;
AVCodec *pCodec = NULL;
AVPacket avpacket;
struct AVResampleContext* resampleContext = NULL;

// AV

int aodriverid = -1;
ao_device *aodev = NULL;
void closeOutputDevice()
{
	if ( aodev )
	{
		ao_close(aodev);
		aodev = NULL;
	}
}

int openOutputDevice()
{
	ao_sample_format aosampleformat;

	// In new version of libao it's added field matrix.
	// Clear all struucture before use
	memset( &aosampleformat, 0, sizeof(ao_sample_format) );

	aosampleformat.bits = 16;
	// aosampleformat.rate = pCodecCtx->sample_rate;
	aosampleformat.rate = 48000;
	aosampleformat.channels = pCodecCtx->channels;
	aosampleformat.byte_format = AO_FMT_LITTLE;

	closeOutputDevice();

	// TODO: Mozliwosc wyboru plik a urzadzenie
	aodev = ao_open_live(aodriverid, &aosampleformat, NULL);
	if ( !aodev )
		return -1; // TODO: Log Error

	// zamknij resampler'a jezeli byl otwarty
	if ( resampleContext )
	{
		// GOC_ERROR("Close resampler");
		av_resample_close(resampleContext);
		//free(outbufResampler);
		resampleContext = NULL;
	}

	if ( pCodecCtx->sample_rate != 48000 )
	{
		// GOC_ERROR("Open resampler");
		resampleContext = av_resample_init(
			48000, // rate out
			pCodecCtx->sample_rate, // rate in
			16, // filter length
			10, // phase count
			0, // linear FIR filter
			0.7); // cutoff frequency
/*		if ( resampleContext )
		{
			GOC_ERROR("Resampler opened");
		//	outbufResampler = malloc( outsize );
		}
		else
			GOC_ERROR("Cannot open resampler");*/

	}
	return 0;
}
// ------------------ LIBAO END ------------------------ //
// ------------------ LIBAVCODEC LIBAVFORMAT BEGIN ------------------------ //
static uint8_t *inbuf;
static int insize;
static int audioStream = 0;

static int logAVError(int averror)
{
	// TODO: Log error from AV_ERROR
	return -1;
}

static void closeAudioFile()
{
	filename = NULL;
	if ( pFormatCtx )
	{
		av_close_input_file( pFormatCtx );
		pFormatCtx = NULL;
	}
	if ( pCodecCtx )
	{
		avcodec_close( pCodecCtx );
		pCodecCtx = NULL;
	}
	pCodec = NULL;
	closeOutputDevice();
}

// Wyrównywanie w pamiêci do bloków 16 - bajtowych (QWORD)
// dla operacji bazuj±cych na SSE / MMX
static void* align16(void* ptr, size_t* size)
{
	size_t change = ((size_t)ptr) & 0xF;
	if ( change )
	{
		char* newptr = (char*)ptr;
		newptr += 16;
		newptr -= change;
		*size -= change;
		return newptr;
	}
	else
	{
		return ptr;
	}
}


// Otwiera plik, ustala dla jego format oraz kodek, jaki bêdzie u¿yty
// Zwraca -1 jesli blad
static int openAudioFile(const char *cfilename)
{
	int err;
	int i;
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
	err = av_open_input_file( &pFormatCtx, filename, NULL, 0, NULL);
	if ( err != 0 )
	{
		filename = NULL;
		return logAVError(err);
	}
	err = av_find_stream_info( pFormatCtx );
	if ( err < 0 )
	{
		filename = NULL;
		return logAVError(err);
	}
	// wyszukaj strumien audio i otworz dla niego codec
	for (i=0; i<pFormatCtx->nb_streams; i++)
	{
		switch (pFormatCtx->streams[i]->codec->codec_type)
		{
			case CODEC_TYPE_AUDIO:
				pCodecCtx = pFormatCtx->streams[i]->codec;
				if ( pCodecCtx->channels > 0 )
					pCodecCtx->request_channels = pCodecCtx->channels <= 2 ? pCodecCtx->channels : 2;
				else
					pCodecCtx->request_channels = 2;
				/*
				if ( pCodecCtx->sample_rate = 44100 )
				{
					GOC_ERROR("CHANGE SAMPLE RAT from 44100 to 48000");
					pCodecCtx->sample_rate = 48000;
				}
				*/
				pCodec = avcodec_find_decoder(
					pCodecCtx->codec_id);
				if ( !pCodec )
					return -1; // TODO: Log error

				err = avcodec_open(pCodecCtx, pCodec);
				if ( err < 0 )
					return logAVError(err);
				audioStream = i;
				break;
			default:
				break;
		}
		if ( pCodecCtx )
			break;
	}
	if ( !pCodecCtx )
	{
		filename = NULL;
		return -1; // TODO: Log error
	}
//	pCodecCtx->request_channel_layout = CH_LAYOUT_STEREO_DOWNMIX;
	pCodecCtx->request_channel_layout = CH_LAYOUT_MONO;
	eventChangeSongPlaying();
	return openOutputDevice();
}

GOC_OStream* ostream = NULL;

// zwraca 1, gdy ok, 0 gdy koniec
int playOneFrame()
{
	int decodedlen;
	size_t outsizeAlign;
//
	// Break on Read Frame With Audio
	do
	{
		// Free Data if Any
		if ( avpacket.data )
			av_free_packet( &avpacket );
		// Read Frame
		if ( av_read_frame(pFormatCtx, &avpacket) < 0 )
			return 0; // END STREAM
	}
	while ( avpacket.stream_index != audioStream );

	inbuf = avpacket.data;
	insize = avpacket.size;

	while (insize > 0)
	{
		outsizeAlign = outsize;
		decodedlen = avcodec_decode_audio2(
			pCodecCtx,
			outbufAlign, &outsizeAlign,
			inbuf, insize);
		insize -= decodedlen;
		inbuf += decodedlen;
		if ( decodedlen < 0 )
		{
			// Nothin decoded - skip this frame
			return 1;
		}
		if ( outsizeAlign <= 0 )
		{
			// Decoded, but no data to play
			continue;
		}

		if ( resampleContext )
		{
			int samplesConsumed = 0;
			size_t outsizeResampler; // number of bytes for output resampler
			int16_t *outbufResampler; //podwojony bufor wejsciowy
			size_t inputSamplesSize;
			int samplesOutput = 0;
			if ( ostream )
			{
				// append this data to previous one
				goc_osWrite(ostream, outbufAlign, outsizeAlign);
				inputSamplesSize = goc_memOStreamSize( ostream ) >> 1;
				outsizeResampler = goc_memOStreamSize( ostream ) << 1;
				outbufResampler = malloc(outsizeResampler); //podwojony bufor wejsciowy
				samplesOutput = av_resample(
					resampleContext, // context
					outbufResampler, // output buffer
					goc_memOStreamGet( ostream ), // input buffer
					&samplesConsumed, // number of consumed input samples
					inputSamplesSize, // number of input samples
					outsizeResampler >> 1, // number of output samples buffer
					0 );
				// clear buffer stream
				goc_osClose(ostream);
				ostream = NULL;
			}
			else
			{
				outsizeResampler = outsizeAlign<<1;
				outbufResampler = malloc(outsizeResampler);
				inputSamplesSize = outsizeAlign>>1;
				samplesOutput = av_resample(
					resampleContext, // context
					outbufResampler, // output buffer
					outbufAlign, // input buffer
					&samplesConsumed, // number of consumed input samples
					inputSamplesSize, // number of input samples
					outsizeResampler >> 1, // number of output samples buffer
					0 );
			}
			// zapisanie do bufora sampli, ktore zostaly do zresamplowania
			if ( samplesConsumed < (outsizeAlign >> 1) )
			{
				if ( ostream == NULL )
					ostream = goc_memOStreamOpen();
				goc_osWrite(ostream, outbufAlign + samplesConsumed, outsizeAlign-(samplesConsumed<<1));
			}
			// GOC_ERROR("Resample operation");
			// GOC_BERROR("Samples Consumed %d, samples Output %d, input samples %d",
			//	samplesConsumed, samplesOutput, inputSamplesSize);
			ao_play(aodev, (void*)outbufResampler, samplesOutput << 1);
			free(outbufResampler);
			// av_resample_close(resampleContext);
		}
		else
			ao_play(aodev, (void*)outbufAlign, outsizeAlign);
	}
	if ( avpacket.data != NULL )
		av_free_packet( &avpacket );
	return 1;
}
// ------------------ LIBAVCODEC LIBAVFORMAT END ------------------------ //

#define NUMBER_OF_TRIES 10
#define TRY_OPEN(nextfun) \
{ \
	int i=0; \
	while ( openAudioFile( nextfun() ) && (i++ < NUMBER_OF_TRIES) ); \
}

void *playFile(void *arg)
{
	int old = MODE_PLAY;

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
	int i;
	int min = -1;
	if (mixer_fd == -1)
		return;
	for (i=0; i<SOUND_MIXER_NRDEVICES; i++)
	{
		if ((1 << i) & devmask)
		{
			if ( current_dev == -1 )
			{
				current_dev = i;
				break;
			}
			if ( min == -1 )
				min = i;
			if (i == current_dev)
				current_dev = -1;
		}
	}
	if ( current_dev == -1 )
		current_dev = min;
	SetCurrentDev(dev_label[current_dev]);
//		printf("Set actual mixer port to [ %s ]\n", dev_label[current_dev]);
}

int doAction(unsigned int action, void *param)
{
	int old;
	char buf[128];
	switch ( action )
	{
		case ACTION_AUMIX:
			switch ( *(int*)param )
			{
				case AUMIX_PLUS: // INCREASE
				{
					int pos;
					if (mixer_fd == -1)
						break;
					ReadLevel(current_dev, &pos);
					pos &= 0x7F;
					pos++;
					AdjustLevel(current_dev, 0, pos);
					break;
				}
				case AUMIX_MINUS: // DECREASE
				{
					int pos;
					if (mixer_fd == -1)
						break;
					ReadLevel(current_dev, &pos);
					pos &= 0x7F;
					pos--;
					AdjustLevel(current_dev, 0, pos);
					break;
				}
				case AUMIX_NEXTDEV:
					if (mixer_fd == -1)
						break;
					next_aumix_port();
					break;
				case AUMIX_PREVDEV:
					break;
				case AUMIX_CURRDEV:
					if (mixer_fd == -1)
						break;
					if ( current_dev == -1 )
						*(int*)param = (int)"";
					ReadLevel(current_dev, (int*)param);
					break;
				case AUMIX_CURRNAME:
					if ( current_dev != -1 )
						*(int*)param = (int)dev_label[current_dev];
					else
						*(int*)param = (int)"";
					break;
			}
			return 0;
		case ACTION_NEXT:
			old = mode;
			// zatrzymaj granie
			if ( mode == MODE_PLAY )
			{
				mode = MODE_STOP;
				pthread_mutex_lock(&mutexPlay);
			}
			// zmieñ piosenkê
			TRY_OPEN( mod_playlistNext );
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
			TRY_OPEN( mod_playlistPrev );
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
					TRY_OPEN( mod_playlistNext );
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
			if ( param != NULL )
				openAudioFile((const char*)param); //TODO: -1
			else
				openAudioFile(filename); //TODO: -1
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
			TRY_OPEN( mod_playlistNext );
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
					TRY_OPEN(mod_playlistNext);
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
			switch ( *(int*)param )
			{
				case INFO_FILE:
					*(int*)param = (int)strdup(filename);
					break;
				case INFO_TITLE:
					*(int*)param = (int)(strdup(pFormatCtx->title));
					break;
				case INFO_ARTIST:
					*(int*)param = (int)(strdup(pFormatCtx->author));
					break;
				case INFO_ALBUM:
					*(int*)param = (int)(strdup(pFormatCtx->album));
					break;
				case INFO_YEAR:
					sprintf(buf, "%d", pFormatCtx->year);
					*(int*)param = (int)(strdup(buf));
					break;
				case INFO_COMMENT:
					*(int*)param = (int)(strdup(pFormatCtx->comment));
					break;
				default:
					*(int*)param = (int)strdup("NONE");
					GOC_ERROR("Nieznany parametr informacji ACTION_INFO");
					return -1;
			}
			return 0;
		case ACTION_SHUFFLE:
			switch ( *(int*)param )
			{
				case SHUFFLE_YES:
					mod_playlistShuffleMode(SHUFFLE_RANDOM);
					break;
				case SHUFFLE_NO:
					mod_playlistShuffleMode(0);
					break;
				case SHUFFLE_REINIT:
					// TODO // SHUFFLE_REINIT
					break;
			}
			return 0;
		case ACTION_PLAYLIST_ADDQUEUE:
			if ( mod_playlistQueue )
				mod_playlistQueue( *(int*)param );
			return 0;
		case ACTION_PLAYLIST_REMQUEUE:
			if ( mod_playlistRemQueue )
				mod_playlistRemQueue( *(int*)param );
			return 0;
		case ACTION_PLAYLIST_ISQUEUE:
			if ( mod_playlistIsQueue )
				(*(int*)param) = mod_playlistIsQueue(
					*(int*)param );
			return 0;
		case ACTION_PLAYLIST_ADDFILE:
			if ( mod_playlistAddFile )
				mod_playlistAddFile( param );
			return 0;
		case ACTION_PLAYLIST_ADDLIST:
			if ( mod_playlistAddList )
				mod_playlistAddList( param );
			return 0;
		case ACTION_PLAYLIST_CLEAR:
			if ( mod_playlistClear )
				mod_playlistClear();
			return 0;
		case ACTION_PLAYLIST_REMOVEFILE:
			if ( mod_playlistRemoveFile )
				mod_playlistRemoveFile( *((int *)param) );
			return 0;
		case ACTION_PLAYLIST_GETSIZE:
			if ( mod_playlistGetSize )
			{
				*((int *)param) = mod_playlistGetSize();
			}
			return 0;
		case ACTION_PLAYLIST_GETFILE:
			if ( mod_playlistGetFile )
				*((int *)param) = (int)mod_playlistGetFile(
					*((int *)param));
			return 0;
		case ACTION_PLAYLIST_GETTABLE:
			if ( mod_playlistGetTable )
				*((int *)param) = (int)mod_playlistGetTable();
			return 0;
		case ACTION_PLAYLIST_GETACTUAL:
			if ( mod_playlistGetActualPos )
				*((int *)param) = mod_playlistGetActualPos();
			return 0;
		case ACTION_PLAYLIST_STORE:
			if ( mod_playlistStoreInFile )
				mod_playlistStoreInFile( (const char *)param );
			return 0;
		default:
			return ERR_UNKNOWN_ACTION;
	}
}

int main(int argc, char *argv[])
{
//	char *modoutputname = NULL;
	char *modplaylistname = NULL;
	char *modcontrolname = NULL;
	unsigned char isLoadFromCmd = 0;
	/* Inicjowanie mixera */
	// ---------------------------------------------------------------
	if (InitializeMixer("/dev/mixer"))
	{
		GOC_ERROR("Inicjowanie urz±dzenia 'mixer' zakoñczone niepowodzeniem");
		endProgram(1);
	}
	if (mixer_fd != -1 && devmask)
	{
		int t = 0;
		while (!((1<<t) & devmask)) t++;
		current_dev = t;
	}
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
//			if ( strcmp("output", pBuf) == 0 )
//				modoutputname = goc_stringCopy(modoutputname, val);
			if ( strcmp("playlist", pBuf) == 0 )
				modplaylistname = goc_stringCopy(modplaylistname, val);
			else if ( strcmp("control", pBuf) == 0 )
				modcontrolname = goc_stringCopy(modcontrolname, val);
			else if ( strncmp("aumix:", pBuf, strlen("aumix:"))==0 )
			{
				if ( SetCurrentDev(pBuf+strlen("aumix:"))!=-1 )
					AdjustLevel(current_dev, 0, atoi(val));
				else
					GOC_ERROR("Wskazano z³e urz±dzenie aumix");
			}
		}
		fclose(plik);
	}
	if ( modcontrolname == NULL )
	{
		GOC_ERROR("Brak w ustawieniach modu³u [control]");
		endProgram(1);
	}
//	if ( modoutputname == NULL )
//	{
//		printf("Brak w ustawieniach modu³u [output]\n");
//		endProgram(1);
//	}
	if ( modplaylistname == NULL )
	{
		GOC_ERROR("Brak w ustawieniach modu³u [playlist]");
		endProgram(1);
	}
	/* Inicjowanie modu³u wyj¶cia */
	// ---------------------------------------------------------------
	/* USE_AO
	hanOutput = 0;
	hanOutput = dlopen(modoutputname, RTLD_LAZY);
	if ( !hanOutput )
	{
		fprintf(stderr, "%s\n", dlerror());
		fprintf(stderr, "Nieudane za³adowanie modu³u wyj¶cia. Line: %s:%d\n",
				__FILE__, __LINE__);
		endProgram(1);
	}
	mod_outputInit = dlsym(hanOutput, "outputInit");
	mod_outputCleanup = dlsym(hanOutput, "outputCleanup");
	mod_outputPlaySample = dlsym(hanOutput, "outputPlaySample");
	mod_outputSetParameter = dlsym(hanOutput, "outputSetParameter");
	mod_outputGetParameter = dlsym(hanOutput, "outputGetParameter");
	if ( !mod_outputInit || !mod_outputCleanup || !mod_outputPlaySample
		|| !mod_outputSetParameter || !mod_outputGetParameter )
	{
		fprintf(stderr, "%s\n", dlerror());
		fprintf(stderr, "Nieudane za³adowanie modu³u playlisty.\n%s:%d\n",
				__FILE__, __LINE__);
		endProgram(1);
	}
	*/
	/* Wstepne inicjowanie modulu playlist */
	// ---------------------------------------------------------------
	hanPlaylist = 0;
	hanPlaylist = dlopen(modplaylistname, RTLD_LAZY);
	if ( !hanPlaylist )
	{
		GOC_BERROR("Nieudane ³adowanie modu³u playlist: [%s]", dlerror());
		endProgram(1);
	}
	mod_playlistNext = dlsym(hanPlaylist, "playlistNext");
	mod_playlistPrev = dlsym(hanPlaylist, "playlistPrev");
	mod_playlistShuffleMode = dlsym(hanPlaylist, "playlistShuffleMode");
	mod_playlistInit = dlsym(hanPlaylist, "playlistInit");
	mod_playlistCleanup = dlsym(hanPlaylist, "playlistCleanup");
	mod_playlistAddFile = dlsym(hanPlaylist, "playlistAddFile");
	mod_playlistAddList = dlsym(hanPlaylist, "playlistAddList");
	mod_playlistClear = dlsym(hanPlaylist, "playlistClear");
	mod_playlistGetSize = dlsym(hanPlaylist, "playlistGetSize");
	mod_playlistRemoveFile = dlsym(hanPlaylist, "playlistRemoveFile");
	mod_playlistGetFile = dlsym(hanPlaylist, "playlistGetFile");
	mod_playlistGetTable = dlsym(hanPlaylist, "playlistGetTable");
	mod_playlistGetActualPos = dlsym(hanPlaylist, "playlistGetActualPos");
	mod_playlistQueue = dlsym(hanPlaylist, "playlistQueue");
	mod_playlistRemQueue = dlsym(hanPlaylist, "playlistRemQueue");
	mod_playlistIsQueue = dlsym(hanPlaylist, "playlistIsQueue");
	mod_playlistStoreInFile = dlsym(hanPlaylist, "playlistStoreInFile");
	if ( !mod_playlistNext || !mod_playlistPrev
		|| !mod_playlistShuffleMode || !mod_playlistInit
		|| !mod_playlistCleanup || !mod_playlistAddList
		|| !mod_playlistAddFile || !mod_playlistClear
		|| !mod_playlistGetSize || !mod_playlistRemoveFile
		|| !mod_playlistGetFile || !mod_playlistGetTable
		|| !mod_playlistGetActualPos || !mod_playlistQueue
		|| !mod_playlistRemQueue || !mod_playlistIsQueue
		|| !mod_playlistStoreInFile )
	{
		GOC_BERROR("Nieudane ³adowanie modu³u playlist: [%s]", dlerror());
		endProgram(1);
	}
	if ( mod_playlistInit() )
	{
		GOC_ERROR("Inicjowanie modu³u playlist zakoñczone b³êdem");
		endProgram(1);
	}
	/* Inicjowanie modu³u kontrolera */
	// ---------------------------------------------------------------
	hanControl = 0;
	hanControl = dlopen(modcontrolname, RTLD_LAZY);
	if ( !hanControl )
	{
		GOC_BERROR("Nieudane ³adowanie modu³u control: [%s]", dlerror());
		endProgram(1);
	}
	mod_controlInit = dlsym(hanControl, "controlInit");
	mod_controlCleanup = dlsym(hanControl, "controlCleanup");
	mod_controlStart = dlsym(hanControl, "controlStart");
	mod_controlEvent = dlsym(hanControl, "controlEvent");
	if ( !mod_controlInit || !mod_controlCleanup || !mod_controlStart
			|| !mod_controlEvent )
	{
		GOC_BERROR("Nieudane ³adowanie modu³u control: [%s]", dlerror());
		endProgram(1);
	}
	if ( mod_controlInit( &doAction ) )
	{
		GOC_ERROR("Inicjowanie modu³u control zakoñczone b³êdem");
		endProgram(1);
	}
//	if ( modoutputname )
//		free(modoutputname);
	if ( modcontrolname )
		free(modcontrolname);
	if ( modplaylistname )
		free(modplaylistname);

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
						mod_playlistAddList( argv[i]+2 );
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
				mod_playlistAddFile( argv[i] );
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
			mod_playlistAddList( konfname );
		}
		konfname = goc_stringFree( konfname );
	}

	// zarejestruj AV TODO: Wyselekcjonowac interesujace formaty
	avcodec_init();
	avcodec_register_all();
	av_register_all();
	av_log_set_level(AV_LOG_QUIET);
	memset( &avpacket, 0, sizeof(AVPacket) );
	// av_log_set_level(AV_LOG_DEBUG);
	
	// zainicjowanie AO
	ao_initialize();
	aodriverid = ao_default_driver_id();
	if ( aodriverid == -1 )
		endProgram(-1);
	// zarezerwowanie buforów do funkcjonowania programu
	outsize = AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(int16_t);
	outbuf = malloc(outsize);
	outbufAlign = align16(outbuf, &outsize);
	// rozpoczecie grania
	mod_controlStart();
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
		mod_playlistStoreInFile( konfname );
		konfname = goc_stringFree( konfname );
	}
	endProgram(0);
}
