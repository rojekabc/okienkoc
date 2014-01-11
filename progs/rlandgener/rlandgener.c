#include <okienkoc/okienkoc.h>
#include <okienkoc/tablica.h>
#include <okienkoc/mystr.h>
#include <stdlib.h>
#include <time.h>
#include <okienkoc/random.h>
#include <okienkoc/arguments.h>
#define GOC_PRINTINFO
#define GOC_PRINTERROR
#define GOC_INFO
#include <okienkoc/log.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

int MAXX=200;
int MAXY=100;
int MAXZ=9;
int MARGINES=10;
int minlandMass = 10;
int maxlandMass = 60;

typedef struct stChange
{
	int x;
	int y;
	int v;
} stChange;

static stChange *mallocPoint(int x, int y, int v)
{
	stChange *tmp = malloc(sizeof(stChange));
	tmp->x = x;
	tmp->y = y;
	tmp->v = v;
	return tmp;
}

stChange **randomStartPoints(int *nStartPoints, int n)
{
	stChange **pStartPoints = NULL;
	int i;
	int x, y, z;

	//srand(time(0));//inicjujLosuj();

	// utworzenie punktow startowych
	for (i=0; i<n; i++ )
	{
		pStartPoints = goc_tableAdd(pStartPoints, nStartPoints,
			sizeof(stChange*));
		z = goc_random((MAXZ+1));
		// zapewnienie przestrzeni na głębokość
		while ( ((x = goc_random((MAXX+1))) < z) || (x>MAXX-z) );
		while ( ((y = goc_random((MAXY+1))) < z) || (y>MAXY-z) );
		pStartPoints[*nStartPoints-1] = mallocPoint(x, y, z);
//				losuj(0, (MAXX + 1)),
//				losuj(0, (MAXY + 1)),
//				losuj(0, (MAXZ + 1)));
	}
	return pStartPoints;
}

stChange **generateStartPoints(int *nStartPoints)
{
	stChange **pStartPoints = NULL;
	// utworzenie punktow startowych
	pStartPoints = goc_tableAdd(pStartPoints, nStartPoints,
		sizeof(stChange*));
	pStartPoints[*nStartPoints-1] = mallocPoint(20, 12, 9);
	pStartPoints = goc_tableAdd(pStartPoints, nStartPoints,
		sizeof(stChange*));
	pStartPoints[*nStartPoints-1] = mallocPoint(12, 10, 9);
	pStartPoints = goc_tableAdd(pStartPoints, nStartPoints,
		sizeof(stChange*));
	pStartPoints[*nStartPoints-1] = mallocPoint(34, 7, 5);
	return pStartPoints;
}

// dla petla Count wychodza calkiem ladne okregi
// dodatkowo metoda wydaje sie szybsza

// wyznaczenie punktow, na podstawie zastosowania okresolnego wzoru
// do kazdego punktu maski
// okregi wokol punktow wzrostu, czy raczej stozki
// dlugosc wyliczen uzalezniona od rozmiaru planszy i liczby punktow
// startowych
void petlaCount(GOC_HANDLER maska, const stChange **pStartPoints, int *nStartPoints)
{
	int i, j, k;
	int o, v;
	stChange *tmp;

	// rozpoczecie wykonywania obliczen
	for ( j=0; j<MAXX; j++ )
	{
		for ( k=0; k<MAXY; k++ )
		{
			v = 0;
			for ( i=0; i<*nStartPoints; i++ )
			{
				// wyznacz odleglosc punktu od punktu startowego
				// z zasady pitagorasa + lekkie zaokraglenie
				tmp = pStartPoints[i];
				o = tmp->x > j
					? (tmp->x - j)*(tmp->x - j)
					: (j - tmp->x)*(j - tmp->x);
				o += tmp->y > k
					? (tmp->y - k)*(tmp->y - k)
					: (k - tmp->y)*(k - tmp->y);
				// 0.4 dla 'zaokraglenia'
				o = (int)(sqrt((double)o)+0.4);
				// na zasadzie spadku liniowego
				if ( o > tmp->v )
					o = 0;
				else
					o = tmp->v - o;
				if ( o > v )
					v = o;
			}
			goc_maparawSetPoint(maska, j, k, v);
		}
	}
}

