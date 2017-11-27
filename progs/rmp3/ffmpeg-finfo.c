#include "finfo.h"

#include <tools/mystr.h>

#include <libavformat/avformat.h>
#if LIBAVFORMAT_VERSION_MAJOR > 56
#	include <libavutil/opt.h>
#endif

AVFormatContext* pFormatCtx = NULL;
int initialized = 0;

static int avCloseFile(AVFormatContext** pFormatCtx) {
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

static int avOpenFile(const char* filename, AVFormatContext** pFormatCtx) {
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

static const char* avGetDict(const char* name) {
	AVDictionaryEntry* el = av_dict_get(pFormatCtx->metadata, name, NULL, 0);
	if ( el == NULL ) {
		return NULL;
	}
	return el->value;
}

FinfoCode finfoInitialize() {
	if (!initialized) {
		av_register_all();
	}
	return FINFO_CODE_OK;
}

FinfoCode finfoInfo(const char* filename, struct FileInfo* fileInfo) {
	// TODO: Checking return codes
	if ( avOpenFile(filename, &pFormatCtx) != 0 ) {
		return FINFO_CODE_FAIL;
	}
	fileInfo->title = goc_stringCopy(fileInfo->title, avGetDict("title"));
	fileInfo->artist = goc_stringCopy(fileInfo->artist, avGetDict("artist"));
	fileInfo->album = goc_stringCopy(fileInfo->album, avGetDict("album"));
	fileInfo->year = goc_stringCopy(fileInfo->year, avGetDict("year"));
	fileInfo->comment = goc_stringCopy(fileInfo->comment, avGetDict("comment"));
	avCloseFile(&pFormatCtx);
	return FINFO_CODE_OK;
}
