#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

/**
 * Open file and read stream info about file.
 *
 * @param filename give a filename to open
 * @param pFormatCtx give a pointer to format context pointer
 */
int avOpenFile(const char* filename, AVFormatContext** pFormatCtx);

/**
 * Close opened file context.
 *
 * @param pFormatCtx pointer to format context pointer
 */
int avCloseFile(AVFormatContext** pFormatCtx);

/**
 * Return 1 if stream is audio type.
 */
int avIsAudio(AVStream* pAVStream);

/**
 * It'll close resampler context, if it's opened.
 */
int avCloseResampler();

/**
 * Open and initialize resampler for given input codec context.
 * Output to 48000 rate.
 */
#if LIBAVCODEC_VERSION_MAJOR > 56
int avOpenResampler(AVCodecParameters* pCodecContext);
#else
int avOpenResampler(AVCodecContext* pCodecContext);
#endif
