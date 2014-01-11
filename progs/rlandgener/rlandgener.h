#ifndef _RLANDGENER_H_
#define _RLANDGENER_H_
#include <okienkoc/okienkoc.h>

typedef struct stConfiguration {
	int maxx;
	int maxy;
	int maxz;
	int margines;
	int minLandMass;
	int maxLandMass;
} stConfiguration;

typedef struct stChange
{
	int x;
	int y;
	int v;
} stChange;

typedef struct stContext {
	stConfiguration configuration;
	GOC_HANDLER maska;
	GOC_HANDLER mapa;
} stContext;

stChange* randomLandMassStartPoint(
	int code,
	int *pAllowedCode,
	int nAllowedCode,
	int numOfTries);

stChange *mallocPoint(int x, int y, int v);
int isInCodeSet(
	int code,
	int *pCodeSet,
	int nCodeSet);
#endif
