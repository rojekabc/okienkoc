#include "vtree.h"
#include "mystr.h"
#include <malloc.h>
#include <string.h>

#define VCHECK(ret) \
	if ( ! vtree ) \
		return ret;

GOC_VTree *goc_vtreeAlloc()
{
	GOC_VTree* vtree = malloc(sizeof(struct GOC_VTree));
	memset(vtree, 0, sizeof(struct GOC_VTree));
	return vtree;
}

GOC_VTree *goc_vtreeFree(GOC_VTree* vtree)
{
	int i = 0;
	VCHECK( NULL );
	vtree->name = goc_stringFree(vtree->name);
	vtree->value = goc_stringFree(vtree->value);
	for (; i<vtree->nNode; i++)
		goc_vtreeFree(vtree->pNode[i]);
	free(vtree);
	return NULL;
}

GOC_VTree *goc_vtreeCreate(const char *name, void *value)
{
	GOC_VTree* vtree = goc_vtreeAlloc();
	return goc_vtreeSet(vtree, name, value);
}

GOC_VTree *goc_vtreeSet(GOC_VTree* vtree, const char *name, void *value)
{
	VCHECK( NULL );
	vtree->name = goc_stringCopy(vtree->name, name);
	vtree->value = value;
	return vtree;
}

GOC_VTree *goc_vtreeAdd(GOC_VTree* vtree, const char *name, void *value)
{
	VCHECK( NULL );
	GOC_VTree* child = goc_vtreeCreate( name, value );
	vtree->pNode = goc_tableAdd(vtree->pNode, &vtree->nNode, sizeof(GOC_VTree*));
	vtree->pNode[ vtree->nNode-1 ] = child;
	return vtree;
}

const char *goc_vtreeGetName(const GOC_VTree* vtree)
{
	VCHECK( NULL );
	return vtree->name;
}

const char *goc_vtreeGetValue(const GOC_VTree* vtree)
{
	VCHECK( NULL );
	return vtree->value;
}

GOC_VTree *goc_vtreeGet(const GOC_VTree* vtree, const char* nodename)
{
	int i = 0;
	VCHECK( NULL );
	for (; i < vtree->nNode; i++)
	{
		if ( goc_stringEquals(vtree->pNode[i]->name, nodename) )
			return vtree->pNode[i];
	}
	return NULL;
}

GOC_VTree *goc_vtreeRemove(GOC_VTree* vtree, const char* nodename)
{
	int i = 0;
	VCHECK( NULL );
	for (; i < vtree->nNode; i++)
	{
		if ( goc_stringEquals(vtree->pNode[i]->name, nodename) )
		{
			goc_vtreeFree(vtree->pNode[i]);
			vtree->pNode = goc_tableRemove( vtree->pNode, &vtree->nNode, sizeof(GOC_VTree*), i );
			return vtree;
		}
	}
	return vtree;
}

GOC_VTree *goc_vtreeRemoveNode(GOC_VTree* vtree, GOC_VTree* node)
{
	int i = 0;
	VCHECK( NULL );
	for (; i < vtree->nNode; i++)
	{
		if ( vtree->pNode[i] == node )
		{
			goc_vtreeFree(vtree->pNode[i]);
			vtree->pNode = goc_tableRemove( vtree->pNode, &vtree->nNode, sizeof(GOC_VTree*), i );
			return vtree;
		}
	}
	return vtree;
}

GOC_VTree *goc_vtreeAttach(GOC_VTree* vtree, GOC_VTree* node)
{
	VCHECK( NULL );
	vtree->pNode = goc_tableAdd(vtree->pNode, &vtree->nNode, sizeof(GOC_VTree*));
	vtree->pNode[ vtree->nNode-1 ] = node;
	return vtree;
}

GOC_VTree *goc_vtreeDetachNode(GOC_VTree* vtree, GOC_VTree* node)
{
	int i = 0;
	VCHECK( NULL );
	for (; i < vtree->nNode; i++)
	{
		if ( vtree->pNode[i] == node )
		{
			vtree->pNode = goc_tableRemove( vtree->pNode, &vtree->nNode, sizeof(GOC_VTree*), i );
			return node;
		}
	}
	return NULL;
}

GOC_VTree *goc_vtreeDetach(GOC_VTree* vtree, const char *nodename)
{
	int i = 0;
	VCHECK( NULL );
	for (; i < vtree->nNode; i++)
	{
		if ( goc_stringEquals(vtree->pNode[i]->name, nodename) )
		{
			GOC_VTree* result = vtree->pNode[i];
			vtree->pNode = goc_tableRemove( vtree->pNode, &vtree->nNode, sizeof(GOC_VTree*), i );
			return result;
		}
	}
	return NULL;
}