void randomLandSeed(GOC_HANDLER maska, int cnt)
{
	int i;
	for ( i=0; i<cnt; i++ )
	{
		goc_maskSet(maska, 20, 12, 9);
		goc_maskSet(maska, 18, 11, 9);
	}
}

/*
 * Wykonuje przebieg poprzez kolejne linie i sprawdza otoczenie punktu,
 * probujac okreslic punkt, ktory postawi - tendencja opadajaca
 * Przebieg wykonywany jest podana liczbe razy
 */
void petlaGrow(GOC_HANDLER maska, int cnt)
{
	stChange **pZmiany = NULL;
	int nZmiany = 0;
	int i, j, k, v, n, t;

	// ustawienie poczatkowych punktow
	randomLandSeed(maska, 1);
	
	for ( i=0; i<cnt; i++ )
	{
		for ( j=0; j<MAXX; j++ )
			for ( k=0; k<MAXY; k++ )
			{
				fprintf(stderr, "[i,j,k]=[%d,%d,%d]\n",i,j,k);
				v = goc_maparawGetPoint(maska, j, k);
				if ( v == 0 )
				{
					n = 0;
					// sprawdz otoczenie
					fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
					if ( j != 39 )
						t = goc_maparawGetPoint(maska, (j+1), k);
					else
						t = 0;
					if ( t )
					{
						v += t;
						n++;
					}
					fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
					if ( j != 0 )
						t = goc_maparawGetPoint(maska, (j-1), k);
					else
						t = 0;
					if ( t )
					{
						v += t;
						n++;
					}
					fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
					if ( k != 22 )
						t = goc_maparawGetPoint(maska, j, (k+1));
					else
						t = 0;
					if ( t )
					{
						v += t;
						n++;
					}
					fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
					if ( k != 0 )
						t = goc_maparawGetPoint(maska, j, (k-1));
					else
						t = 0;
					if ( t )
					{
						v += t;
						n++;
					}
					fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
					if ( n )
					{
						// zgromadz zmiany
						stChange *x = malloc(sizeof(stChange));
						x->x = j;
						x->y = k;
						v /= n;
						v--;
						x->v = v;
						pZmiany = goc_tableAdd(
							pZmiany,
							&nZmiany,
							sizeof(stChange*));
						pZmiany[nZmiany-1] = x;
					}
				}
			}
		// zastosuj zmiany
		for ( j = 0; j<nZmiany; j++ )
			goc_maparawSetPoint(maska, pZmiany[j]->x, pZmiany[j]->y,
				pZmiany[j]->v);

		// czysc tablice zmian
		for ( j=0; j<nZmiany; j++ )
			free(pZmiany[j]);
		
		pZmiany = goc_tableClear(pZmiany, &nZmiany);

	}
}

