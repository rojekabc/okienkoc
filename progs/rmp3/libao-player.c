#include "player.h"

#include <ao/ao.h>
#include <string.h>

int aodriverid = -1;
ao_device *aodev = NULL;

PlayerCode playerCloseOutput() {
	if ( aodev )
	{
		ao_close(aodev);
		aodev = NULL;
	}
	return PLAYER_CODE_OK;
}

PlayerCode playerInitialize() {
	ao_initialize();
	aodriverid = ao_default_driver_id();
	if ( aodriverid == -1 ) {
		return PLAYER_CODE_FAIL;
	}
	return PLAYER_CODE_OK;
}

PlayerCode playerOpenOutput() {
	// close opened device
	playerCloseOutput();

	// prepare sample format information
	ao_sample_format aosampleformat;
	memset( &aosampleformat, 0, sizeof(ao_sample_format) );
	aosampleformat.bits = 16;
	aosampleformat.rate = 44100;
	aosampleformat.channels = 2;
	aosampleformat.byte_format = AO_FMT_LITTLE;

	// TODO: Mozliwosc wyboru plik a urzadzenie
	aodev = ao_open_live(aodriverid, &aosampleformat, NULL);
	if ( !aodev ) {
		return PLAYER_CODE_FAIL;
	}
	return PLAYER_CODE_OK;
}

PlayerCode playerPlay(int16_t* pBuf, size_t nBuf) {
	if ( ao_play(aodev, (void*)pBuf, nBuf*sizeof(int16_t)) ) {
		return PLAYER_CODE_OK;
	} else {
		playerCloseOutput();
		return PLAYER_CODE_FAIL;
	}
}
