#include "player.h"

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <string.h>
#define GOC_PRINTERROR
#ifdef _DEBUG_
#	define GOC_PRINTDEBUG
#endif
#include <tools/log.h>

#define PCM_DEVICE "default"
#define SAMPLE_RATE 44100
#define SAMPLE_CHANNELS 2
// sample - Signed 16 bit, Little Endian

snd_pcm_t* handler = NULL;
snd_pcm_hw_params_t* params = NULL;
int bufsize;
snd_pcm_uframes_t frames;

PlayerCode playerCloseOutput() {
	GOC_DEBUG("-> playerCloseOutput");
	if ( handler ) {
		// Marked because of playling whole file before close
		// snd_pcm_drain( handler );
		snd_pcm_close( handler );
		handler = NULL;
	}
	/* Marked because of failing
	if ( params ) {
		snd_pcm_hw_params_free( params );
		GOC_DEBUG("free");
		params = NULL;
	}
	*/
	GOC_DEBUG("<- playerCloseOutput");
	return PLAYER_CODE_OK;
}

PlayerCode playerInitialize() {
	return PLAYER_CODE_OK;
}

PlayerCode playerOpenOutput() {
	unsigned int rate = SAMPLE_RATE;
	playerCloseOutput();

	// open device
	GOC_CHEOP(snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) >= 0, return PLAYER_CODE_FAIL);
	// set up parameters
	snd_pcm_hw_params_alloca( &params );
	snd_pcm_hw_params_any( handler, params );
	GOC_CHEOP(snd_pcm_hw_params_set_access( handler, params, SND_PCM_ACCESS_RW_INTERLEAVED ) >= 0, return PLAYER_CODE_FAIL);
	GOC_CHEOP(snd_pcm_hw_params_set_format( handler, params, SND_PCM_FORMAT_S16_LE ) >= 0, return PLAYER_CODE_FAIL);
	GOC_CHEOP(snd_pcm_hw_params_set_channels( handler, params, SAMPLE_CHANNELS ) >= 0, return PLAYER_CODE_FAIL);
	GOC_CHEOP(snd_pcm_hw_params_set_rate_near( handler, params, &rate, NULL ) >= 0, return PLAYER_CODE_FAIL);
	// send parameters to device
	GOC_CHEOP(snd_pcm_hw_params( handler, params ) >=0, return PLAYER_CODE_FAIL);
	// to get device name
	// snd_pcm_name
	// to get device state
	// snd_pcm_state
	// to get device state name
	// snd_pcm_state_name
	// to get device channels
	// snd_pcm_hw_params_get_channels
	// to get device sample rate
	// snd_pcm_hw_params_get_rate
	snd_pcm_hw_params_get_period_size(params, &frames, 0);
	bufsize = frames * SAMPLE_CHANNELS; /*  *sample-size (2, which means 2 bytes = 16bits) */

	return PLAYER_CODE_OK;
}

PlayerCode playerPlay(int16_t* pBuf, size_t nBuf) {
	int err;
	snd_pcm_uframes_t framesToPlay;
	// TO THINK buffsize and frames
	while ( nBuf > 0 ) {
		if ( nBuf > bufsize ) {
			framesToPlay = frames;
			nBuf -= bufsize;
		} else {
			framesToPlay = nBuf / SAMPLE_CHANNELS;
			nBuf = 0;
		}
		// liczba ramek. Jedna ramka to sample-size * sample-channels
		while ( (err = snd_pcm_writei( handler, pBuf, framesToPlay)) == -EPIPE ) {
			if ( snd_pcm_prepare( handler ) < 0 ) {
				GOC_ERROR("PCM PREPARE FAIL");
				break;
			}
		}
	       	if (err < 0 ) {
			GOC_ERROR("Cannot play");
			return PLAYER_CODE_FAIL;
		}
	}
	return PLAYER_CODE_OK;
}