// argument point has a pointer for input/output data of point
// argument type 1 random a point as neighbour of point
// argument type 0 random a point on a map
// argument minLevel tells a level of point on which we can set new point
// function check the level of random point - it's posiible to put there a point
// return 1 if positive random
// return 0 if negative random
int randomPoint(stChange* point, int type, int minLevel, GOC_HANDLER mapa)
{
	int safe;
	if ( type == 0 )
	{
		safe = 30;
		while ( safe-- )
		{
			int x = point->x = goc_random(MAXX);
			int y = point->y = goc_random(MAXY);
			if (( x < 0 + MARGINES ) || ( x >= MAXX - MARGINES ))
				continue;
			if (( y < 0 + MARGINES ) || ( y >= MAXY - MARGINES ))
				continue;
			if ( goc_maparawGetPoint(mapa, point->x, point->y) == minLevel )
			{
				int v;
				if ( x+1 < MAXX )
				{
					v = goc_maparawGetPoint(mapa, x+1, y);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				if ( y+1 < MAXY )
				{
					v = goc_maparawGetPoint(mapa, x, y+1);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				if ( y > 0 )
				{
					v = goc_maparawGetPoint(mapa, x, y-1);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				if ( x > 0 )
				{
					v = goc_maparawGetPoint(mapa, x-1, y);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				return 1;
			}
		}
	}
	else if ( type == 1 )
	{
		safe = 3;
		while ( safe-- )
		{
			// RANDOM ONLY in 4 base direction
			int r = goc_random(4);
			int x, y;
			switch ( r )
			{
				case 0:
					point->x++;
					break;
				case 1:
					point->x--;
					break;
				case 2:
					point->y++;
					break;
				case 3:
					point->y--;
					break;
			}
			x = point->x;
			y = point->y;
			// RANDOM in all 8 direction
			// int x = point->x = point->x + goc_random(3) - 1;
			// int y = point->y = point->y + goc_random(3) - 1;

			if (( point->x < 0 ) || ( point->x >= MAXX ))
				continue;
			if (( point->y < 0 ) || ( point->y >= MAXY ))
				continue;
			if ( goc_maparawGetPoint(mapa, point->x, point->y) == minLevel )
			{
				int v;
				if ( x+1 < MAXX )
				{
					v = goc_maparawGetPoint(mapa, x+1, y);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				if ( y+1 < MAXY )
				{
					v = goc_maparawGetPoint(mapa, x, y+1);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				if ( y > 0 )
				{
					v = goc_maparawGetPoint(mapa, x, y-1);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				if ( x > 0 )
				{
					v = goc_maparawGetPoint(mapa, x-1, y);
					if ( v < minLevel )
						continue;
				}
				else if ( minLevel != 0 )
					continue;
				return 1;
			}
		}
	}
	return 0;
}

static int precentageCut[10] = {0, 0, 1, 2, 9, 3, 7, 7, 8, 9};

// TODO : random landMass from min to max
// TODO : putting next landMass level for height
static void civGeneration(GOC_HANDLER mapa, int numStartPoints, int minlandMass, int maxlandMass, int MINLEVEL, int NEWLEVEL)
{
//	int MINLEVEL=0;
//	int NEWLEVEL=1;
	int type = 0;
	stChange point;
	point.x = 0;
	point.y = 0;
	point.v = 0;
	stChange** pSet = NULL;
	int nSet = 0;
	int landMass;
	int svStartPoints = numStartPoints;
	int createdLandMass;

	GOC_BINFO("Start civ generation system [numStartPoint: %d, minlandMass: %d, maxlandMass: %d, level: %d]", numStartPoints, minlandMass, maxlandMass, NEWLEVEL);

	if ( MINLEVEL >= 10 )
		return;
	if ( numStartPoints <= 0 )
		return;
	if ( maxlandMass <= 0 )
		return;


	while (numStartPoints--)
	{
		GOC_BDEBUG("Number of start points %d", numStartPoints);
		// inicjowanie do kolejnej iteracji tworzenia masy ladowej
		type = 0;
		landMass = minlandMass + goc_random(maxlandMass-minlandMass);
		createdLandMass = 0;
		GOC_BINFO("Generated landmass: %d", landMass);
		if ( nSet )
		{
			GOC_DEBUG("Clear set");
			int i;
			for ( i=0; i<nSet; i++ )
				free(pSet[i]);
			pSet = goc_tableClear(pSet, &nSet);
		}
		while ( landMass )
		{
			GOC_BDEBUG("Landmass to generate %d", landMass);
			// random new point
			if ( randomPoint(&point, type, MINLEVEL, mapa) )
			{
				GOC_BDEBUG("Setting a point (%d,%d,%d)", point.x, point.y, NEWLEVEL);
				pSet = goc_tableAdd(pSet, &nSet, sizeof(void*));
				pSet[nSet-1] = mallocPoint(point.x, point.y, NEWLEVEL);
				goc_maparawSetPoint(mapa, point.x, point.y, NEWLEVEL);
				createdLandMass++;
			}
			landMass--;
			while ( nSet )
			{
				// random point from set
				int randp = goc_random(nSet);
				int x, y;
				type = 1; // neighbour
				// check a point has any free neighbour
				x = pSet[randp]->x;
				y = pSet[randp]->y;
				if (
					( (x+1 < MAXX) && (goc_maparawGetPoint(mapa, x+1, y) <= MINLEVEL) ) ||
					( (y+1 < MAXY) && (goc_maparawGetPoint(mapa, x, y+1) <= MINLEVEL )) ||
					( (y > 0) && (goc_maparawGetPoint(mapa, x, y-1) <= MINLEVEL) ) ||
					( (x > 0) && (goc_maparawGetPoint(mapa, x-1, y) <= MINLEVEL) )
				)
				{
					point.x = x;
					point.y = y;
					point.v = pSet[randp]->v;
					GOC_BDEBUG("Find point in set (%d,%d,%d)", point.x, point.y, point.v);
					break;
				}
				else
				{
					GOC_DEBUG("Remove point from set");
					free(pSet[randp]);
					pSet = goc_tableRemove(pSet, &nSet, sizeof(void*), randp);
				}
			}
			// check, if there are any points in set
			if ( !nSet )
				break;
		}
		GOC_BINFO("Created landmass: %d", createdLandMass);
	}
	civGeneration(
		mapa,
		svStartPoints, //-svStartPoints/20,
		minlandMass-minlandMass*precentageCut[MINLEVEL]/10,
		maxlandMass-maxlandMass*precentageCut[MINLEVEL]/10,
		MINLEVEL+1,
		NEWLEVEL+1);
	/*
	pStartPoint = randomStartPoints(&nStartPoint, numStartPoints);
	for (i=0; i<nStartPoint; i++)
	{
		goc_maparawSetPoint(mapa, pStartPoint[i]->x, pStartPoint[i]->y, 1);
	}
	*/
}

// mapa - uchwyt do generowanej mapy
// numStartPoints - liczba startowych biom-ow
// minlandMass / maxlandMass - obszar losowany miedzy tymi zmiennymi masy generowanego biomu
// TODO: Ustawienie poziomów losowania każdego z biom-ów
// TODO: Ustawienie poziomów rozprzestrzeniania się poziomów
// TODO: Może warto ustawić startowe punkty biom-ów morza
// TODO: Może warto zastanowić się nad zwalczaniem się poszczególnych biomów między sobą, albo możliwością ich zbyt bliskiego
// istnienia obok siebie
stChange** pBiomeSet = NULL;
int nBiomeSet = 0;
static void bacteryGeneration(GOC_HANDLER mapa, int numStartPoints, int minlandMass, int maxlandMass, int nBiomes)
{
	int i;
	// wygenerowanie punktów startowych biomów
	MAXZ = nBiomes-1;
	pBiomeSet = randomStartPoints(&nBiomeSet, numStartPoints);
	for ( i=0; i<nBiomeSet; i++ )
	{
		goc_maparawSetPoint(mapa,
			pBiomeSet[i]->x, pBiomeSet[i]->y, ++(pBiomeSet[i]->v));
	}
	goc_systemSendMsg(mapa, GOC_MSG_PAINT, 0, 0);
}


// TODO: Random start point
// TODO: Generate around start point in selected radius with kind of gauss propability or
// kond of selected propability
static void rainGeneration(GOC_HANDLER mapa, int numStartPoints, int minlandMass, int maxlandMass)
{
	int landMass = minlandMass + goc_random(maxlandMass-minlandMass);
	stChange point;
	point.x = 0;
	point.y = 0;
	point.v = 0;

	GOC_BINFO("Raind landmass %d", landMass);
	while ( landMass-- )
	{
		int x = point.x = goc_random(MAXX);
		int y = point.y = goc_random(MAXY);
		int v = goc_maparawGetPoint(mapa, point.x, point.y);
		if ( v >= 9 )
			continue;
		goc_maparawSetPoint(mapa, point.x, point.y, v+1);
	}
}

#define MODE_VIEW_NUMBERS	0
#define MODE_VIEW_CHARS		1
#define MODE_VIEW_BIOMES	2

static short modeView = MODE_VIEW_NUMBERS;
GOC_HANDLER maska;
GOC_HANDLER mapa;

static void setUpNumericModeView()
{
		modeView = MODE_VIEW_NUMBERS;
		// zestaw numeryczny
		// none
		goc_maskSetValue(maska, 0, '.', GOC_WHITE);
		// sea
		goc_maskSetValue(maska, 1, '0', GOC_BLUE);
		goc_maskSetValue(maska, 2, '1', GOC_BLUE | GOC_FBOLD);
		goc_maskSetValue(maska, 3, '2', GOC_CYAN | GOC_FBOLD);
		// land
		goc_maskSetValue(maska, 4, '3', GOC_GREEN);
		goc_maskSetValue(maska, 5, '4', GOC_GREEN | GOC_FBOLD);
		goc_maskSetValue(maska, 6, '5', GOC_RED);
		// mountain
		goc_maskSetValue(maska, 7, '6', GOC_YELLOW);
		goc_maskSetValue(maska, 8, '7', GOC_WHITE);
		goc_maskSetValue(maska, 9, '8', GOC_WHITE | GOC_FBOLD);
}

static void setUpCharModeView()
{
		modeView = MODE_VIEW_CHARS;
		// zestaw rysunkowy
		// sea
		goc_maskSetValue(maska, 0, '~', GOC_BLUE);
		goc_maskSetValue(maska, 1, '~', GOC_BLUE);
		goc_maskSetValue(maska, 2, '~', GOC_BLUE);
		goc_maskSetValue(maska, 3, '~', GOC_BLUE);
		// landmass
		goc_maskSetValue(maska, 4, 't', GOC_GREEN);
		goc_maskSetValue(maska, 5, 't', GOC_GREEN);
		// mountain
		goc_maskSetValue(maska, 6, '^', GOC_YELLOW);
		goc_maskSetValue(maska, 7, '^', GOC_YELLOW);
		// high mountain
		goc_maskSetValue(maska, 8, '^', GOC_WHITE | GOC_FBOLD);
		goc_maskSetValue(maska, 9, '^', GOC_WHITE | GOC_FBOLD);
}

static void setUpBiomeModeView()
{
	modeView = MODE_VIEW_BIOMES;
	// none
	goc_maskSetValue(maska, 0, ' ', GOC_BLACK);
	// grassland
	goc_maskSetValue(maska, 1, '"', GOC_GREEN);
	// forest
	goc_maskSetValue(maska, 2, 't', GOC_GREEN);
	// taiga
	goc_maskSetValue(maska, 3, 't', GOC_WHITE | GOC_FBOLD);
	// hills
	goc_maskSetValue(maska, 4, '^', GOC_YELLOW);
	// sand (pustynia)
	goc_maskSetValue(maska, 5, '.', GOC_YELLOW | GOC_FBOLD);
	// sea
	goc_maskSetValue(maska, 6, '~', GOC_BLUE);
	// swamp
	goc_maskSetValue(maska, 7, '"', GOC_CYAN);
	// tundra
	goc_maskSetValue(maska, 8, '.', GOC_WHITE | GOC_FBOLD);
	// jungle
	goc_maskSetValue(maska, 9, 'T', GOC_GREEN | GOC_FBOLD);
}

static int hotkeyModeView(
	GOC_HANDLER u, GOC_MSG m, void* pBuf, unsigned int nBuf)
{
	if ( modeView == MODE_VIEW_CHARS )
	{
		setUpNumericModeView();
	}
	else if ( modeView == MODE_VIEW_NUMBERS )
	{
		setUpCharModeView();
	}
	goc_systemSendMsg(mapa, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

int nBiomeTurn = 0;
static int hotkeyNextTurnBactery(
	GOC_HANDLER u, GOC_MSG m, void* pBuf, unsigned int nBuf)
{
	int randomAtOneTurn = 100;
	int nKill = 0;
	int nBurn = 0;
	int nInvade = 0;
	GOC_BINFO("Turn %d - randoms(%d)", nBiomeTurn, randomAtOneTurn);
	nBiomeTurn++;
	while ( randomAtOneTurn-- )
	{
		if ( ! nBiomeSet )
		{
			break;
		}
		int r = goc_random(nBiomeSet);
		stChange* point = pBiomeSet[r];
		int x = point->x;
		int y = point->y;
		// czy ma zginac
		if (
			( (x+1 >= MAXX) || (goc_maparawGetPoint(mapa, x+1, y) != 0) ) &&
			( (y+1 >= MAXY) || (goc_maparawGetPoint(mapa, x, y+1) != 0 )) &&
			( (y-1 <= 0) || (goc_maparawGetPoint(mapa, x, y-1) != 0) ) &&
			( (x-1 <= 0) || (goc_maparawGetPoint(mapa, x-1, y) != 0) )
		)
		{
			nKill++;
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
		if (( x >= MAXX ) || ( y >= MAXY ) || ( x < 0 ) || ( y < 0 ))
		{
			nBurn++;
			continue;
		}
		if ( goc_maparawGetPoint(mapa, x, y) != 0 )
		{
			nBurn++;
			continue;
		}
		// wykonaj zarazenie
		{
			nInvade++;
			stChange* newpoint = mallocPoint(x, y, point->v);
			pBiomeSet = goc_tableAdd(pBiomeSet, &nBiomeSet, sizeof(void*));
			pBiomeSet[nBiomeSet-1] = newpoint;
			goc_maparawSetPoint(mapa, x, y, point->v);
			// goc_maskPaintPoint(maska, x, y); // TODO: metoda paintAreaPoint, do rysowania punktu wskazanego z danych
		}
	}
	GOC_BINFO("Dies: %3d Burns: %3d Invades: %3d", nKill, nBurn, nInvade);
	goc_systemSendMsg(mapa, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

static void generateFillUp(int code)
{
	int x;
	int y;
	GOC_BINFO("Filling up with %d", code);
	for ( x = 0; x < MAXX; x++ )
	{
		for ( y = 0; y < MAXY; y++ )
		{
			goc_maparawSetPoint(mapa, x, y, code);
		}
	}
}

static int isInCodeSet(
	int code,
	int *pCodeSet,
	int nCodeSet)
{
	int i;
	if ( !pCodeSet )
		return 0;
	for ( i=0; i<nCodeSet; i++)
	{
		if ( pCodeSet[i] == code )
			return 1;
	}
	return 0;
}

static stChange* randomLandMassStartPoint(
	int code,
	int *pAllowedCode,
	int nAllowedCode,
	int numOfTries)
{
	int x, y;
	int v;
	while ( numOfTries-- )
	{
		x = MARGINES + goc_random(MAXX + 1 - MARGINES - MARGINES);
		y = MARGINES + goc_random(MAXY + 1 - MARGINES - MARGINES);
		v = goc_maparawGetPoint(mapa, x, y);
		if ( isInCodeSet(v, pAllowedCode, nAllowedCode) )
			return mallocPoint(x, y, code);
	}
	return NULL;
}

static void generateLandMass(
	int code, // genrowany kod
	int *pAllowedCode, // dozwolona tablica kodow do zjadania
	int nAllowedCode)
{
	int landMass = minlandMass + goc_random(maxlandMass-minlandMass);
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
	goc_maparawSetPoint(mapa, point->x, point->y, point->v);
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
			( (x+1 >= MAXX) || (!isInCodeSet(goc_maparawGetPoint(mapa, x+1, y), pAllowedCode, nAllowedCode)) ) &&
			( (y+1 >= MAXY) || (!isInCodeSet(goc_maparawGetPoint(mapa, x, y+1), pAllowedCode, nAllowedCode)) ) &&
			( (y-1 <= 0) || (!isInCodeSet(goc_maparawGetPoint(mapa, x, y-1), pAllowedCode, nAllowedCode)) ) &&
			( (x-1 <= 0) || (!isInCodeSet(goc_maparawGetPoint(mapa, x-1, y), pAllowedCode, nAllowedCode)) )
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
		if (( x >= MAXX ) || ( y >= MAXY ) || ( x < 0 ) || ( y < 0 ))
		{
			continue;
		}
		if ( !isInCodeSet( goc_maparawGetPoint(mapa, x, y), pAllowedCode, nAllowedCode) )
		{
			continue;
		}
		// wykonaj zarazenie
		{
			stChange* newpoint = mallocPoint(x, y, point->v);
			pBiomeSet = goc_tableAdd(pBiomeSet, &nBiomeSet, sizeof(void*));
			pBiomeSet[nBiomeSet-1] = newpoint;
			goc_maparawSetPoint(mapa, x, y, point->v);
			landMass--;
			// goc_maskPaintPoint(maska, x, y); // TODO: metoda paintAreaPoint, do rysowania punktu wskazanego z danych
		}
	}
	pBiomeSet = goc_tableClear(pBiomeSet, &nBiomeSet);
}

#define NUMBEROFLANDS 15
#define NUMBEROFTREES 5
#define NUMBEROFHILLS 3
#define NUMBEROFDESERT 2
#define NUMBEROFSWAMP 2

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
	else if ( nBiomeTurn <= NUMBEROFLANDS )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 6;
		generateLandMass(1, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= NUMBEROFLANDS + NUMBEROFTREES )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 1;
		generateLandMass(2, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= NUMBEROFLANDS + NUMBEROFTREES + NUMBEROFHILLS )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 1;
		generateLandMass(4, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= NUMBEROFLANDS + NUMBEROFTREES + NUMBEROFHILLS + NUMBEROFDESERT )
	{
		int *allowed = malloc(sizeof(int)*1);
		allowed[0] = 1;
		generateLandMass(5, allowed, 1);
		free(allowed);
	}
	else if ( nBiomeTurn <= NUMBEROFLANDS + NUMBEROFTREES + NUMBEROFHILLS + NUMBEROFDESERT + NUMBEROFSWAMP )
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
	goc_systemSendMsg(mapa, GOC_MSG_PAINT, 0, 0);
	return GOC_ERR_OK;
}

int main(int argc, char **argv)
{
	int nStartPoints = 0;
	GOC_Arguments* args = NULL;
	char *levelChars = "signs"; // TODO: for string use stringCopy to alloc
	int numberOfStartPoints = 20;
	char* genSystem = "points";

	srand(time(NULL));

	// ustalanie argumentow
	args = goc_argumentsAdd(args,
		"-h", "Print this help", goc_argumentHelpFunction, &args);
	args = goc_argumentsAdd(args,
		"--help", "Print this help", goc_argumentHelpFunction, &args);
	args = goc_argumentsAdd(args,
		"--xsize", "Set x size of map (int:200)", goc_argumentIntFunction, &MAXX);
	args = goc_argumentsAdd(args,
		"--ysize", "Set y size of map (int:100)", goc_argumentIntFunction, &MAXY);
	args = goc_argumentsAdd(args,
		"--level", "Set type of level show (signs/numbers)", goc_argumentStringFunction, &levelChars);
	args = goc_argumentsAdd(args,
		"--points", "Number of start points [system:civ/points] (int:20)", goc_argumentIntFunction, &numberOfStartPoints);
	args = goc_argumentsAdd(args,
		"--system", "Used generation system (points/civ/rain/bactery/drops)", goc_argumentStringFunction, &genSystem);
	args = goc_argumentsAdd(args,
		"--minlandmass", "Minimum land mass of generated land [system:civ/rain] (int:10)", goc_argumentIntFunction, &minlandMass);
	args = goc_argumentsAdd(args,
		"--maxlandmass", "Maximum land mass of generated land [system:civ/rain] (int:60)", goc_argumentIntFunction, &maxlandMass);
	args = goc_argumentsAdd(args,
		"--margines", "Set margines value of random new points [system:civ/points/rain] (int:10)", goc_argumentIntFunction, &MARGINES);

	args = goc_argumentsSetUnknownFunction(args, goc_argumentHelpFunction);

	if ( goc_argumentsParse(args, argc, argv) )
		return -1;

	maska = goc_elementCreate(GOC_ELEMENT_MASK, 1, 1, 0, 0,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
		GOC_WHITE | GOC_FBOLD,
		GOC_HANDLER_SYSTEM);
	goc_maskSetRealArea(maska, MAXX, MAXY);
	mapa = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, MAXX, MAXY,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
		GOC_WHITE,
		maska);
	goc_maskAddMap(maska, mapa);
	goc_maparawSetBPV(mapa, 4);

	// maskaUstRodzajWartosci(maska, 4, 1);
	if ( goc_stringEquals(genSystem, "bactery") )
	{
		setUpBiomeModeView();
	}
	else if ( goc_stringEquals(levelChars, "numbers") )
	{
		setUpNumericModeView();
	}
	else if ( goc_stringEquals(levelChars, "signs") )
	{
		setUpCharModeView();
	}
	if ( goc_stringEquals(genSystem, "drops") )
	{
		setUpBiomeModeView();
	}


	// petlaGrow(maska, 10); // 10 bez lawirowania, tylko opadanie
//	petlaCount(maska, generateStartPoints(&nStartPoints), &nStartPoints);
	if ( goc_stringEquals( genSystem, "points" ) )
	{
		goc_hkAdd(GOC_HANDLER_SYSTEM, 'V', GOC_EFLAGA_ENABLE, hotkeyModeView);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 'v', GOC_EFLAGA_ENABLE, hotkeyModeView);
		petlaCount(mapa, randomStartPoints(&nStartPoints, numberOfStartPoints), &nStartPoints);
	}
	else if ( goc_stringEquals( genSystem, "civ" ) )
	{
		goc_hkAdd(GOC_HANDLER_SYSTEM, 'V', GOC_EFLAGA_ENABLE, hotkeyModeView);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 'v', GOC_EFLAGA_ENABLE, hotkeyModeView);
		civGeneration(mapa, numberOfStartPoints, minlandMass, maxlandMass, 0, 1);
	}
	else if ( goc_stringEquals( genSystem, "rain" ) )
	{
		goc_hkAdd(GOC_HANDLER_SYSTEM, 'V', GOC_EFLAGA_ENABLE, hotkeyModeView);
		goc_hkAdd(GOC_HANDLER_SYSTEM, 'v', GOC_EFLAGA_ENABLE, hotkeyModeView);
		rainGeneration(mapa, numberOfStartPoints, minlandMass, maxlandMass);
	}
	else if ( goc_stringEquals( genSystem, "bactery" ) )
	{
		goc_hkAdd(GOC_HANDLER_SYSTEM, ' ', GOC_EFLAGA_ENABLE, hotkeyNextTurnBactery);
		bacteryGeneration(mapa, numberOfStartPoints, minlandMass, maxlandMass, 9);
	}
	else if ( goc_stringEquals( genSystem, "drops" ) )
	{
		goc_hkAdd(GOC_HANDLER_SYSTEM, ' ', GOC_EFLAGA_ENABLE, hotkeyNextTurnDrops);
	}
		

	goc_systemFocusOn(maska);
//	goc_systemSendMsg(0, GOC_MGS_PAINT, 0, 0);
	while (goc_systemCheckMsg( NULL ));
	return 0;
}
