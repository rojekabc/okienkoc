#ifndef _DECORER_H_
#	define _DECODER_H_
#	include <stdlib.h>

typedef enum DecoderCode {
	DECODER_CODE_OK,
	DECODER_CODE_FAIL
} DecoderCode;

DecoderCode decoderInitialize();
DecoderCode decoderOpen(const char* filename, size_t bufferSize);
DecoderCode decoderRead(int16_t* pBuf, size_t* nBuf);
DecoderCode decoderClose();
DecoderCode decoderQuit();

#endif
