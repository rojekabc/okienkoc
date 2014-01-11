#ifndef _GOC_CHAR_H_
#define _GOC_CHAR_H_

#include "global.h"

typedef struct GOC_StChar
{
	GOC_COLOR color;
	char code;
} GOC_StChar;

#define GOC_STRUCT_POINT \
	unsigned int x;\
	unsigned int y

typedef struct GOC_StPoint
{
	GOC_STRUCT_POINT;
} GOC_StPoint;

#define GOC_STRUCT_AREA \
	unsigned int x; \
	unsigned int y; \
	unsigned int width; \
	unsigned int height

typedef struct GOC_StArea
{
	GOC_STRUCT_AREA;
} GOC_StArea;

typedef struct GOC_StFillArea
{
	GOC_STRUCT_AREA;
	GOC_StChar *pElement;
} GOC_StFillArea;

#define goc_areaIsInside(_obszar_, _punkt_) \
	(( _punkt_.x >= _obszar_.x ) \
	&& ( _punkt_.x < _obszar_.x + _obszar_.width ) \
	&& ( _punkt_.y >= _obszar_.y ) \
	&& ( _punkt_.y < _obszar_.y + _obszar_.height ))

#endif // _GOC_CHAR_H_
