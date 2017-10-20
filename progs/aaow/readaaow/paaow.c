#include <libxml/parser.h>
#define GOC_PRINTERROR
#include <okienkoc/log.h>
#include <okienkoc/okienkoc.h>
#include <okienkoc/fileistream.h>
#include <okienkoc/properties.h>

#include "aaow.h"
#include "common.h"

#define MAPHEIGHT 20
#define MAPWIDTH 20

typedef struct StPlayer {
	unsigned char flag;
} StPlayer;

// Game Options TODO:
// - know oponents flies ammount
// - show oponents flies on airports
// - show oponents flies move (which are out of view)
// - show oponents buildings properties

StPlayer player;

GOC_HANDLER maska;
GOC_HANDLER labelGreenCount;
GOC_HANDLER labelRedCount;
GOC_HANDLER labelTickCount;
GOC_HANDLER labelInfo;
GOC_HANDLER slidebarSelector;

// [flag:green|red] [fly:fighter|bomber]
int flyCount[2][2];
int tickCount;
int tickSpeed;

const char* labelInfoText[] = {
	"[C]reate  [M]ission: %s"
};
const char* flyMission[] = {
	"Bombard", "Attack", "Scan"
};
short selectedMission;

int nasluchSelector(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CHAR_ID )
	{
		GOC_StMsgChar* msgChar = (GOC_StMsgChar*)msg;
		if ( msgChar->charcode == 9 )
		{
			// don't allow to remove focus
			return GOC_ERR_OK;
		}
		if (( msgChar->charcode == 'M' ) || ( msgChar->charcode == 'm' ))
		{
			// Change mission type
			GOC_StSlideBar* slide = (GOC_StSlideBar*)uchwyt;
			char buf[80];
			selectedMission++;
			selectedMission %= sizeof(flyMission)/sizeof(char*);
			sprintf(buf, labelInfoText[0], flyMission[selectedMission]);
			goc_labelSetText( labelInfo, buf, GOC_FALSE );
			goc_gotoxy( goc_elementGetScreenX( uchwyt ), goc_elementGetScreenY( uchwyt ) + slide->position );
			fflush(stdout);
		}
		if (( msgChar->charcode == 'C' ) || ( msgChar->charcode == 'c' ))
		{
			// TODO: Accept creation of new fly team
			// - create fly team
			// - remove flies from airport
			// - destory elements
			goc_systemClearArea(uchwyt);
			goc_systemDestroyElement(uchwyt);
			goc_systemClearArea(labelInfo);
			goc_systemDestroyElement(labelInfo);
			// - move focus back to map
			goc_systemFocusOn(maska);
			fflush(stdout);
			return GOC_ERR_OK;
		}
		if ( msgChar->charcode == 27 )
		{
			// - destory elements
			goc_systemClearArea(uchwyt);
			goc_systemDestroyElement(uchwyt);
			goc_systemClearArea(labelInfo);
			goc_systemDestroyElement(labelInfo);
			// - move focus back to map
			goc_systemFocusOn(maska);
			return GOC_ERR_OK;
		}
		if (( msgChar->charcode == ' ' ) || ( msgChar->charcode == 13 ))
		{
			// - Select a fly to team (color green says selected)
			GOC_StSlideBar* slide = (GOC_StSlideBar*)uchwyt;
			GOC_COLOR color = goc_labelGetColor(labelDesc, 2+slide->position);
			if ( color == GOC_WHITE )
				goc_labelSetColor(labelDesc, 2+slide->position, GOC_GREEN);
			else
				goc_labelSetColor(labelDesc, 2+slide->position, GOC_WHITE);
			goc_gotoxy( goc_elementGetScreenX( uchwyt ), goc_elementGetScreenY( uchwyt ) + slide->position );
			fflush(stdout);
			return GOC_ERR_OK;
		}
		
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

	/*
	else if ( msg->id == GOC_MSG_CHAR_ID )
	{
		GOC_StMsgChar* msgch = (GOC_StMsgChar*)msg;
		if (( msgch->charcode == ' ' ) || ( msgch->charcode == 13 ))
		{
			GOC_StValuePoint *v = NULL;
			StBuild* building = NULL;
			GOC_StPoint mapCursor;
			goc_maskGetCursor( maska, mapCursor );
			v = goc_mapaposReadPoint(build, mapCursor.x, mapCursor.y);
			if ( !v )
				return GOC_ERR_OK;
			building = (StBuild*)FROMGOC(v);
			if ( building->flag != player.flag )
				return GOC_ERR_OK;
			if ( building->cname == cn_Airport )
			{
				StAirport* airport = (StAirport*)building;
				if ( airport->nfly > 0 )
				{
					char buf[80];
					slidebarSelector = goc_elementCreate( GOC_ELEMENT_SLIDEBAR, 43, 5, 1, airport->nfly,
						GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, GOC_HANDLER_SYSTEM );
					GOC_MSG_PAINT( msgpaint );
					goc_systemSendMsg(slidebarSelector, msgpaint);
					goc_elementSetFunc(slidebarSelector, GOC_FUNID_LISTENER, nasluchSelector);

					labelInfo = goc_elementCreate( GOC_ELEMENT_LABEL, 2, 24, 0, 1,
						GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM );
					selectedMission = 0;
					sprintf(buf, labelInfoText[0], flyMission[selectedMission]);
					goc_labelSetText( labelInfo, buf, GOC_FALSE );
					goc_systemSendMsg(labelInfo, msgpaint);

					goc_systemFocusOn(slidebarSelector);
				}
			}
			return GOC_ERR_OK;
		}
	}
	*/

static void setFlyCounters()
{
	char buf[20];
	goc_labelRemLines(labelGreenCount);
	goc_labelRemLines(labelRedCount);
	sprintf(buf, "F: %02d B: %02d", flyCount[0][0], flyCount[0][1]);
	goc_labelAddLine(labelGreenCount, buf);
	sprintf(buf, "F: %02d B: %02d", flyCount[1][0], flyCount[1][1]);
	goc_labelAddLine(labelRedCount, buf);
}

static void setTickCount()
{
	char buf[15];
	goc_labelRemLines(labelTickCount);
	sprintf(buf, "T: %04d (%d)", tickCount, tickSpeed);
	goc_labelAddLine(labelTickCount, buf);
}

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

	// liczniki
	labelTickCount = goc_elementCreate( GOC_ELEMENT_LABEL, 2, 1, 13, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	labelGreenCount = goc_elementCreate( GOC_ELEMENT_LABEL, 15, 1, 12, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	labelRedCount = goc_elementCreate( GOC_ELEMENT_LABEL, 28, 1, 12, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_RED, GOC_HANDLER_SYSTEM );

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

	// zliczanie jednostek po obu stronach
	{
		memset(flyCount, 0, sizeof(flyCount));
		int i;
		for (i=0; i<gameData->nbuild; i++)
		{
			StBuild* punkt = gameData->pbuild[i];
			if ( punkt->cname == cn_Airport )
			{
				int j;
				StAirport* airport = (StAirport*)punkt;
				for (j=0; j<airport->nfly; j++)
				{
					StFly* fly = (StFly*)airport->pfly[j];
					if ( fly->cname == cn_Fighter )
					{
						flyCount[(fly->flag>>4)-1][0]++;
					}
					else if ( fly->cname == cn_Bomber )
					{
						flyCount[(fly->flag>>4)-1][1]++;
					}
				}
			}
		}
		setFlyCounters();
	}

	// ustawienie tick
	tickCount = 0;
	tickSpeed = 1;
	setTickCount();

	// inicjuj graczy
	player.flag = FLAG_GREEN;
	
	// uruchomienie systemu
	goc_systemFocusOn(maska);
	GOC_StMessage msg;
	while (goc_systemCheckMsg( &msg ));
	
	return 0;
}
