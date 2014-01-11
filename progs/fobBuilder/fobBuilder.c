#include <stdio.h>
#include <libxml/parser.h>
#include <okienkoc/plik.h>
#include <okienkoc/mystr.h>
#include <okienkoc/tablica.h>
#define GOC_PRINTINFO
#define GOC_PRINTERROR
#include <okienkoc/log.h>

#define DST_FOLDER "gen"

// definicje u¿ywanych sk³adowych XMLa wejsciowego
//
const char *CN_ROOTNODE = "Definitions";
const char *CN_CLASSNODE = "Class";
const char *CN_FIELDNODE = "Field";
const char *CN_TABLENODE = "Table";

xmlNode **pClassNode;
char **pClassName;
xmlNode **pClassParent;
int nClass;

/*
 * Pobranie atrybutu o nazwie name z podanego node child
 * Je¿eli nie ma zwróci NULL
 */
const char *getAttr(xmlNode *child, const char *name)
{
	xmlAttr *attr = NULL;

	attr = child->properties;
	while ( attr )
	{
		if ( goc_stringEquals((const char *)attr->name, name) )
			return (const char *)xmlNodeGetContent(attr->children);
		attr = attr->next;
	}
	return NULL;
}

const char *getChildNodeAttr(
	xmlNode *node, const char *nodename, const char *attrname)
{
	xmlNode *field = NULL;
	for ( field = node->children; field; field = field->next )
	{
		if ( field->type != XML_ELEMENT_NODE )
			continue;
		if ( goc_stringEquals( (const char *)field->name, nodename ) )
			return (const char *)getAttr(field, attrname);
	}
}

xmlNode *nextChild(xmlNode *lastNode, const char *pParentName)
{
	int i = -1;
	if ( lastNode )
	{
		for (i=0; i<nClass; i++)
		{
			if ( pClassNode[i] == lastNode )
				break;
		}
		// nie znaleziono podanego node-a
		if ( i == nClass )
			return NULL;
	}
	// wyszukanie nastepnego node-a
	for ( i++; i<nClass; i++ )
	{
		if ( !pClassParent[i] )
			continue;
		if ( goc_stringEquals(
			getAttr( pClassParent[i], "name" ),
			pParentName ) )
			return pClassNode[i];
	}
	// nie ma wiecej
	return NULL;
}


/*
 * Wypisanie pomocy do programu
 */
int printHelp(const char *progname)
{
	FILE *out = stdout;
	fprintf(out, "Usage: %s file.xml\n", progname);
	fprintf(out, "Generowanie kodu na podstawie podanego pliku XML\n");
	return 0;
}

/*
 * Wygenerowanie wnêtrza metody alloc dla podanego noda
 */
int generateAllocBody(xmlNode *child, xmlNode *method, FILE *file)
{
	char *classname = NULL;
	classname = goc_stringCopy(classname, getAttr(child, "name"));

	fprintf(file, "\tSt%s* tmp = NULL;\n", classname);
	fprintf(file, "\ttmp = malloc(sizeof(St%s));\n", classname);
	fprintf(file, "\tmemset(tmp, 0, sizeof(St%s));\n", classname);
	fprintf(file, "\treturn tmp;\n");

	classname = goc_stringFree(classname);
	return 0;
}

/*
 * Jesli jest zwroci, jesli nie ma NULL
 */
xmlNode *hasParent(const char *classname)
{
	int i = nClass;
	while ( i-- )
	{
		if ( goc_stringEquals( pClassName[i], classname ) )
			return pClassParent[i];
	}
	return NULL;
}

/*
 * Jesli jest zwroci, jesli nie ma NULL
 */
xmlNode *hasClass(const char *classname)
{
	int i = nClass;
	while ( i-- )
	{
		if ( goc_stringEquals( pClassName[i], classname ) )
			return pClassNode[i];
	}
	return NULL;
}

