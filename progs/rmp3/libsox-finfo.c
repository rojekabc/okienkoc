#include "finfo.h"

#include <tools/mystr.h>
#ifdef _DEBUG_
#	define GOC_PRINTDEBUG
#endif
#define GOC_PRINTERROR
#include <tools/log.h>
#include <sox.h>

FinfoCode finfoInitialize() {
	return FINFO_CODE_OK;
}

FinfoCode finfoInfo(const char* filename, struct FileInfo* fileInfo) {
	GOC_DEBUG("finfo read info");
	sox_format_t* input = NULL;

	input = sox_open_read(filename, NULL, NULL, NULL);
	if ( !input ) {
		return FINFO_CODE_FAIL;
	}

	fileInfo->filename = goc_stringCopy(fileInfo->filename, filename);
	fileInfo->title = goc_stringCopy(fileInfo->title, sox_find_comment(input->oob.comments, "Title"));
	fileInfo->artist = goc_stringCopy(fileInfo->artist, sox_find_comment(input->oob.comments, "Artist"));
	fileInfo->album = goc_stringCopy(fileInfo->album, sox_find_comment(input->oob.comments, "Album"));
	fileInfo->year = goc_stringCopy(fileInfo->year, sox_find_comment(input->oob.comments, "Year"));
	fileInfo->comment = goc_stringCopy(fileInfo->comment, sox_find_comment(input->oob.comments, "Comment"));

	sox_close(input);

	return FINFO_CODE_OK;
}
