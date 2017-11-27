#include "avCommon.h"
#include <tools/mystr.h>

#if LIBAVFORMAT_VERSION_MAJOR > 56
#	include <libswresample/swresample.h>
#	include <libavutil/opt.h>
#endif

#if LIBAVFORMAT_VERSION_MAJOR > 56
struct SwrContext* resampleContext = NULL;
#else
struct AVResampleContext* resampleContext = NULL;
#endif

int avCloseFile(AVFormatContext** pFormatCtx) {
	if ( *pFormatCtx != NULL )
	{
#if LIBAVFORMAT_VERSION_MAJOR < 56
		av_close_input_file( *pFormatCtx );
		*pFormatCtx = NULL;
#else
		avformat_close_input( pFormatCtx );
#endif
	}
	return 0;
}

int avOpenFile(const char* filename, AVFormatContext** pFormatCtx) {
	int err;
#if LIBAVFORMAT_VERSION_MAJOR > 56
	char* url = NULL;
	url = goc_stringMultiAdd(NULL, "file:", filename);
	err = avformat_open_input( pFormatCtx, url, NULL, NULL);
	url = goc_stringFree(url);
#else
	err = av_open_input_file( pFormatCtx, filename, NULL, 0, NULL);
#endif
	if (err != 0) {
		// return logAVError( err );
		return err;
	}
#if LIBAVFORMAT_VERSION_MAJOR > 56
	err = avformat_find_stream_info( *pFormatCtx, NULL );
#else
	err = av_find_stream_info( pFormatCtx );
#endif
	if ( err < 0 ) {
		avCloseFile(pFormatCtx);
		// return logAVError( err );
		return err;
	}
	return 0;
}

int avIsAudio(AVStream* pAVStream) {
#if LIBAVCODEC_VERSION_MAJOR > 56
	return pAVStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO;
#elif LIBAVCODEC_VERSION_MAJOR > 52
	return pAVStream->codec->codec_type == AVMEDIA_TYPE_AUDIO;
#else
	return pAVStream->codec->codec_type == CODEC_TYPE_AUDIO;
#endif
}

#if LIBAVCODEC_VERSION_MAJOR > 56
int avOpenResampler(AVCodecParameters* pCodecContext) {
#else
int avOpenResampler(AVCodecContext* pCodecContext) {
#endif

#if LIBAVCODEC_VERSION_MAJOR > 56
	int err;
	resampleContext = swr_alloc();

	av_opt_set_int(resampleContext, "in_sample_rate", pCodecContext->sample_rate, 0);
	av_opt_set_int(resampleContext, "in_channel_layout", pCodecContext->channel_layout, 0);
	av_opt_set_int(resampleContext, "in_sample_fmt", pCodecContext->format, 0);

	av_opt_set_int(resampleContext, "out_sample_rate", 48000, 0);
	av_opt_set_int(resampleContext, "out_channel_layout", pCodecContext->channel_layout, 0);
	av_opt_set_int(resampleContext, "out_sample_fmt", pCodecContext->format, 0);

	err = swr_init( resampleContext );
	if ( err < 0 ) {
		return err;
	}
#else
	resampleContext = av_resample_init(
		48000, // rate out
		pCodecContext->sample_rate, // rate in
		16, // filter length
		10, // phase count
		0, // linear FIR filter
		0.7); // cutoff frequency
#endif
	return 0;
}

int avCloseResampler() {
#if LIBAVCODEC_VERSION_MAJOR > 56
	if ( resampleContext ) {
		swr_free( &resampleContext );
	}
#else
	if ( resampleContext ) {
		av_resample_close(resampleContext);
		//free(outbufResampler);
		resampleContext = NULL;
	}
#endif
	return 0;
}
