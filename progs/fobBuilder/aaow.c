#include "aaow.h"
const char *cn_Configuration = "Configuration";
const char *cn_ValuePunkt = "ValuePunkt";
const char *cn_Fly = "Fly";
const char *cn_Bomber = "Bomber";
const char *cn_Fighter = "Fighter";
const char *cn_Build = "Build";
const char *cn_Supply = "Supply";
const char *cn_Factory = "Factory";
const char *cn_City = "City";
const char *cn_Airport = "Airport";
const char *cn_Village = "Village";
const char *cn_Fort = "Fort";
const char *cn_Capital = "Capital";
const char *cn_BridgeH = "BridgeH";
const char *cn_BridgeV = "BridgeV";
const char *cn_MoveUnit = "MoveUnit";
const char *cn_SupplyUnit = "SupplyUnit";
const char *cn_Mission = "Mission";
const char *cn_Eskadra = "Eskadra";
const char *cn_Scaning = "Scaning";
const char *cn_Bombard = "Bombard";
const char *cn_FrontAttack = "FrontAttack";
const char *cn_FrontHelp = "FrontHelp";
const char *cn_Land = "Land";
const char *cn_FlyAttack = "FlyAttack";
const char *cn_GameData = "GameData";
fobElement *fobAlloc(const char* cname)
{
	fobElement* result;
	if ( cname == cn_Configuration )
	{
		METHOD_ALLOC_FOB(result, Configuration);
		return result;
	}
	if ( cname == cn_ValuePunkt )
	{
		METHOD_ALLOC_FOB(result, ValuePunkt);
		return result;
	}
	if ( cname == cn_Fly )
	{
		METHOD_ALLOC_FOB(result, Fly);
		return result;
	}
	if ( cname == cn_Bomber )
	{
		METHOD_ALLOC_FOB(result, Bomber);
		return result;
	}
	if ( cname == cn_Fighter )
	{
		METHOD_ALLOC_FOB(result, Fighter);
		return result;
	}
	if ( cname == cn_Build )
	{
		METHOD_ALLOC_FOB(result, Build);
		return result;
	}
	if ( cname == cn_Supply )
	{
		METHOD_ALLOC_FOB(result, Supply);
		return result;
	}
	if ( cname == cn_Factory )
	{
		METHOD_ALLOC_FOB(result, Factory);
		return result;
	}
	if ( cname == cn_City )
	{
		METHOD_ALLOC_FOB(result, City);
		return result;
	}
	if ( cname == cn_Airport )
	{
		METHOD_ALLOC_FOB(result, Airport);
		return result;
	}
	if ( cname == cn_Village )
	{
		METHOD_ALLOC_FOB(result, Village);
		return result;
	}
	if ( cname == cn_Fort )
	{
		METHOD_ALLOC_FOB(result, Fort);
		return result;
	}
	if ( cname == cn_Capital )
	{
		METHOD_ALLOC_FOB(result, Capital);
		return result;
	}
	if ( cname == cn_BridgeH )
	{
		METHOD_ALLOC_FOB(result, BridgeH);
		return result;
	}
	if ( cname == cn_BridgeV )
	{
		METHOD_ALLOC_FOB(result, BridgeV);
		return result;
	}
	if ( cname == cn_MoveUnit )
	{
		METHOD_ALLOC_FOB(result, MoveUnit);
		return result;
	}
	if ( cname == cn_SupplyUnit )
	{
		METHOD_ALLOC_FOB(result, SupplyUnit);
		return result;
	}
	if ( cname == cn_Mission )
	{
		METHOD_ALLOC_FOB(result, Mission);
		return result;
	}
	if ( cname == cn_Eskadra )
	{
		METHOD_ALLOC_FOB(result, Eskadra);
		return result;
	}
	if ( cname == cn_Scaning )
	{
		METHOD_ALLOC_FOB(result, Scaning);
		return result;
	}
	if ( cname == cn_Bombard )
	{
		METHOD_ALLOC_FOB(result, Bombard);
		return result;
	}
	if ( cname == cn_FrontAttack )
	{
		METHOD_ALLOC_FOB(result, FrontAttack);
		return result;
	}
	if ( cname == cn_FrontHelp )
	{
		METHOD_ALLOC_FOB(result, FrontHelp);
		return result;
	}
	if ( cname == cn_Land )
	{
		METHOD_ALLOC_FOB(result, Land);
		return result;
	}
	if ( cname == cn_FlyAttack )
	{
		METHOD_ALLOC_FOB(result, FlyAttack);
		return result;
	}
	if ( cname == cn_GameData )
	{
		METHOD_ALLOC_FOB(result, GameData);
		return result;
	}
}
void fobSerialize(fobElement* fob, FILE* out)
{
	const char *cname = fob->cname;
	if ( cname == cn_Configuration )
	{
		StConfiguration* fe = (StConfiguration*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Configuration);
		METHOD_SERIALIZE_BYTE(supplyObjectsMax, out);
		METHOD_SERIALIZE_BYTE(supplyRadius, out);
		METHOD_SERIALIZE_BYTE(supplyTick, out);
		METHOD_SERIALIZE_BYTE(supplyStore, out);
		METHOD_SERIALIZE_BYTE(supplyRepairTick, out);
		METHOD_SERIALIZE_BYTE(tickTime, out);
		METHOD_SERIALIZE_STRING(flySeen, out);
		METHOD_SERIALIZE_BYTE(factoryRadius, out);
		METHOD_SERIALIZE_BYTE(factoryStore, out);
		METHOD_SERIALIZE_BYTE(factoryTick, out);
		METHOD_SERIALIZE_BYTE(factoryRepairTick, out);
		METHOD_SERIALIZE_BYTE(cityWorkersRadius, out);
		METHOD_SERIALIZE_BYTE(citySupplyTick, out);
		METHOD_SERIALIZE_BYTE(citySypplyRadius, out);
		METHOD_SERIALIZE_BYTE(cityRepairTick, out);
		METHOD_SERIALIZE_BYTE(airportFlyMax, out);
		METHOD_SERIALIZE_BYTE(airportSupplyMax, out);
		METHOD_SERIALIZE_BYTE(airportRepairTick, out);
		METHOD_SERIALIZE_BYTE(villageSupplyTick, out);
		METHOD_SERIALIZE_BYTE(villageSupplyRadius, out);
		METHOD_SERIALIZE_BYTE(villageRepairTick, out);
		METHOD_SERIALIZE_BYTE(fortRepairTick, out);
		METHOD_SERIALIZE_BYTE(capitalRepaitTick, out);
		METHOD_SERIALIZE_FOB_END(out, Configuration);
		return;
	}
	if ( cname == cn_ValuePunkt )
	{
		StValuePunkt* fe = (StValuePunkt*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, ValuePunkt);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, ValuePunkt);
		return;
	}
	if ( cname == cn_Fly )
	{
		StFly* fe = (StFly*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Fly);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_STRING(pilotName, out);
		METHOD_SERIALIZE_BYTE(pilotExp, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_BYTE(fuel, out);
		METHOD_SERIALIZE_FOB_END(out, Fly);
		return;
	}
	if ( cname == cn_Bomber )
	{
		StBomber* fe = (StBomber*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Bomber);
		METHOD_SERIALIZE_BYTE(bomb, out);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_STRING(pilotName, out);
		METHOD_SERIALIZE_BYTE(pilotExp, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_BYTE(fuel, out);
		METHOD_SERIALIZE_FOB_END(out, Bomber);
		return;
	}
	if ( cname == cn_Fighter )
	{
		StFighter* fe = (StFighter*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Fighter);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_STRING(pilotName, out);
		METHOD_SERIALIZE_BYTE(pilotExp, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_BYTE(fuel, out);
		METHOD_SERIALIZE_FOB_END(out, Fighter);
		return;
	}
	if ( cname == cn_Build )
	{
		StBuild* fe = (StBuild*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Build);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Build);
		return;
	}
	if ( cname == cn_Supply )
	{
		StSupply* fe = (StSupply*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Supply);
		METHOD_SERIALIZE_TABLE_FOB(dest, out);
		METHOD_SERIALIZE_BYTE(tick, out);
		METHOD_SERIALIZE_BYTE(store, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Supply);
		return;
	}
	if ( cname == cn_Factory )
	{
		StFactory* fe = (StFactory*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Factory);
		METHOD_SERIALIZE_BYTE(tick, out);
		METHOD_SERIALIZE_TABLE_FOB(store, out);
		METHOD_SERIALIZE_TABLE_FOB(dest, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Factory);
		return;
	}
	if ( cname == cn_City )
	{
		StCity* fe = (StCity*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, City);
		METHOD_SERIALIZE_BYTE(tick, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, City);
		return;
	}
	if ( cname == cn_Airport )
	{
		StAirport* fe = (StAirport*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Airport);
		METHOD_SERIALIZE_TABLE_FOB(fly, out);
		METHOD_SERIALIZE_BYTE(store, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Airport);
		return;
	}
	if ( cname == cn_Village )
	{
		StVillage* fe = (StVillage*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Village);
		METHOD_SERIALIZE_BYTE(tick, out);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Village);
		return;
	}
	if ( cname == cn_Fort )
	{
		StFort* fe = (StFort*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Fort);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Fort);
		return;
	}
	if ( cname == cn_Capital )
	{
		StCapital* fe = (StCapital*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Capital);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Capital);
		return;
	}
	if ( cname == cn_BridgeH )
	{
		StBridgeH* fe = (StBridgeH*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, BridgeH);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, BridgeH);
		return;
	}
	if ( cname == cn_BridgeV )
	{
		StBridgeV* fe = (StBridgeV*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, BridgeV);
		METHOD_SERIALIZE_BYTE(live, out);
		METHOD_SERIALIZE_BYTE(repairTick, out);
		METHOD_SERIALIZE_BYTE(flag, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, BridgeV);
		return;
	}
	if ( cname == cn_MoveUnit )
	{
		StMoveUnit* fe = (StMoveUnit*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, MoveUnit);
		METHOD_SERIALIZE_INT(identifier, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, MoveUnit);
		return;
	}
	if ( cname == cn_SupplyUnit )
	{
		StSupplyUnit* fe = (StSupplyUnit*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, SupplyUnit);
		METHOD_SERIALIZE_BYTE(destx, out);
		METHOD_SERIALIZE_BYTE(desty, out);
		METHOD_SERIALIZE_FOB(destbuild, out);
		METHOD_SERIALIZE_INT(identifier, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, SupplyUnit);
		return;
	}
	if ( cname == cn_Mission )
	{
		StMission* fe = (StMission*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Mission);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_FOB_END(out, Mission);
		return;
	}
	if ( cname == cn_Eskadra )
	{
		StEskadra* fe = (StEskadra*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Eskadra);
		METHOD_SERIALIZE_TABLE_FOB(fly, out);
		METHOD_SERIALIZE_TABLE_FOB(mission, out);
		METHOD_SERIALIZE_INT(identifier, out);
		METHOD_SERIALIZE_UINT(x, out);
		METHOD_SERIALIZE_UINT(y, out);
		METHOD_SERIALIZE_INT(value, out);
		METHOD_SERIALIZE_FOB_END(out, Eskadra);
		return;
	}
	if ( cname == cn_Scaning )
	{
		StScaning* fe = (StScaning*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Scaning);
		METHOD_SERIALIZE_BYTE(x, out);
		METHOD_SERIALIZE_BYTE(y, out);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_FOB_END(out, Scaning);
		return;
	}
	if ( cname == cn_Bombard )
	{
		StBombard* fe = (StBombard*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Bombard);
		METHOD_SERIALIZE_BYTE(x, out);
		METHOD_SERIALIZE_BYTE(y, out);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_FOB_END(out, Bombard);
		return;
	}
	if ( cname == cn_FrontAttack )
	{
		StFrontAttack* fe = (StFrontAttack*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, FrontAttack);
		METHOD_SERIALIZE_BYTE(x, out);
		METHOD_SERIALIZE_BYTE(y, out);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_FOB_END(out, FrontAttack);
		return;
	}
	if ( cname == cn_FrontHelp )
	{
		StFrontHelp* fe = (StFrontHelp*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, FrontHelp);
		METHOD_SERIALIZE_BYTE(x, out);
		METHOD_SERIALIZE_BYTE(y, out);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_FOB_END(out, FrontHelp);
		return;
	}
	if ( cname == cn_Land )
	{
		StLand* fe = (StLand*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, Land);
		METHOD_SERIALIZE_BYTE(x, out);
		METHOD_SERIALIZE_BYTE(y, out);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_FOB_END(out, Land);
		return;
	}
	if ( cname == cn_FlyAttack )
	{
		StFlyAttack* fe = (StFlyAttack*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, FlyAttack);
		METHOD_SERIALIZE_INT(identifier, out);
		METHOD_SERIALIZE_BYTE(type, out);
		METHOD_SERIALIZE_FOB_END(out, FlyAttack);
		return;
	}
	if ( cname == cn_GameData )
	{
		StGameData* fe = (StGameData*)fob;
		METHOD_SERIALIZE_FOB_BEGIN(out, GameData);
		METHOD_SERIALIZE_STRING(title, out);
		METHOD_SERIALIZE_STRING(description, out);
		METHOD_SERIALIZE_BYTE(mapwidth, out);
		METHOD_SERIALIZE_BYTE(mapheight, out);
		METHOD_SERIALIZE_STRING(terrain, out);
		METHOD_SERIALIZE_STRING(front, out);
		METHOD_SERIALIZE_TABLE_FOB(build, out);
		METHOD_SERIALIZE_TABLE_FOB(fly, out);
		METHOD_SERIALIZE_FOB_END(out, GameData);
		return;
	}
}
fobElement *fobDeserialize(xmlNode* node)
{
	METHOD_DESERIALIZE_FOB_BEGIN(node, Configuration)
		METHOD_DESERIALIZE_BYTE(supplyObjectsMax);
		METHOD_DESERIALIZE_BYTE(supplyRadius);
		METHOD_DESERIALIZE_BYTE(supplyTick);
		METHOD_DESERIALIZE_BYTE(supplyStore);
		METHOD_DESERIALIZE_BYTE(supplyRepairTick);
		METHOD_DESERIALIZE_BYTE(tickTime);
		METHOD_DESERIALIZE_STRING(flySeen);
		METHOD_DESERIALIZE_BYTE(factoryRadius);
		METHOD_DESERIALIZE_BYTE(factoryStore);
		METHOD_DESERIALIZE_BYTE(factoryTick);
		METHOD_DESERIALIZE_BYTE(factoryRepairTick);
		METHOD_DESERIALIZE_BYTE(cityWorkersRadius);
		METHOD_DESERIALIZE_BYTE(citySupplyTick);
		METHOD_DESERIALIZE_BYTE(citySypplyRadius);
		METHOD_DESERIALIZE_BYTE(cityRepairTick);
		METHOD_DESERIALIZE_BYTE(airportFlyMax);
		METHOD_DESERIALIZE_BYTE(airportSupplyMax);
		METHOD_DESERIALIZE_BYTE(airportRepairTick);
		METHOD_DESERIALIZE_BYTE(villageSupplyTick);
		METHOD_DESERIALIZE_BYTE(villageSupplyRadius);
		METHOD_DESERIALIZE_BYTE(villageRepairTick);
		METHOD_DESERIALIZE_BYTE(fortRepairTick);
		METHOD_DESERIALIZE_BYTE(capitalRepaitTick);
	METHOD_DESERIALIZE_FOB_END(node, Configuration);
	METHOD_DESERIALIZE_FOB_BEGIN(node, ValuePunkt)
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, ValuePunkt);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Fly)
		METHOD_DESERIALIZE_BYTE(type);
		METHOD_DESERIALIZE_STRING(pilotName);
		METHOD_DESERIALIZE_BYTE(pilotExp);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_BYTE(fuel);
	METHOD_DESERIALIZE_FOB_END(node, Fly);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Bomber)
		METHOD_DESERIALIZE_BYTE(bomb);
		METHOD_DESERIALIZE_BYTE(type);
		METHOD_DESERIALIZE_STRING(pilotName);
		METHOD_DESERIALIZE_BYTE(pilotExp);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_BYTE(fuel);
	METHOD_DESERIALIZE_FOB_END(node, Bomber);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Fighter)
		METHOD_DESERIALIZE_BYTE(type);
		METHOD_DESERIALIZE_STRING(pilotName);
		METHOD_DESERIALIZE_BYTE(pilotExp);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_BYTE(fuel);
	METHOD_DESERIALIZE_FOB_END(node, Fighter);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Build)
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Build);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Supply)
		METHOD_DESERIALIZE_TABLE_FOB(dest);
		METHOD_DESERIALIZE_BYTE(tick);
		METHOD_DESERIALIZE_BYTE(store);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Supply);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Factory)
		METHOD_DESERIALIZE_BYTE(tick);
		METHOD_DESERIALIZE_TABLE_FOB(store);
		METHOD_DESERIALIZE_TABLE_FOB(dest);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Factory);
	METHOD_DESERIALIZE_FOB_BEGIN(node, City)
		METHOD_DESERIALIZE_BYTE(tick);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, City);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Airport)
		METHOD_DESERIALIZE_TABLE_FOB(fly);
		METHOD_DESERIALIZE_BYTE(store);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Airport);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Village)
		METHOD_DESERIALIZE_BYTE(tick);
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Village);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Fort)
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Fort);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Capital)
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Capital);
	METHOD_DESERIALIZE_FOB_BEGIN(node, BridgeH)
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, BridgeH);
	METHOD_DESERIALIZE_FOB_BEGIN(node, BridgeV)
		METHOD_DESERIALIZE_BYTE(live);
		METHOD_DESERIALIZE_BYTE(repairTick);
		METHOD_DESERIALIZE_BYTE(flag);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, BridgeV);
	METHOD_DESERIALIZE_FOB_BEGIN(node, MoveUnit)
		METHOD_DESERIALIZE_INT(identifier);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, MoveUnit);
	METHOD_DESERIALIZE_FOB_BEGIN(node, SupplyUnit)
		METHOD_DESERIALIZE_BYTE(destx);
		METHOD_DESERIALIZE_BYTE(desty);
		METHOD_DESERIALIZE_FOB(destbuild);
		METHOD_DESERIALIZE_INT(identifier);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, SupplyUnit);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Mission)
		METHOD_DESERIALIZE_BYTE(type);
	METHOD_DESERIALIZE_FOB_END(node, Mission);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Eskadra)
		METHOD_DESERIALIZE_TABLE_FOB(fly);
		METHOD_DESERIALIZE_TABLE_FOB(mission);
		METHOD_DESERIALIZE_INT(identifier);
		METHOD_DESERIALIZE_UINT(x);
		METHOD_DESERIALIZE_UINT(y);
		METHOD_DESERIALIZE_INT(value);
	METHOD_DESERIALIZE_FOB_END(node, Eskadra);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Scaning)
		METHOD_DESERIALIZE_BYTE(x);
		METHOD_DESERIALIZE_BYTE(y);
		METHOD_DESERIALIZE_BYTE(type);
	METHOD_DESERIALIZE_FOB_END(node, Scaning);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Bombard)
		METHOD_DESERIALIZE_BYTE(x);
		METHOD_DESERIALIZE_BYTE(y);
		METHOD_DESERIALIZE_BYTE(type);
	METHOD_DESERIALIZE_FOB_END(node, Bombard);
	METHOD_DESERIALIZE_FOB_BEGIN(node, FrontAttack)
		METHOD_DESERIALIZE_BYTE(x);
		METHOD_DESERIALIZE_BYTE(y);
		METHOD_DESERIALIZE_BYTE(type);
	METHOD_DESERIALIZE_FOB_END(node, FrontAttack);
	METHOD_DESERIALIZE_FOB_BEGIN(node, FrontHelp)
		METHOD_DESERIALIZE_BYTE(x);
		METHOD_DESERIALIZE_BYTE(y);
		METHOD_DESERIALIZE_BYTE(type);
	METHOD_DESERIALIZE_FOB_END(node, FrontHelp);
	METHOD_DESERIALIZE_FOB_BEGIN(node, Land)
		METHOD_DESERIALIZE_BYTE(x);
		METHOD_DESERIALIZE_BYTE(y);
		METHOD_DESERIALIZE_BYTE(type);
	METHOD_DESERIALIZE_FOB_END(node, Land);
	METHOD_DESERIALIZE_FOB_BEGIN(node, FlyAttack)
		METHOD_DESERIALIZE_INT(identifier);
		METHOD_DESERIALIZE_BYTE(type);
	METHOD_DESERIALIZE_FOB_END(node, FlyAttack);
	METHOD_DESERIALIZE_FOB_BEGIN(node, GameData)
		METHOD_DESERIALIZE_STRING(title);
		METHOD_DESERIALIZE_STRING(description);
		METHOD_DESERIALIZE_BYTE(mapwidth);
		METHOD_DESERIALIZE_BYTE(mapheight);
		METHOD_DESERIALIZE_STRING(terrain);
		METHOD_DESERIALIZE_STRING(front);
		METHOD_DESERIALIZE_TABLE_FOB(build);
		METHOD_DESERIALIZE_TABLE_FOB(fly);
	METHOD_DESERIALIZE_FOB_END(node, GameData);
}
