#ifndef _GOC_CONFFILE_H_
#define _GOC_CONFFILE_H_

#include "fobclass.h"

int writeObjectToFile(const char *filename, GOC_OBJECTHANDLER object);
int listObjectsInFile(
	const char *filename,
	int (*callFun)(GOC_OBJECTHANDLER object, void *arg), void *arg);
int removeObjectsInFile(
	const char *filename, GOC_OBJECTHANDLER filter);
int deserializeObjectsInFile(
	const char *filename,
	int (*callFun)(const char *classname, void *node, void *arg),
	void *arg);

#endif // ifndef _GOC_CONFFILE_H_
