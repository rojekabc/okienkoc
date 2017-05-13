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

#define goc_areaIsInside(_area_x_, _area_y_, _width_, _height_, _point_x_, _point_y_) \
	(( _point_x_ >= _area_x_ ) \
	&& ( _point_x_ < _area_x_ + _width_ ) \
	&& ( _point_y_ >= _area_y_ ) \
	&& ( _point_y_ < _area_y_ + _height_ ))

#endif // _GOC_CHAR_H_
