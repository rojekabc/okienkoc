#ifndef _GOC_FILEOBJECT_H_
#define _GOC_FILEOBJECT_H_

#include <stdio.h>
#include "fobclass.h"

extern const char *CN_FILEOBJECT;
extern const char *CN_STRING;
extern const char *CN_TABLE;
extern const char *CN_INTEGER;
extern const char *CN_OBJECTTABLE;

typedef struct GOC_FILEOBJECT
{
//	char *name;
	char **attrName;
	GOC_OBJECTHANDLER *attrValue;
	int attrAmmount;
} GOC_FILEOBJECT;

void fileObjectInit();

GOC_FILEOBJECT *fileObjectCreate();
GOC_FILEOBJECT *fileObjectSetAttribute(
	GOC_FILEOBJECT *object, const char *attrName, const char *attrValue);
GOC_FILEOBJECT *fileObjectRemoveAttribute(
	GOC_FILEOBJECT *object, const char *attrName);
const char *fileObjectGetAttribute(
	GOC_FILEOBJECT *object, const char *attrName);
GOC_FILEOBJECT *fileObjectDestroy(GOC_FILEOBJECT *object);
int fileObjectGetAttributePos(
	GOC_FILEOBJECT *object, const char *attrName);
void fileObjectSerialize(GOC_FILEOBJECT *object, FILE *fileptr);
int fileObjectCount(GOC_FILEOBJECT *object);
const char *fileObjectGetAttributeNameAt(GOC_FILEOBJECT *object, int pos);
const char *fileObjectGetAttributeAt(GOC_FILEOBJECT *object, int pos);

int fobGetInt(GOC_FILEOBJECT *object, const char *attrName);
unsigned int fobGetUInt(GOC_FILEOBJECT *object, const char *attrName);
const char *fobGetStr(GOC_FILEOBJECT *object, const char *attrName);

GOC_FILEOBJECT* fobSetInt(GOC_FILEOBJECT *object, const char *attrName, int value);
GOC_FILEOBJECT* fobSetStr(GOC_FILEOBJECT *object, const char *attrName, const char *value);

#endif // ifndef _GOC_FILEOBJECT_H_
