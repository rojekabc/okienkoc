#include <okienkoc/properties.h>
#include <okienkoc/fileistream.h>
#include <okienkoc/okienkoc.h>

#include "common.h"
#include "aaow.h"

GOC_HANDLER maska;
GOC_HANDLER terrain;
GOC_HANDLER build;
GOC_HANDLER front;
GOC_HANDLER labelDesc;

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


static void loadConfiguration(GOC_HANDLER terrain, GOC_HANDLER build, GOC_HANDLER front)
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

	/* Now commented - all comes from listener calculation
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
	*/

	goc_maparawSetBPV(front, 2); // 2 bity na warto¶æ
	goc_maparawAddChar(front, 0, GOC_WHITE, 0);
	goc_maparawAddChar(front, getChar(props, "front.char", 'x'), GOC_RED | GOC_FBOLD, 1);
	goc_maparawAddChar(front, getChar(props, "front.char", 'x'), GOC_GREEN | GOC_FBOLD, 2);
	goc_maparawAddChar(front, getChar(props, "front.char", 'x'), GOC_CYAN | GOC_FBOLD, 3);
	props = goc_propertiesFree( props );
}

static int buildCommonListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_MAPGETCHAR_ID )
	{
		// here is our own organization
		GOC_StMsgMapChar* msgchar = (GOC_StMsgMapChar*)msg;
		// goc_systemDefaultAction(uchwyt, msg);
		switch ( msgchar->position & 0x0F ) {
			case 0x00: msgchar->charcode = 0; break;
			case 0x01: msgchar->charcode = 'S'; break;
			case 0x02: msgchar->charcode = 'f'; break;
			case 0x03: msgchar->charcode = 'O'; break;
			case 0x04: msgchar->charcode = 'L'; break;
			case 0x05: msgchar->charcode = 'o'; break;
			case 0x06: msgchar->charcode = '#'; break;
			case 0x07: msgchar->charcode = '0'; break;
			case 0x08: msgchar->charcode = '-'; break;
			case 0x09: msgchar->charcode = '|'; break;
		}
		switch (msgchar->position & 0xF0) {
			case FLAG_GREEN: msgchar->charcolor=GOC_GREEN | GOC_FBOLD; break;
			case FLAG_RED: msgchar->charcolor=GOC_RED | GOC_FBOLD; break;
			default: msgchar->charcolor=GOC_WHITE; break;
		}
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_POSMAPFREEPOINT_ID )
	{
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
	return goc_systemDefaultAction(uchwyt, msg);
}

static int mapaCommonListener(GOC_HANDLER uchwyt, GOC_StMessage *msg)
{
	if ( msg->id == GOC_MSG_CURSORCHANGE_ID )
	{
		GOC_StMsgMapPoint* msgp = (GOC_StMsgMapPoint*)msg;
		char buf[80];
		GOC_StValuePoint *v;
		int p;
		StAirport* airport = NULL;

		goc_labelRemLines(labelDesc);
		p = goc_maparawGetPoint(terrain, msgp->x, msgp->y);
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
		v = goc_mapaposReadPoint(build, msgp->x, msgp->y);
		memset(buf, 0, 80);
		if ( v != NULL )
		{
			switch ( v-> value & 0x0F )
			{
				case 0x00:
					sprintf(buf, "");
					break;
				case 0x01:
					sprintf(buf, "Build: Supply");
					break;
				case 0x02:
					sprintf(buf, "Build: Factory");
					break;
				case 0x03:
					sprintf(buf, "Build: City");
					break;
				case 0x04:
					sprintf(buf, "Build: Airport");
					airport = (StAirport*)FROMGOC(v);
					break;
				case 0x05:
					sprintf(buf, "Build: Village");
					break;
				case 0x06:
					sprintf(buf, "Build: Fort");
					break;
				case 0x07:
					sprintf(buf, "Build: Capital");
					break;
				case 0x08:
				case 0x09:
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
		memset(buf, 0, 80);
		if ( v != NULL ) {
			switch ( v->value & 0xF0 )
			{
				case 0x00:
					break;
				case 0x10:
					sprintf(buf, "Front: green power" );
					break;
				case 0x20:
					sprintf(buf, "Front: red power" );
					break;
				case 0x30:
					sprintf(buf, "Front: none power" );
					break;
				default:
					sprintf(buf, "Front: unknown" );
					break;
			}
		}
		if ( strlen( buf ) )
			goc_labelAddLine(labelDesc, buf);
		goc_systemClearArea(labelDesc);
		GOC_MSG_PAINT( msgPaint );
		goc_systemSendMsg(labelDesc, msgPaint);
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

void createCommonObjects() {
	maska = goc_elementCreate( GOC_ELEMENT_MASK, 1, 3, MAPWIDTH, MAPHEIGHT, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	terrain = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);

	build = goc_elementCreate(GOC_ELEMENT_POSMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);
	// dodanie nasluchiwania oraz zmiana sposobu zwalniania punktow, ktore sa predefiniowane
	goc_elementSetFunc(build, GOC_FUNID_LISTENER, buildCommonListener);

	front = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, MAPWIDTH, MAPHEIGHT,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, maska);
	// definiuj wartosci
	loadConfiguration( terrain, build, front );

	goc_maskAddMap(maska, terrain);
	goc_maskAddMap(maska, build);
	goc_maskAddMap(maska, front);
	goc_maskCharSpace(maska, 2, 1);
	goc_elementSetFunc(maska, GOC_FUNID_LISTENER, mapaCommonListener);

	// przestrzen do opisywania
	labelDesc = goc_elementCreate( GOC_ELEMENT_LABEL, 50, 2, 30, 20, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );

}
