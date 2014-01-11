#ifndef _VTREE_H_
#	define _VTREE_H_
#	include "tablica.h"

typedef struct GOC_VTree
{
	char *name;
	void *value;
	struct GOC_VTree **pNode;
	_GOC_TABEL_SIZETYPE_ nNode;
} GOC_VTree;

/**
 * Alloc tree node
 */
GOC_VTree *goc_vtreeAlloc();
/**
 * Free tree node and its resources
 */
GOC_VTree *goc_vtreeFree(GOC_VTree* vtree);
/**
 * Alloc tree node and set basic values
 */
GOC_VTree *goc_vtreeCreate(const char *name, void *value);
/**
 * Set node basic values
 */
GOC_VTree *goc_vtreeSet(GOC_VTree* vtree, const char *name, void *value);
/**
 * Add new node to tree with name and value
 */
GOC_VTree *goc_vtreeAdd(GOC_VTree* vtree, const char *name, void *value);
/**
 * Get node name
 */
const char *goc_vtreeGetName(const GOC_VTree* vtree);
/**
 * Get node value
 */
const char *goc_vtreeGetValue(const GOC_VTree* vtree);
/**
 * Find child node with name
 * Return node wich was found or NULL
 */
GOC_VTree *goc_vtreeGet(const GOC_VTree* vtree, const char* nodename);
/**
 * Remove a node with name from parent node vtree
 */
GOC_VTree *goc_vtreeRemove(GOC_VTree* vtree, const char* nodename);
/**
 * Remove a node from vtree
 */
GOC_VTree *goc_vtreeRemoveNode(GOC_VTree* vtree, GOC_VTree* node);
/**
 * Attacg node to tree
 * Return changed vtree
 */
GOC_VTree *goc_vtreeAttach(GOC_VTree* vtree, GOC_VTree* node);
/**
 * Detach node from vtree
 * Return detached node
 */
GOC_VTree *goc_vtreeDetachNode(GOC_VTree* vtree, GOC_VTree* node);
/**
 * Detach node from vtree
 * Return detached node finded by name
 */
GOC_VTree *goc_vtreeDetach(GOC_VTree* vtree, const char *nodename);

#endif // ifndef _VALUEDTREE_H_
