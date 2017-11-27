#ifndef _FINFO_H_
#	define _FINFO_H_

typedef enum FinfoCode {
	FINFO_CODE_OK = 0,
	FINFO_CODE_FAIL
} FinfoCode;

typedef struct FileInfo {
	char* filename;
	char* title;
	char* artist;
	char* album;
	char* year;
	char* comment;
	char* genre;
} FileInfo;


FinfoCode finfoInitialize();
FinfoCode finfoInfo(const char* filename, struct FileInfo* finfo);

#endif
