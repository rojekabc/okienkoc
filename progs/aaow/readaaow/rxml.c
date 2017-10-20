#include <libxml/parser.h>
#define GOC_PRINTERROR
#include <okienkoc/log.h>
#include <okienkoc/okienkoc.h>
#include <okienkoc/fileistream.h>
#include <okienkoc/properties.h>

#include "aaow.h"
#include "common.h"

int main(int argc, char **argv)
{
	// zaladowanie dokumentu
	xmlDoc *doc = NULL;
	xmlNode *root = NULL;
	if ( argc != 2 )
	{
		GOC_ERROR("No argument");
		return -1;
	}
	doc = xmlReadFile( argv[1], NULL, 0 );
	if ( doc == NULL )
	{
		GOC_ERROR("No XML Doc");
		return -1;
	}
	root = xmlDocGetRootElement(doc);
	if ( root == NULL )
	{
		GOC_ERROR("No XML Root");
		return -1;
	}
	StGameData* gameData = (StGameData*)fobDeserialize(root);
	xmlFreeDoc(doc);

	createCommonObjects();

	// przenisienie danych do map
	{
		// teren
		GOC_StMapRaw *map = (GOC_StMapRaw*)terrain;
		goc_nbitFieldFromString(map->dane, gameData->terrain);
	}
	{
		// front
		GOC_StMapRaw *map = (GOC_StMapRaw*)front;
		goc_nbitFieldFromString(map->dane, gameData->front);
	}
	{
		// budynki
		int i;
		for (i=0; i<gameData->nbuild; i++)
		{
			StBuild* punkt = gameData->pbuild[i];
			punkt->value |= punkt->flag;
			goc_mapaposAddPoint(build, TOGOC(punkt));
		}
	}

	// uruchomienie systemu
	GOC_StMessage msg;
	while (goc_systemCheckMsg( &msg ));
	
	return 0;
}
