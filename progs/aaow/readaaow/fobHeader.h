#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <tools/mystr.h>
#include <tools/tablica.h>
#include <libxml/parser.h>

#define TYPE_STRING(name) char* name;
#define TYPE_BYTE(name) unsigned char name;
#define TYPE_INT(name) int name;
#define TYPE_UINT(name) unsigned int name;
#define TYPE_FOB(type, name) St ## type* name;

#define TYPE_TABLE_STRING(name) char** p ## name; int n ## name;
#define TYPE_TABLE_FOB(type, name) St ## type** p ## name; int n ## name;

//	result = (St ## name*)malloc(sizeof(St ## name));
#define METHOD_ALLOC_FOB(result, name)\
	result = malloc(sizeof(St ## name));\
	memset(result, 0, sizeof(St ## name));\
	result->cname = cn_ ## name;

#define	METHOD_SERIALIZE_FOB_BEGIN(out, name)\
	fprintf(out, "<%s>", #name);

#define	METHOD_SERIALIZE_FOB_END(out, name)\
	fprintf(out, "</%s>", #name);

#define METHOD_SERIALIZE_STRING(name, out)\
	fprintf(out, "<%s>%s</%s>", #name, fe->name ? fe->name : "", #name);

#define METHOD_SERIALIZE_INT(name, out)\
	fprintf(out, "<%s>%i</%s>", #name, fe->name, #name);

#define METHOD_SERIALIZE_UINT(name, out)\
	fprintf(out, "<%s>%u</%s>", #name, fe->name, #name);

#define METHOD_SERIALIZE_BYTE(name, out)\
	fprintf(out, "<%s>%u</%s>", #name, (unsigned int)fe->name, #name);

#define METHOD_SERIALIZE_FOB(name, out)\
	fobSerialize((fobElement*)fe->name, out);

#define METHOD_SERIALIZE_TABLE_STRING(name, out)\
{ \
	int i;\
	for (i=0; i<fe->n ## name; i++)\
		fprintf(out, "<%s>%s</%s>", #name, fe->p ## name[i] ? fe->p ## name[i] : "", #name);\
}

#define METHOD_SERIALIZE_TABLE_FOB(name, out)\
{\
	int i;\
	fprintf(out, "<%s>", #name);\
	for (i=0; i<fe->n ## name; i++)\
	{\
		fobSerialize((fobElement*)fe->p ## name[i], out);\
	}\
	fprintf(out, "</%s>", #name);\
}

#define METHOD_DESERIALIZE_FOB_BEGIN(node, classname)\
	if ( goc_stringEquals(node->name, #classname) ) \
	{ \
		xmlNode* child = NULL; \
		St ## classname* result = NULL; \
		METHOD_ALLOC_FOB(result, classname); \
		for (child=node->children; child; child=child->next)\
		{ \
			if (child->type!=XML_ELEMENT_NODE)\
				continue;\


#define METHOD_DESERIALIZE_FOB_END(node, classname) \
		} \
		return (fobElement*)result; \
	}

#define METHOD_DESERIALIZE_INT(fieldname)\
	if ( goc_stringEquals(child->name, #fieldname) ) \
	{ \
		result->fieldname = atoi( xmlNodeGetContent( child ) ); \
		continue; \
	}

#define METHOD_DESERIALIZE_UINT(fieldname)\
	if ( goc_stringEquals(child->name, #fieldname) ) \
	{ \
		result->fieldname = atoi( xmlNodeGetContent( child ) ); \
		continue; \
	}

#define METHOD_DESERIALIZE_BYTE(fieldname)\
	if ( goc_stringEquals(child->name, #fieldname) ) \
	{ \
		result->fieldname = (unsigned char)atoi( xmlNodeGetContent( child ) ); \
		continue; \
	}

#define METHOD_DESERIALIZE_STRING(fieldname)\
	if ( goc_stringEquals(child->name, #fieldname) ) \
	{ \
		result->fieldname = goc_stringCopy( result->fieldname, xmlNodeGetContent( child ) ); \
		continue; \
	}

#define METHOD_DESERIALIZE_FOB(fieldname)\
	if ( goc_stringEquals(child->name, #fieldname) ) \
	{ \
		xmlNode* childfob = NULL;\
		for (childfob=child->children; childfob; childfob=childfob->next)\
		{\
			if (child->type!=XML_ELEMENT_NODE)\
				continue;\
			result->fieldname = (void*)fobDeserialize(childfob);\
		}\
		continue; \
	}

#define METHOD_DESERIALIZE_TABLE_STRING(fieldname)\
	if ( goc_stringEquals(child->name, #fieldname) ) \
	{ \
		result->p ## fieldname = (char **)goc_tableAdd( result->p ## fieldname, & (result->n ## fieldname), sizeof(char*) ); \
		result->p ## fieldname[ result->n ## fieldname - 1 ] = goc_stringCopy(NULL, xmlNodeGetContent( child ) ); \
		continue; \
	}

#define METHOD_DESERIALIZE_TABLE_FOB(fieldname)\
	if ( goc_stringEquals(child->name, #fieldname) )\
	{\
		xmlNode* childfob = NULL;\
		for (childfob=child->children; childfob; childfob=childfob->next)\
		{\
			if (child->type!=XML_ELEMENT_NODE)\
				continue;\
			result->p ## fieldname = (void*)goc_tableAdd( result->p ## fieldname, & (result->n ## fieldname), sizeof(void*) );\
			result->p ## fieldname[ result->n ## fieldname - 1 ] = (void*)fobDeserialize(childfob);\
		}\
		continue;\
	}

typedef struct fobElement
{
	const char *cname;
} fobElement;
