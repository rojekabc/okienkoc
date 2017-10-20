#ifndef _SLISTA_H
#define _GOC_SELLIST_H_

#include "lista.h"

/** File list element id. */
extern const char* GOC_ELEMENT_FILELIST;
/*
 * Check, that element at selected position and name is a directory.
 * IF position is -1 check using filename.
 * return GOC_ERR_OK if folder
 * return GOC_ERR_FALS if not a folder
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	char* pFilename;
	int position;
} GOC_StMsgFileListPositionFilename;
#define GOC_MSG_FILELISTISFOLDER(variable, _filename_, _position_) \
	GOC_MSG(GOC_StMsgFileListPositionFilename, variable, GOC_MSG_FILELISTISFOLDER_ID); \
	variable##Full.pFilename = _filename_; \
	variable##Full.position = _position_
extern const char* GOC_MSG_FILELISTISFOLDER_ID;

/*
 * Do query about append element to a list.
 * If flag is SFLISTAFLAG_KATALOG it means it'll be a foler/
 * return GOC_ERR_OK - append
 * GOC_ERR_FALS - don't append (for example sort of filter)
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	char* pFilename;
	int flag;
} GOC_StMsgFileListAddFile;
#define GOC_MSG_FILELISTADDFILE(variable, _filename_, _flag_) \
	GOC_MSG(GOC_StMsgFileListAddFile, variable, GOC_MSG_FILELISTADDFILE_ID); \
	variable##Full.pFilename = _filename_; \
	variable##Full.flag = _flag_
extern const char* GOC_MSG_FILELISTADDFILE_ID;

/*
 * List a folder.
 * Folder for element will be not changed.
 * Elements inside list will be removed and append one by one as a list files of folder.
 * If a file name is NULL, than use a folder, which is set for element.
 * return GOC_ERR_OK ok
 * return GOC_ERR_FALSE it's not a folder or not found in filesystem
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	char* pFilename;
} GOC_StMsgFileListFilename;
#define GOC_MSG_FILELISTLISTFOLDER(variable, _filename_) \
	GOC_MSG(GOC_StMsgFileListFilename, variable, GOC_MSG_FILELISTLISTFOLDER_ID); \
	variable##Full.pFilename = _filename_;
extern const char* GOC_MSG_FILELISTLISTFOLDER_ID;

/*
 * Set a folder for a list. Check, that current folder exists in filesystem.
 * It can be set as a relative or absolute path. If it's a relative it's counted
 * from current folder position of element.
 */
#define GOC_MSG_FILELISTSETFOLDER(variable, _filename_) \
	GOC_MSG(GOC_StMsgFileListFilename, variable, GOC_MSG_FILELISTSETFOLDER_ID); \
	variable##Full.pFilename = _filename_;
extern const char* GOC_MSG_FILELISTSETFOLDER_ID;

/*
 * Query that positioned filename is a folder redirecting to upper one.
 * If position is -1 use only filename.
 * return GOC_ERR_OK if it is a up folder.
 */
#define GOC_MSG_FILELISTISUPFOLDER(variable, _filename_, _position_) \
	GOC_MSG(GOC_StMsgFileListPositionFilename, variable, GOC_MSG_FILELISTISUPFOLDER_ID); \
	variable##Full.pFilename = _filename_; \
	variable##Full.position = _position_;
extern const char* GOC_MSG_FILELISTISUPFOLDER_ID;

/*
 * Query about name identifying current folder.
 * After going up, this name will be used to select this folder.
 * System will automatically free up given pointer.
 */
#define GOC_MSG_FILELISTGETFOLDERNAME(variable) \
	GOC_MSG(GOC_StMsgFileListFilename, variable, GOC_MSG_FILELISTGETFOLDERNAME_ID);
extern const char* GOC_MSG_FILELISTGETFOLDERNAME_ID;

#define GOC_STRUCT_FILELIST \
	GOC_STRUCT_LIST; \
	char *folder;

typedef struct GOC_StFileList
{
	GOC_STRUCT_FILELIST;
} GOC_StFileList;

int goc_filelistSetFolder(GOC_HANDLER uchwyt, const char *dirname);
const char *goc_filelistGetFolder(GOC_HANDLER uchwyt);
int goc_filelistListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);

#endif // ifndef _GOC_SELLIST_H_
