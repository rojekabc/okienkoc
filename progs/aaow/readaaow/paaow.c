#include "aaow.h"
#include <libxml/parser.h>
#define GOC_PRINTERROR
#include <okienkoc/log.h>
#include <okienkoc/okienkoc.h>
#include <okienkoc/fileistream.h>
#include <okienkoc/properties.h>

#define MAPHEIGHT 20
#define MAPWIDTH 20

#define TOGOC(variable) (GOC_StValuePoint*)(((char*)variable)+sizeof(const char*))
#define FROMGOC(variable) (((char*)variable)-sizeof(const char*))

#define FLAG_GREEN 0x10
#define FLAG_RED 0x20

typedef struct StPlayer {
	unsigned char flag;
} StPlayer;

// Game Options TODO:
// - know oponents flies ammount
// - show oponents flies on airports
// - show oponents flies move (which are out of view)
// - show oponents buildings properties

StPlayer player;

static int nasluchBuild(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	if ( wiesc == GOC_MSG_MAPGETCHAR )
	{
		GOC_StChar *znak = (GOC_StChar*)pBuf;
		// ustalenie informacji o przynaleznosci
		StBuild *build = (StBuild*)FROMGOC(nBuf);
		goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
		if ( build->flag == FLAG_GREEN )
			znak->color = GOC_GREEN | GOC_FBOLD;
		else if ( build->flag == FLAG_RED )
			znak->color = GOC_RED | GOC_FBOLD;
		else
			znak->color = GOC_WHITE;
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_POSMAPFREEPOINT )
	{
		GOC_StValuePoint *punkt = (GOC_StValuePoint*)pBuf;
		/* TODO FREE
		switch ( punkt->value )
		{
			case 0x03:
				cityFree((StCity*)punkt);
				break;
			case 0x07:
				capitalFree((StCapital*)punkt);
				break;
			case 0x04:
				airportFree((StAirport*)punkt);
				break;
			case 0x02:
				factoryFree((StFactory*)punkt);
				break;
			case 0x01:
				supplyFree((StSupply*)punkt);
				break;
			case 0x06:
				fortFree((StFort*)punkt);
				break;
			case 0x05:
				villageFree((StVillage*)punkt);
				break;
			case 0x08:
				bridgehFree((StBridgeH*)punkt);
				break;
			case 0x09:
				bridgevFree((StBridgeV*)punkt);
				break;
			default:
				goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
				break;
		}
		*/
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
}

GOC_HANDLER maska;
GOC_HANDLER terrain;
GOC_HANDLER build;
GOC_HANDLER front;
GOC_HANDLER labelDesc;
GOC_HANDLER labelGreenCount;
GOC_HANDLER labelRedCount;
GOC_HANDLER labelTickCount;
GOC_HANDLER labelInfo;
GOC_HANDLER slidebarSelector;

// [flag:green|red] [fly:fighter|bomber]
int flyCount[2][2];
int tickCount;
int tickSpeed;
// aktualne polozenie na mapie jakiegos wybranego elementu
GOC_StPoint *punkt = NULL;

const char* labelInfoText[] = {
	"[C]reate  [M]ission: %s"
};
const char* flyMission[] = {
	"Bombard", "Attack", "Scan"
};
short selectedMission;

int nasluchSelector(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	if ( wiesc == GOC_MSG_CHAR )
	{
		if ( nBuf == 9 )
		{
			// don't allow to remove focus
			return GOC_ERR_OK;
		}
		if (( nBuf == 'M' ) || ( nBuf == 'm' ))
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
		if (( nBuf == 'C' ) || ( nBuf == 'c' ))
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
		if ( nBuf == 27 )
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
		if (( nBuf == ' ' ) || ( nBuf == 13 ))
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
	return goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
}

int mapaNasluch(GOC_HANDLER uchwyt, GOC_MSG wiesc, void *pBuf, uintptr_t nBuf)
{
	if ( wiesc == GOC_MSG_CURSORCHANGE )
	{
		char buf[80];
		GOC_StValuePoint *v;
		int p;
		StAirport* airport = NULL;

		punkt = (GOC_StPoint*)pBuf;

		goc_labelRemLines(labelDesc);
		p = goc_maparawGetPoint(terrain, punkt->x, punkt->y);
		memset(buf, 0, 80);
		switch ( p )
		{
			case 0:
				sprintf(buf, "Terrain: forrest" );
				break;
			case 1:
				sprintf(buf, "Terrain: clear" );
				break;
			case 2:
				sprintf(buf, "Terrain: river" );
				break;
			case 3:
				sprintf(buf, "Terrain: mountains" );
				break;
			default:
				sprintf(buf, "Terrain: unknown" );
				break;
		}
		goc_labelAddLine(labelDesc, buf);
		v = goc_mapaposReadPoint(build, punkt->x, punkt->y);
		memset(buf, 0, 80);
		if ( v != NULL )
		{
			switch ( v-> value )
			{
				case 0x0:
					sprintf(buf, "");
					break;
				case 0x1:
					sprintf(buf, "Build: Supply");
					break;
				case 0x2:
					sprintf(buf, "Build: Factory");
					break;
				case 0x3:
					sprintf(buf, "Build: City");
					break;
				case 0x4:
					sprintf(buf, "Build: Airport");
					airport = (StAirport*)FROMGOC(v);
					break;
				case 0x5:
					sprintf(buf, "Build: Village");
					break;
				case 0x6:
					sprintf(buf, "Build: Fort");
					break;
				case 0x7:
					sprintf(buf, "Build: Capital");
					break;
				case 0x8:
				case 0x9:
					sprintf(buf, "Build: Bridge");
					break;
				default:
					sprintf(buf, "Build: unknown" );
					break;
			}
		}
		if ( strlen(buf) )
			goc_labelAddLine(labelDesc, buf);
		if ( airport )
		{
			int i;
			for (i=0; i<airport->nfly; i++)
			{
				sprintf(buf, "  %-20s [%1s:%1d]",
					airport->pfly[i]->pilotName,
					airport->pfly[i]->cname == cn_Fighter ? "F" : "B",
					airport->pfly[i]->pilotExp/10);
				goc_labelAddLine(labelDesc, buf);
			}

		}
		p = goc_maparawGetPoint(front, punkt->x, punkt->y);
		memset(buf, 0, 80);
		switch ( p )
		{
			case 0:
				break;
			case 1:
				sprintf(buf, "Front: red power" );
				break;
			case 2:
				sprintf(buf, "Front: green power" );
				break;
			case 3:
				sprintf(buf, "Front: none power" );
				break;
			default:
				sprintf(buf, "Front: unknown" );
				break;
		}
		if ( strlen( buf ) )
			goc_labelAddLine(labelDesc, buf);
		goc_systemClearArea(labelDesc);
		goc_systemSendMsg(labelDesc, GOC_MSG_PAINT, NULL, 0);
		return GOC_ERR_OK;
	}
	else if ( wiesc == GOC_MSG_CHAR )
	{
		if (( nBuf == ' ' ) || ( nBuf == 13 ))
		{
			GOC_StValuePoint *v = NULL;
			StBuild* building = NULL;
			if ( !punkt )
				return GOC_ERR_OK;
			v = goc_mapaposReadPoint(build, punkt->x, punkt->y);
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
					goc_systemSendMsg(slidebarSelector, GOC_MSG_PAINT, NULL, 0);
					goc_elementSetFunc(slidebarSelector, GOC_FUNID_LISTENER, nasluchSelector);

					labelInfo = goc_elementCreate( GOC_ELEMENT_LABEL, 2, 24, 0, 1,
						GOC_EFLAGA_PAINTED, GOC_WHITE, GOC_HANDLER_SYSTEM );
					selectedMission = 0;
					sprintf(buf, labelInfoText[0], flyMission[selectedMission]);
					goc_labelSetText( labelInfo, buf, GOC_FALSE );
					goc_systemSendMsg( labelInfo, GOC_MSG_PAINT, NULL, 0 );

					goc_systemFocusOn(slidebarSelector);
				}
			}
			return GOC_ERR_OK;
		}
	}
	return goc_systemDefaultAction(uchwyt, wiesc, pBuf, nBuf);
}

