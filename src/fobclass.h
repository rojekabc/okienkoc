#ifndef _GOC_FOBCLASS_H_
#define _GOC_FOBCLASS_H_

#include "global.h"
#include "tablica.h"
// definicje kompilatora
#ifdef _DEBUG_
#	define GOC_RET_OK {GOC_FUNCTIONRETURN ret = {GOC_ERR_OK, {0}, __FILE__, __LINE__}; return ret;}
#	define GOC_RET_FALSE(desc) {GOC_FUNCTIONRETURN ret = {GOC_ERR_FALSE, {.description = desc}, __FILE__, __LINE__}; return ret;}
#	define GOC_RET_RESULT(res) {GOC_FUNCTIONRETURN ret = {GOC_ERR_OK, {.result = (int)res}, __FILE__, __LINE__}; return ret;}
#	define GOC_RET_PARENT {GOC_FUNCTIONRETURN ret = {GOC_ERR_PARENT, {0}, __FILE__, __LINE__}; return ret;}
#	define GOC_RET_NEXT {GOC_FUNCTIONRETURN ret = {GOC_ERR_NEXT, {0}, __FILE__, __LINE__}; return ret;}
#else
#	define GOC_RET_OK {GOC_FUNCTIONRETURN ret = {GOC_ERR_OK, {0}}; return ret;}
#	define GOC_RET_FALSE(desc) {GOC_FUNCTIONRETURN ret = {GOC_ERR_FALSE, {.description = desc}}; return ret;}
#	define GOC_RET_RESULT(res) {GOC_FUNCTIONRETURN ret = {GOC_ERR_OK, {.result = (int)res}}; return ret;}
#	define GOC_RET_PARENT {GOC_FUNCTIONRETURN ret = {GOC_ERR_PARENT, {0}}; return ret;}
#	define GOC_RET_NEXT {GOC_FUNCTIONRETURN ret = {GOC_ERR_NEXT, {0}}; return ret;}
#endif // ifdef _DEBUG_

// stale - napisy - powod bledu
extern const char *TEXT_NOMEMORY;
extern const char *TEXT_NOCLASSFOUND;
extern const char *TEXT_CLASSEXISTS;
extern const char *TEXT_NOFUNCTIONFOUND;
extern const char *TEXT_NOLIBFOUND;
extern const char *TEXT_NOLIBLOAD;
extern const char *TEXT_NOFUNLOAD;
extern const char *TEXT_NOOBJECTFOUND;
extern const char *TEXT_OBJECTEXISTS;
extern const char *TEXT_NOTALLOWED;
extern const char *TEXT_WRONGARG;

// definicje typow
typedef void* GOC_OBJECTHANDLER;
typedef struct GOC_FUNCTIONRETURN
{
	int code;
	union
	{
		const char *description;
		unsigned int result;
	};
#ifdef _DEBUG_
	const char *file;
	int line;
#endif // ifdef _DEBUG
} GOC_FUNCTIONRETURN;

typedef GOC_FUNCTIONRETURN (GOC_FUNCTIONDEFINE)(GOC_OBJECTHANDLER, void*, unsigned int);

typedef struct GOC_OBJECTFUNCTION
{
	char *name;
	GOC_FUNCTIONDEFINE *function;
} GOC_OBJECTFUNCTION;

typedef struct GOC_OBJECTCLASS
{
	char *name;
	struct GOC_OBJECTCLASS *parent;
	struct GOC_OBJECTCLASS **interface;
	_GOC_TABEL_SIZETYPE_ nInterface;
	GOC_OBJECTFUNCTION **function;
	_GOC_TABEL_SIZETYPE_ nFunction;
} GOC_OBJECTCLASS;

// definicje funkcji
GOC_FUNCTIONRETURN registerClass(const char *name);
GOC_FUNCTIONRETURN unregisterClass(const char *name);
GOC_FUNCTIONRETURN hasClass(const char *name);
GOC_FUNCTIONRETURN registerClassParented(
	const char *classname, const char *parentclassname);
GOC_FUNCTIONRETURN registerFunction(const char *name, GOC_FUNCTIONDEFINE *fun);
GOC_FUNCTIONRETURN registerClassFunction(
	const char *classname, const char *name, GOC_FUNCTIONDEFINE *fun);
GOC_FUNCTIONRETURN unregisterFunction(const char *name);
GOC_FUNCTIONRETURN unregisterClassFunction(const char *classname, const char *name);
GOC_FUNCTIONRETURN registerClassExtFunction(const char *classname, const char *name, const char *funName, const char *libname);
GOC_FUNCTIONRETURN registerExtFunction(const char *name, const char *funName, const char *libname);
GOC_FUNCTIONRETURN registerClassObject(const char *classname, GOC_OBJECTHANDLER handler, unsigned char flag);
GOC_FUNCTIONRETURN registerObject(GOC_OBJECTHANDLER handler);
GOC_FUNCTIONRETURN unregisterObject(GOC_OBJECTHANDLER handler);
GOC_FUNCTIONRETURN allocObject(
	const char *classname, void *pBuf, unsigned int nBuf);
GOC_FUNCTIONRETURN allocExternal(
	const char *classname, void *ptr);
GOC_FUNCTIONRETURN freeObject(GOC_OBJECTHANDLER handler);
GOC_FUNCTIONRETURN reallocObject(GOC_OBJECTHANDLER ptrFrom, GOC_OBJECTHANDLER ptrTo);
GOC_FUNCTIONRETURN reallocTableOfObject(GOC_OBJECTHANDLER firstOldElem, GOC_OBJECTHANDLER firstNewElem, int n);
#define call(handler, funname, pBuf, uBuf) __call(handler, funname, (void*)pBuf, (unsigned int)uBuf)
GOC_FUNCTIONRETURN __call(GOC_OBJECTHANDLER handler, const char *functionname, void *pBuf, unsigned int nBuf);
#define callStatic(classname, funname, pBuf, uBuf) __callStatic(classname, funname, (void*)pBuf, (unsigned int)uBuf)
GOC_FUNCTIONRETURN __callStatic(const char *classname, const char *functionname, void *pBuf, unsigned int nBuf);
GOC_FUNCTIONRETURN callParent(
	GOC_OBJECTHANDLER handler, const char *name, void *pBuf, unsigned int nBuf);
GOC_FUNCTIONRETURN loadLibrary(const char *libname);
GOC_FUNCTIONRETURN getObjectClass(GOC_OBJECTHANDLER handler);
#endif // ifndef _GOC_FOBCLASS_H_
