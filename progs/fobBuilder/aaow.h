#include "fobHeader.h"
extern const char *cn_Configuration;

typedef struct StConfiguration
{
	const char *cname;
	TYPE_BYTE(supplyObjectsMax);
	TYPE_BYTE(supplyRadius);
	TYPE_BYTE(supplyTick);
	TYPE_BYTE(supplyStore);
	TYPE_BYTE(supplyRepairTick);
	TYPE_BYTE(tickTime);
	TYPE_STRING(flySeen);
	TYPE_BYTE(factoryRadius);
	TYPE_BYTE(factoryStore);
	TYPE_BYTE(factoryTick);
	TYPE_BYTE(factoryRepairTick);
	TYPE_BYTE(cityWorkersRadius);
	TYPE_BYTE(citySupplyTick);
	TYPE_BYTE(citySypplyRadius);
	TYPE_BYTE(cityRepairTick);
	TYPE_BYTE(airportFlyMax);
	TYPE_BYTE(airportSupplyMax);
	TYPE_BYTE(airportRepairTick);
	TYPE_BYTE(villageSupplyTick);
	TYPE_BYTE(villageSupplyRadius);
	TYPE_BYTE(villageRepairTick);
	TYPE_BYTE(fortRepairTick);
	TYPE_BYTE(capitalRepaitTick);
} StConfiguration;

extern const char *cn_ValuePunkt;

typedef struct StValuePunkt
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
} StValuePunkt;

extern const char *cn_Fly;

typedef struct StFly
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_STRING(pilotName);
	TYPE_BYTE(pilotExp);
	TYPE_BYTE(live);
	TYPE_BYTE(flag);
	TYPE_BYTE(fuel);
} StFly;

extern const char *cn_Bomber;

typedef struct StBomber
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_STRING(pilotName);
	TYPE_BYTE(pilotExp);
	TYPE_BYTE(live);
	TYPE_BYTE(flag);
	TYPE_BYTE(fuel);
	TYPE_BYTE(bomb);
} StBomber;

extern const char *cn_Fighter;

typedef struct StFighter
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_STRING(pilotName);
	TYPE_BYTE(pilotExp);
	TYPE_BYTE(live);
	TYPE_BYTE(flag);
	TYPE_BYTE(fuel);
} StFighter;

extern const char *cn_Build;

typedef struct StBuild
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
} StBuild;

extern const char *cn_Supply;

typedef struct StSupply
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
	TYPE_TABLE_FOB(ValuePunkt, dest);
	TYPE_BYTE(tick);
	TYPE_BYTE(store);
} StSupply;

extern const char *cn_Factory;

typedef struct StFactory
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
	TYPE_BYTE(tick);
	TYPE_TABLE_FOB(Fly, store);
	TYPE_TABLE_FOB(ValuePunkt, dest);
} StFactory;

extern const char *cn_City;

typedef struct StCity
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
	TYPE_BYTE(tick);
} StCity;

extern const char *cn_Airport;

typedef struct StAirport
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
	TYPE_TABLE_FOB(Fly, fly);
	TYPE_BYTE(store);
} StAirport;

extern const char *cn_Village;

typedef struct StVillage
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
	TYPE_BYTE(tick);
} StVillage;

extern const char *cn_Fort;

typedef struct StFort
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
} StFort;

extern const char *cn_Capital;

typedef struct StCapital
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
} StCapital;

extern const char *cn_BridgeH;

typedef struct StBridgeH
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
} StBridgeH;

extern const char *cn_BridgeV;

typedef struct StBridgeV
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_BYTE(live);
	TYPE_BYTE(repairTick);
	TYPE_BYTE(flag);
} StBridgeV;

extern const char *cn_MoveUnit;

typedef struct StMoveUnit
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_INT(identifier);
} StMoveUnit;

extern const char *cn_SupplyUnit;

typedef struct StSupplyUnit
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_INT(identifier);
	TYPE_BYTE(destx);
	TYPE_BYTE(desty);
	TYPE_FOB(Build, destbuild);
} StSupplyUnit;

extern const char *cn_Mission;

typedef struct StMission
{
	const char *cname;
	TYPE_BYTE(type);
} StMission;

extern const char *cn_Eskadra;

typedef struct StEskadra
{
	const char *cname;
	TYPE_UINT(x);
	TYPE_UINT(y);
	TYPE_INT(value);
	TYPE_INT(identifier);
	TYPE_TABLE_FOB(Fly, fly);
	TYPE_TABLE_FOB(Mission, mission);
} StEskadra;

extern const char *cn_Scaning;

typedef struct StScaning
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_BYTE(x);
	TYPE_BYTE(y);
} StScaning;

extern const char *cn_Bombard;

typedef struct StBombard
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_BYTE(x);
	TYPE_BYTE(y);
} StBombard;

extern const char *cn_FrontAttack;

typedef struct StFrontAttack
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_BYTE(x);
	TYPE_BYTE(y);
} StFrontAttack;

extern const char *cn_FrontHelp;

typedef struct StFrontHelp
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_BYTE(x);
	TYPE_BYTE(y);
} StFrontHelp;

extern const char *cn_Land;

typedef struct StLand
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_BYTE(x);
	TYPE_BYTE(y);
} StLand;

extern const char *cn_FlyAttack;

typedef struct StFlyAttack
{
	const char *cname;
	TYPE_BYTE(type);
	TYPE_INT(identifier);
} StFlyAttack;

extern const char *cn_GameData;

typedef struct StGameData
{
	const char *cname;
	TYPE_STRING(title);
	TYPE_STRING(description);
	TYPE_BYTE(mapwidth);
	TYPE_BYTE(mapheight);
	TYPE_STRING(terrain);
	TYPE_STRING(front);
	TYPE_TABLE_FOB(Build, build);
	TYPE_TABLE_FOB(Eskadra, fly);
} StGameData;