static char getChar(GOC_Properties* p, char* name, char defValue)
{
	char* value = NULL;
	if ( !p )
		return defValue;
	value = goc_propertiesGetValue(p, name);
	if ( !value )
		return defValue;
	if ( strlen(value) == 0 )
		return defValue;
	return value[0];
}

static int getColor(GOC_Properties* p, char* name, int defValue)
{
	char* value = NULL;
	if ( !p )
		return defValue;
	value = goc_propertiesGetValue(p, name);
	if ( !value )
		return defValue;
	if ( strlen(value) == 0 )
		return defValue;
	return goc_stringToColor(value);
}

void loadConfiguration(GOC_HANDLER terrain, GOC_HANDLER build, GOC_HANDLER front)
{
	GOC_IStream* is = NULL;
	GOC_Properties* props = NULL;

	is = goc_fileIStreamOpen( "aaow.properties" );
	if ( is )
	{
		props = goc_propertiesLoad( props, is );
		goc_isClose( is );
	}

	// definiuj wartosci
	goc_maparawSetBPV(terrain, 2); // 2 bity na warto¶æ
	goc_maparawAddChar(terrain, getChar(props, "terrain.tree.char", 't'), getColor(props, "terrain.tree.color", GOC_GREEN), 0);
	goc_maparawAddChar(terrain, getChar(props, "terrain.dirt.char", '.'), getColor(props, "terrain.dirt.color", GOC_WHITE), 1);
	goc_maparawAddChar(terrain, getChar(props, "terrain.river.char", '='), getColor(props, "terrain.river.color", GOC_CYAN), 2);
	goc_maparawAddChar(terrain, getChar(props, "terrain.mountain.char", '^'), getColor(props, "terrain.mountain.color", GOC_RED), 3);

	// definiuj wartosci
	goc_mapaposAddChar(build,  0 , GOC_WHITE, 0x00); // nic - don't add!
	goc_mapaposAddChar(build, getChar(props, "build.supply.char", 'S'), GOC_WHITE, 0x01); // supply
	goc_mapaposAddChar(build, getChar(props, "build.factory.char", 'f'), GOC_WHITE, 0x02); // factory
	goc_mapaposAddChar(build, getChar(props, "build.city.char", 'O'), GOC_WHITE, 0x03); // city
	goc_mapaposAddChar(build, getChar(props, "build.airport.char", 'L'), GOC_WHITE, 0x04); // airport
	goc_mapaposAddChar(build, getChar(props, "build.village.char", 'o'), GOC_WHITE, 0x05); // village
	goc_mapaposAddChar(build, getChar(props, "build.fort.char", '#'), GOC_WHITE, 0x06); // fort
	goc_mapaposAddChar(build, getChar(props, "build.capital.char", '0'), GOC_WHITE, 0x07); // capital
	goc_mapaposAddChar(build, getChar(props, "build.bridge.vertical.char", '-'), GOC_WHITE, 0x08); // bridge
	goc_mapaposAddChar(build, getChar(props, "build.bridge.horizontal.char", '|'), GOC_WHITE, 0x09); // bridge

	goc_maparawSetBPV(front, 2); // 2 bity na warto¶æ
	goc_maparawAddChar(front, 0, GOC_WHITE, 0);
	goc_maparawAddChar(front, getChar(props, "front.char", 'x'), GOC_RED | GOC_FBOLD, 1);
	goc_maparawAddChar(front, getChar(props, "front.char", 'x'), GOC_GREEN | GOC_FBOLD, 2);
	goc_maparawAddChar(front, getChar(props, "front.char", 'x'), GOC_CYAN | GOC_FBOLD, 3);
	props = goc_propertiesFree( props );
}

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

	// stworzenie maski
	maska = goc_elementCreate( GOC_ELEMENT_MASK, 2, 3, MAPWIDTH, MAPHEIGHT, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );

	terrain = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);

	build = goc_elementCreate(GOC_ELEMENT_POSMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);
	// dodanie nasluchiwania oraz zmiana sposobu zwalniania punktow, ktore sa predefiniowane
	goc_elementSetFunc(build, GOC_FUNID_LISTENER, nasluchBuild);

	front = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);

	// przestrzen do opisywania
	labelDesc = goc_elementCreate( GOC_ELEMENT_LABEL, 45, 3, 30, 20, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );

	// liczniki
	labelTickCount = goc_elementCreate( GOC_ELEMENT_LABEL, 2, 1, 13, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	labelGreenCount = goc_elementCreate( GOC_ELEMENT_LABEL, 15, 1, 12, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_GREEN, GOC_HANDLER_SYSTEM );
	labelRedCount = goc_elementCreate( GOC_ELEMENT_LABEL, 28, 1, 12, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_RED, GOC_HANDLER_SYSTEM );
	// definiuj wartosci
	loadConfiguration( terrain, build, front );

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

	// stworzenie maski
	goc_maskAddMap(maska, terrain);
	goc_maskAddMap(maska, build);
	goc_maskAddMap(maska, front);
	goc_maskCharSpace(maska, 2, 1);
	// ustawienie nasluchiwania
	goc_elementSetFunc(maska, GOC_FUNID_LISTENER, mapaNasluch);

	// inicjuj graczy
	player.flag = FLAG_GREEN;
	
	// uruchomienie systemu
	goc_systemFocusOn(maska);
	GOC_MSG wiesc;
	while (goc_systemCheckMsg( &wiesc ));
	
	return 0;
}
