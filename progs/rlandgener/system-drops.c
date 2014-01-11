#include "rlandgener.h"
#include "system-drops.h"
#include <okienkoc/random.h>
// #define GOC_PRINTINFO
#define GOC_PRINTERROR
#define GOC_INFO
#include <okienkoc/log.h>

static stContext* context = NULL;
static int nBiomeTurn = 0;
struct {
	int lands;
	int trees;
	int hills;
	int desert;
	int swamp;
} biome = {15, 5, 3, 2, 2};

static void generateFillUp(int code)
{
	int x;
	int y;
	GOC_BINFO("Filling up with %d", code);
	for ( x = 0; x < context->configuration.maxx; x++ )
	{
		for ( y = 0; y < context->configuration.maxy; y++ )
		{
			goc_maparawSetPoint(context->mapa, x, y, code);
		}
	}
}



static void generateLandMass(
	int code, // genrowany kod
	int *pAllowedCode, // dozwolona tablica kodow do zjadania
	int nAllowedCode)
{
	stChange** pBiomeSet = NULL;
	int nBiomeSet = 0;
	int landMass = context->configuration.minLandMass + goc_random(context->configuration.maxLandMass-context->configuration.minLandMass);
	stChange* point = NULL;
	GOC_BINFO("Generate land %d with landmass size %d", code, landMass);
	point = randomLandMassStartPoint(code, pAllowedCode, nAllowedCode, 20);
	if ( point == NULL )
	{
		GOC_INFO("Start point of landmass not found");
		return;
	}
	pBiomeSet = goc_tableAdd(pBiomeSet, &nBiomeSet, sizeof(void*));
	pBiomeSet[nBiomeSet-1] = point;
	goc_maparawSetPoint(context->mapa, point->x, point->y, point->v);
	while ( landMass )
	{
		if ( ! nBiomeSet )
		{
			break;
		}
		int r = goc_random(nBiomeSet);
		point = pBiomeSet[r];
		int x = point->x;
		int y = point->y;
		// czy ma zginac - TODO: MARGIN
		if (
			( (x+1 >= context->configuration.maxx) || (!isInCodeSet(goc_maparawGetPoint(context->mapa, x+1, y), pAllowedCode, nAllowedCode)) ) &&
			( (y+1 >= context->configuration.maxy) || (!isInCodeSet(goc_maparawGetPoint(context->mapa, x, y+1), pAllowedCode, nAllowedCode)) ) &&
			( (y-1 <= 0) || (!isInCodeSet(goc_maparawGetPoint(context->mapa, x, y-1), pAllowedCode, nAllowedCode)) ) &&
			( (x-1 <= 0) || (!isInCodeSet(goc_maparawGetPoint(context->mapa, x-1, y), pAllowedCode, nAllowedCode)) )
		)
		{
			free(pBiomeSet[r]);
			pBiomeSet = goc_tableRemove(pBiomeSet, &nBiomeSet, sizeof(void*), r);
			continue;
		}
		// znajdz, jakie miejsce moze zarazic
		{
			// RANDOM ONLY in 4 base direction
			int r = goc_random(4);
			switch ( r )
			{
				case 0:
					x++;
					break;
				case 1:
					x--;
					break;
				case 2:
					y++;
					break;
				case 3:
					y--;
					break;
			}
		}
		// wykluczenie
		if (( x >= context->configuration.maxx ) || ( y >= context->configuration.maxy ) || ( x < 0 ) || ( y < 0 ))
		{
			continue;
		}
		if ( !isInCodeSet( goc_maparawGetPoint(context->mapa, x, y), pAllowedCode, nAllowedCode) )
		{
			continue;
		}
		// wykonaj zarazenie
		{
			stChange* newpoint = mallocPoint(x, y, point->v);
			pBiomeSet = goc_tableAdd(pBiomeSet, &nBiomeSet, sizeof(void*));
			pBiomeSet[nBiomeSet-1] = newpoint;
			goc_maparawSetPoint(context->mapa, x, y, point->v);
			landMass--;
			// goc_maskPaintPoint(maska, x, y); // TODO: metoda paintAreaPoint, do rysowania punktu wskazanego z danych
		}
	}
	pBiomeSet = goc_tableClear(pBiomeSet, &nBiomeSet);
}

static int hotkeyNextTurnDrops(
	GOC_HANDLER u, GOC_MSG m, void* pBuf, unsigned int nBuf)
{
	// TODO: Setting max and min landmass for forest/hills/land seperate
	GOC_BINFO("Turn: %3d", nBiomeTurn);
	if ( nBiomeTurn <= 0 )
	{
		// fill up with sea
		generateFillUp(6);
	}
	else if ( nBiomeTurn <= biome.lands )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 6;
		generateLandMass(1, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= biome.lands + biome.trees )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 1;
		generateLandMass(2, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= biome.lands + biome.trees + biome.hills )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 1;
		generateLandMass(4, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= biome.lands + biome.trees + biome.hills + biome.desert )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 1;
		generateLandMass(5, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= biome.lands + biome.trees + biome.hills + biome.desert + biome.swamp)
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 1;
		generateLandMass(7, allowed, 1);
		free(allowed);
	}
	else
	{
		GOC_INFO("This is the end");
		// I think it's the end
		return GOC_ERR_OK;
	}
	nBiomeTurn++;
	goc_systemSendMsg(context->mapa, GOC_MSG_PAINT, 0, 0);
	goc_gotoxy(0, 0);
	goc_textallcolor(GOC_WHITE);
	printf("%03d", nBiomeTurn);
	goc_gotoxy(0, 0);
	fflush(stdout);
	return GOC_ERR_OK;
}

void systemDropsAddArgs(GOC_Argument* args)
{
	args = goc_argumentsAdd(args,
		"--drops-land", "Set ammount of lands", goc_argumentIntFunction, &biome.lands);
	args = goc_argumentsAdd(args,
		"--drops-forest", "Set ammount of lands", goc_argumentIntFunction, &biome.trees);
	args = goc_argumentsAdd(args,
		"--drops-swamp", "Set ammount of lands", goc_argumentIntFunction, &biome.swamp);
	args = goc_argumentsAdd(args,
		"--drops-hills", "Set ammount of lands", goc_argumentIntFunction, &biome.hills);
	args = goc_argumentsAdd(args,
		"--drops-desert", "Set ammount of lands", goc_argumentIntFunction, &biome.desert);
}

void initSystemDrops(stContext* icontext)
{
	context = icontext;
	goc_hkAdd(GOC_HANDLER_SYSTEM, ' ', GOC_EFLAGA_ENABLE, hotkeyNextTurnDrops);

}