int generateCNames(FILE *code)
{
	int i;
	const char *classname;
	for ( i = 0; i<nClass; i++ )
	{
		xmlNode* child = pClassNode[i];
		if ( goc_stringEquals( child->name, CN_CLASSNODE) )
		{
			classname = getAttr( child, "name" );
			fprintf(code, "const char *cn_%s = \"%s\";\n", classname, classname);
		}
	}
	return 0;
}

int generateAllocMethod(FILE *code)
{
	int i;
	const char *classname;
	fprintf(code, "fobElement *fobAlloc(const char* cname)\n");
	fprintf(code, "{\n");
	fprintf(code, "\tfobElement* result;\n");
	for ( i = 0; i<nClass; i++ )
	{
		xmlNode* child = pClassNode[i];
		if ( goc_stringEquals( child->name, CN_CLASSNODE) )
		{
			classname = getAttr( child, "name" );
			fprintf(code, "\tif ( cname == cn_%s )\n", classname);
			fprintf(code, "\t{\n");
			fprintf(code, "\t\tMETHOD_ALLOC_FOB(result, %s);\n", classname);
			fprintf(code, "\t\treturn result;\n");
			fprintf(code, "\t}\n");
		}
	}
	fprintf(code, "}\n");
	return 0;
}

static int generateSerializeMethodClassFields(FILE *code, xmlNode* class)
{
	xmlNode* field = NULL;
	// wygenerowanie dla podanej klasy
	for ( field = class->children; field; field = field->next )
	{
		if ( field->type != XML_ELEMENT_NODE )
			continue;
		if ( goc_stringEquals((const char *)field->name, CN_FIELDNODE) )
		{
			const char *fname = getAttr(field, "name");
			const char *ftype = getAttr(field, "type");
			if ( goc_stringEquals(ftype, "int") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_INT(%s, out);\n", fname);
			else if ( goc_stringEquals(ftype, "uint") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_UINT(%s, out);\n", fname);
			else if ( goc_stringEquals(ftype, "byte") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_BYTE(%s, out);\n", fname);
			else if ( goc_stringEquals(ftype, "string") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_STRING(%s, out);\n", fname);
			else if ( hasClass( ftype ) )
				fprintf(code, "\t\tMETHOD_SERIALIZE_FOB(%s, out);\n", fname);
		}
		else if ( goc_stringEquals((const char *)field->name, CN_TABLENODE) )
		{
			const char *fname = getAttr(field, "name");
			const char *ftype = getAttr(field, "type");
			if ( goc_stringEquals(ftype, "int") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_TABLE_INT(%s, out);\n", fname);
			else if ( goc_stringEquals(ftype, "uint") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_TABLE_UINT(%s, out);\n", fname);
			else if ( goc_stringEquals(ftype, "byte") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_TABLE_BYTE(%s, out);\n", fname);
			else if ( goc_stringEquals(ftype, "string") )
				fprintf(code, "\t\tMETHOD_SERIALIZE_TABLE_STRING(%s, out);\n", fname);
			else if ( hasClass( ftype ) )
				fprintf(code, "\t\tMETHOD_SERIALIZE_TABLE_FOB(%s, out);\n", fname);
		}
	}
	// jezeli ma rodzica, wygeneruj pola rodzica
	const char *classname = getAttr(class, "name");
	class = hasParent( classname );
	if ( class )
		generateSerializeMethodClassFields(code, class);
}

int generateSerializeMethod(FILE *code)
{
	int i;
	const char *classname;
	fprintf(code, "void fobSerialize(fobElement* fob, FILE* out)\n");
	fprintf(code, "{\n");
	fprintf(code, "\tconst char *cname = fob->cname;\n");
	for ( i = 0; i<nClass; i++ )
	{
		xmlNode* child = pClassNode[i];
		if ( goc_stringEquals( child->name, CN_CLASSNODE) )
		{
			classname = getAttr( child, "name" );
			fprintf(code, "\tif ( cname == cn_%s )\n", classname);
			fprintf(code, "\t{\n");
			fprintf(code, "\t\tSt%s* fe = (St%s*)fob;\n", classname, classname);
			fprintf(code, "\t\tMETHOD_SERIALIZE_FOB_BEGIN(out, %s);\n", classname);
			// przeszukiwanie dostepnych galezi - wypelnianie polami struktury
			generateSerializeMethodClassFields(code, child);
			fprintf(code, "\t\tMETHOD_SERIALIZE_FOB_END(out, %s);\n", classname);
			fprintf(code, "\t\treturn;\n");
			fprintf(code, "\t}\n");
		}
	}
	fprintf(code, "}\n");
	return 0;
}

static int generateDeserializeMethodClassFields(FILE *code, xmlNode* class)
{
	xmlNode* field = NULL;
	for ( field = class->children; field; field = field->next )
	{
		if ( field->type != XML_ELEMENT_NODE )
			continue;
		if ( goc_stringEquals((const char *)field->name, CN_FIELDNODE) )
		{
			const char *fname = getAttr(field, "name");
			const char *ftype = getAttr(field, "type");
			if ( goc_stringEquals(ftype, "int") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_INT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "uint") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_UINT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "byte") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_BYTE(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "string") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_STRING(%s);\n", fname);
			else if ( hasClass( ftype ) )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_FOB(%s);\n", fname);
		}
		else if ( goc_stringEquals((const char *)field->name, CN_TABLENODE) )
		{
			const char *fname = getAttr(field, "name");
			const char *ftype = getAttr(field, "type");
			if ( goc_stringEquals(ftype, "int") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_TABLE_INT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "uint") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_TABLE_UINT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "byte") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_TABLE_BYTE(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "string") )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_TABLE_STRING(%s);\n", fname);
			else if ( hasClass( ftype ) )
				fprintf(code, "\t\tMETHOD_DESERIALIZE_TABLE_FOB(%s);\n", fname);
		}
	}
	// jezeli ma rodzica, wygeneruj pola rodzica
	const char *classname = getAttr(class, "name");
	class = hasParent( classname );
	if ( class )
		generateDeserializeMethodClassFields(code, class);
}

int generateDeserializeMethod(FILE *code)
{
	int i;
	const char *classname;
	fprintf(code, "fobElement *fobDeserialize(xmlNode* node)\n");
	fprintf(code, "{\n");
	for ( i = 0; i<nClass; i++ )
	{
		xmlNode* child = pClassNode[i];
		if ( goc_stringEquals( child->name, CN_CLASSNODE) )
		{
			classname = getAttr( child, "name" );
			fprintf(code, "\tMETHOD_DESERIALIZE_FOB_BEGIN(node, %s)\n", classname);
			// przeszukiwanie dostepnych galezi - wypelnianie polami struktury
			generateDeserializeMethodClassFields(code, child);
			fprintf(code, "\tMETHOD_DESERIALIZE_FOB_END(node, %s);\n", classname);
		}
	}
	fprintf(code, "}\n");
	return 0;
}

/*
 * wygenerowanie kodu dla klasy podanej w nodzie class
 */
int generateCode(FILE* code, const char* hname)
{
	int ret = 0;
	const char *classname = NULL;

	fprintf(code, "#include \"%s\"\n", hname);

	generateCNames(code);
	generateAllocMethod(code);
	generateSerializeMethod(code);
	generateDeserializeMethod(code);

	return ret;
}

/*
 * Wygenerowanie pól struktury w pliku nag³ówkowym
 */
int generateHeaderStructureFields(xmlNode *child, FILE *header)
{
	char *tmp = NULL;
	char *classname = goc_stringCopy(NULL, getAttr(child, "name"));
	xmlNode *field = hasParent(classname);
	// czy ma ojca
	if ( field )
	{
		generateHeaderStructureFields( field, header );
	}
	// przeszukiwanie dostepnych galezi - wypelnianie polami struktury
	for ( field = child->children; field; field = field->next )
	{
		if ( field->type != XML_ELEMENT_NODE )
			continue;
		if ( goc_stringEquals((const char *)field->name, CN_FIELDNODE) )
		{
			const char *fname = getAttr(field, "name");
			const char *ftype = getAttr(field, "type");
			if ( goc_stringEquals(ftype, "int") )
				fprintf(header, "\tTYPE_INT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "uint") )
				fprintf(header, "\tTYPE_UINT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "byte") )
				fprintf(header, "\tTYPE_BYTE(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "string") )
				fprintf(header, "\tTYPE_STRING(%s);\n", fname);
			else if ( hasClass( ftype ) )
				fprintf(header, "\tTYPE_FOB(%s, %s);\n", ftype, fname);
			else
				GOC_BINFO("Unknown field type [%s] of field [%s] in class [%s]", ftype, fname, classname);
		}
		else if ( goc_stringEquals((const char *)field->name, CN_TABLENODE) )
		{
			const char *fname = getAttr(field, "name");
			const char *ftype = getAttr(field, "type");
			if ( goc_stringEquals(ftype, "int") )
				fprintf(header, "\tTYPE_TABLE_INT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "uint") )
				fprintf(header, "\tTYPE_TABLE_UINT(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "byte") )
				fprintf(header, "\tTYPE_TABLE_BYTE(%s);\n", fname);
			else if ( goc_stringEquals(ftype, "string") )
				fprintf(header, "\tTYPE_TABLE_STRING(%s);\n", fname);
			else if ( hasClass( ftype ) )
				fprintf(header, "\tTYPE_TABLE_FOB(%s, %s);\n", ftype, fname);
			else
				GOC_BINFO("Unknown table type [%s] of table [%s] in class [%s]", ftype, fname, classname);
		}
	}
	return 0;
}

int generateMethodName(
	FILE *file, const char *classname, const char *methodname)
{
	char *name = goc_stringCopy(NULL, classname);
	name = goc_stringToLower(name);
	if ( goc_stringEquals(methodname, "Alloc") )
		fprintf(file, "St%s *%s%s()", 
			classname, name, methodname);
	else if ( goc_stringEquals(methodname, "Free") )
		fprintf(file, "St%s* %s%s(St%s* %s)", 
			classname, name, methodname, classname, name);
	else if ( goc_stringEquals(methodname, "Serialize") )
		fprintf(file, "int %s%s(St%s* %s, FILE *out)", 
			name, methodname, classname, name);
	else if ( goc_stringEquals(methodname, "SelectSerialize") )
		fprintf(file, "int %s%s(St%s* %s, FILE *out)", 
			name, methodname, classname, name);
	else if ( goc_stringEquals(methodname, "Deserialize") )
		fprintf(file, "St%s* %s%s(St%s* %s, xmlNode *node)", 
			classname, name, methodname, classname, name);
	else if ( goc_stringEquals(methodname, "SelectDeserialize") )
		fprintf(file, "St%s* %s%s(St%s* %s, xmlNode *node)", 
			classname, name, methodname, classname, name);
	else
		fprintf(file, "int %s%s(St%s* %s)", 
			name, methodname, classname, name);
}

/*
 * Wygenerowanie pliku nag³ówkowego
 */
int generateHeader(FILE* header)
{
	const char *classname = NULL;
	int i;

	// generowanie naglowkow
	fprintf(header, "#include \"fobHeader.h\"\n");
	// fprintf(header, "#include <stdio.h>\n", defName);
	// fprintf(header, "#include <libxml/parser.h>\n");
	// definiowanie struktury

	for ( i = 0; i<nClass; i++ )
	{
		xmlNode* child = pClassNode[i];
		if ( goc_stringEquals( child->name, CN_CLASSNODE) )
		{
			classname = getAttr( child, "name" );
			GOC_BINFO( "Generowanie klasy [%s]", classname );
			fprintf(header,"extern const char *cn_%s;\n",classname);
			fprintf(header, "\ntypedef struct St%s\n", classname);
			fprintf(header, "{\n");
			fprintf(header, "\tconst char *cname;\n");
			generateHeaderStructureFields( child, header );
			fprintf(header, "} St%s;\n\n", classname);
		}
	}

	return 0;
}

/*
 * Dodanie do zbioru klasy
 */
int addNewClass( const char *classname, xmlNode *node, xmlNode *parent )
{
	pClassName = goc_tableAdd(pClassName, &nClass, sizeof(char *));
	pClassName[--nClass] = goc_stringCopy(NULL, classname);
	pClassNode = goc_tableAdd(pClassNode, &nClass, sizeof(xmlNode*));
	pClassNode[--nClass] = node;
	pClassParent = goc_tableAdd(pClassParent, &nClass, sizeof(xmlNode*));
	pClassParent[nClass-1] = parent;
	return 0;
}

/*
 * przetwarzanie podanego pliku
 */
int doFile(const char *filename)
{
	FILE* genC = NULL;
	FILE* genH = NULL;

	xmlDoc *doc = NULL;
	xmlNode *root = NULL;
	xmlNode *child = NULL;
	int ret = 0;
	char *tmp = NULL;
	char *hname, *cname;
	int i;

	// zaladowanie pliku
	if ( !filename )
	{
		GOC_ERROR("Podano wartosn NULL");
		return -1;
	}
	if ( !goc_isFileExists(filename) )
	{
		GOC_BERROR("Plik [%s] nie istnieje", filename);
		return -1;
	}
	if ( ! goc_stringEndsWith(filename, ".xml") )
	{
		GOC_ERROR("Plik powinien miec rozszerzenie .xml");
		return -1;
	}

	doc = xmlReadFile( filename, NULL, 0 );
	if ( !doc )
	{
		GOC_BERROR("Nieudane parsowanie pliku [%s]", filename);
		return -1;
	}
	root = xmlDocGetRootElement( doc );
	if ( !root )
	{
		GOC_ERROR("Nieudane pozyskanie galezi root");
		return -1;
	}
	if ( !goc_stringEquals((const char *)root->name, CN_ROOTNODE) )
	{
		GOC_BERROR("Nieoczekiwana galaz root o nazwie [%s] - oczekiwano [%s]\n",
			(const char *)root->name, CN_ROOTNODE);
		return -1;
	}

	// przeszukiwanie dostepnych galezi i zebranie wszystkich klas
	for ( child = root->children; child; child = child->next )
	{
		const char *name = NULL;
		const char *classname = NULL;
		const char *parent = NULL;
		xmlNode *parentNode = NULL;

		if ( child->type != XML_ELEMENT_NODE )
			continue;
		name = (const char*)child->name;
		if ( goc_stringEquals(name, CN_CLASSNODE) )
		{
			classname = getAttr(child, "name");
			parent = getAttr(child, "parent");

			if ( classname == NULL )
			{
				GOC_ERROR("Nie okre¶lono nazwy klasy");
				return -1;
			}

			if ( parent )
			{
				// odszukaj rodzica budowanej klasy
				parentNode = hasClass(parent);
				if ( parentNode == NULL )
				{
					GOC_FERROR(
					goc_stringAdd(goc_stringCopy(NULL,
					"Brak definicji rodzica:"), parent));
					return -1;
				}
			}
		}
		else
			continue;
		addNewClass(classname, child, parentNode);
	}

	// otworzenie pliku naglowkowego
	hname = goc_stringCopy(hname, filename);
	hname = goc_stringReplaceStr(hname, ".xml", ".h");
	genH = fopen(hname, "wb");
	generateHeader( genH );
	if ( genH ) fclose(genH);

	// tworzenie pliku kodu
	cname = goc_stringCopy(cname, filename);
	cname = goc_stringReplaceStr(cname, ".xml", ".c");
	genC = fopen(cname, "wb");
	generateCode( genC, hname );
	if ( genC ) fclose(genC);

	hname = goc_stringFree(hname);
	cname = goc_stringFree(cname);

	xmlFreeDoc( doc );
	return 0;
}

/*
 * metoda uruchomienowa programu
 */
int main(int argc, const char **argv)
{
	pClassNode = NULL;
	pClassName = NULL;
	nClass = 0;
	printf("%s, wersja: 1.0a\n", argv[0]);
	if ( argc < 2 )
		return printHelp(argv[0]);
	return doFile(argv[1]);
}
