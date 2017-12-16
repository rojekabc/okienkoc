#include "decoder.h"

#include <sox.h>
#define GOC_PRINTINFO
#define GOC_PRINTERROR
#define GOC_PRINTDEBUG
#include <tools/log.h>

sox_format_t* input = NULL;
sox_sample_t* buffer = NULL;
size_t size = 0;
size_t clips = 0; SOX_SAMPLE_LOCALS;

DecoderCode decoderInitialize() {
	return DECODER_CODE_OK;
}

DecoderCode decoderOpen(const char* filename, size_t bufferSize) {
	input = sox_open_read(filename, NULL, NULL, NULL);
	if ( !input ) {
		GOC_ERROR("Cannot open for read");
		return DECODER_CODE_FAIL;
	}
	if ( input->signal.rate != 44100 ) {
		GOC_ERROR("Wrong rate");
		return DECODER_CODE_FAIL;
	}
	if ( input->signal.channels != 2 ) {
		GOC_ERROR("Wrong channels");
		return DECODER_CODE_FAIL;
	}
	buffer = malloc(bufferSize*sizeof(sox_sample_t));
	size = bufferSize;
	return DECODER_CODE_OK;
}

DecoderCode decoderRead(int16_t* pBuf, size_t* nBuf) {
	*nBuf = sox_read(input, buffer, size);
	if ( *nBuf > 0 ) {
		for (size_t i=0; i<*nBuf; i++) {
			pBuf[i] = SOX_SAMPLE_TO_SIGNED(16, buffer[i], clips);
		}
	}
	return DECODER_CODE_OK;
}

DecoderCode decoderClose() {
	if ( input ) {
		sox_close(input);
		input = NULL;
		clips = 0;
		free(buffer);
		buffer = NULL;
		size = 0;
	}
	return DECODER_CODE_OK;
}

DecoderCode decoderQuit() {
	sox_quit();
	return DECODER_CODE_OK;
}
