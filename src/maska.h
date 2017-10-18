#ifndef _GOC_MASK_H_
#define _GOC_MASK_H_

#include "element.h"
#include "nbitpola.h"
#include "tablica.h"
#include "znak.h"

/** Mask element id. */
extern const char* GOC_ELEMENT_MASK;

/**
 * Set map size (width/height).
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	int width;
	int height;
} GOC_StMsgMapSetSize;
#define GOC_MSG_MAPSETSIZE(variable, _width_, _height_) \
	GOC_MSG(GOC_StMsgMapSetSize, variable, GOC_MSG_MAPSETSIZE_ID); \
	variable##Full.width = _width_; \
	variable##Full.height = _height_
extern const char* GOC_MSG_MAPSETSIZE_ID;

/**
 * Get map point value.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	unsigned int x;
	unsigned int y;
	GOC_COLOR charcolor;
	char charcode;
} GOC_StMsgMapGetCharAt;
#define GOC_MSG_MAPGETCHARAT(variable, _x_, _y_) \
	GOC_MSG(GOC_StMsgMapGetCharAt, variable, GOC_MSG_MAPGETCHARAT_ID); \
	variable##Full.x = _x_; \
	variable##Full.y = _y_
extern const char* GOC_MSG_MAPGETCHARAT_ID;

 /**
  * Append char definition to map.
  * Set position to -1 if it should be add to the end.
  */
typedef struct {
	GOC_STRUCT_MESSAGE;
	GOC_COLOR charcolor;
	char charcode;
	int position;
} GOC_StMsgMapChar;
#define GOC_MSG_MAPADDCHAR(variable, _color_, _charcode_, _position_) \
	GOC_MSG(GOC_StMsgMapChar, variable, GOC_MSG_MAPADDCHAR_ID); \
	variable##Full.charcolor = _color_; \
	variable##Full.charcode = _charcode_; \
	variable##Full.position = _position_
extern const char* GOC_MSG_MAPADDCHAR_ID;


 /*
  * Set value on a map. It will be mapped to character with color.
  */
typedef struct {
	GOC_STRUCT_MESSAGE;
	unsigned int x;
	unsigned int y;
	int value;
} GOC_StMsgMapPointValue;
#define GOC_MSG_MAPSETPOINT(variable, _x_, _y_, _value_) \
	GOC_MSG(GOC_StMsgMapPointValue, variable, GOC_MSG_MAPSETPOINT_ID); \
	variable##Full.x = _x_; \
	variable##Full.y = _y_; \
	variable##Full.value = _value_
extern const char* GOC_MSG_MAPSETPOINT_ID;

/**
 * Pain part of map from point on a screen.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	unsigned int screenx;
	unsigned int screeny;
	unsigned int mapx;
	unsigned int mapy;
	unsigned int width;
	unsigned int height;
	int value;
} GOC_StMsgMapPaint;
#define GOC_MSG_MAPPAINT(variable, _screenx_, _screeny_, _mapx_, _mapy_, _width_, _height_) \
	GOC_MSG(GOC_StMsgMapPaint, variable, GOC_MSG_MAPPAINT_ID); \
	variable##Full.screenx = _screenx_; \
	variable##Full.screeny = _screeny_; \
	variable##Full.mapx = _mapx_; \
	variable##Full.mapy = _mapy_; \
	variable##Full.width = _width_; \
	variable##Full.height = _height_;
extern const char* GOC_MSG_MAPPAINT_ID;

/**
 * Get value of point on map.
 */
#define GOC_MSG_MAPGETPOINT(variable, _x_, _y_) \
	GOC_MSG(GOC_StMsgMapPointValue, variable, GOC_MSG_MAPGETPOINT_ID); \
	variable##Full.x = _x_; \
	variable##Full.y = _y_;
extern const char* GOC_MSG_MAPGETPOINT_ID;

/**
 * Get char to paint on selected position and with given value.
 */
#define GOC_MSG_MAPGETCHAR(variable, _position_) \
	GOC_MSG(GOC_StMsgMapChar, variable, GOC_MSG_MAPGETCHAR_ID); \
	variable##Full.position = _position_;
extern const char* GOC_MSG_MAPGETCHAR_ID;

/*
 * Free point from position map.
 * If point is connected with any other resources
 * than they should be free also. Point will be
 * free by a caller.
 * TODO: verify that value is needed.
 */
#define GOC_MSG_POSMAPFREEPOINT(variable, _x_, _y_, _value_) \
	GOC_MSG(GOC_StMsgMapPointValue, variable, GOC_MSG_MAPGETPOINT_ID); \
	variable##Full.x = _x_; \
	variable##Full.y = _y_; \
	variable##Full.value = _value_;
extern const char* GOC_MSG_POSMAPFREEPOINT_ID;

/*
 * Change position of cursor.
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	unsigned int x;
	unsigned int y;
} GOC_StMsgMapPoint;
#define GOC_MSG_CURSORCHANGE(variable, _x_, _y_) \
	GOC_MSG(GOC_StMsgMapPoint, variable, GOC_MSG_CURSORCHANGE_ID); \
	variable##Full.x = _x_; \
	variable##Full.y = _y_;
extern const char* GOC_MSG_CURSORCHANGE_ID;

/**
 * Fill the area of map. (Fragment repaint)
 */
typedef struct {
	GOC_STRUCT_MESSAGE;
	unsigned int mapx;
	unsigned int mapy;
	unsigned int width;
	unsigned int height;
	GOC_StChar* pElements;
} GOC_StMsgMapFill;
#define GOC_MSG_POSMAPFILL(variable, _mapx_, _mapy_, _width_, _height_, _elements_) \
	GOC_MSG(GOC_StMsgMapFill, variable, GOC_MSG_MAPFILL_ID); \
	variable##Full.mapx = _mapx_; \
	variable##Full.mapy = _mapy_; \
	variable##Full.width = _width_; \
	variable##Full.height = _height_; \
	variable##Full.pElements = _elements_
extern const char* GOC_MSG_MAPFILL_ID;

/**
 * The mask structure.
 *
 * Mask has:
 *  - real area - it's whole area of the mask data
 *  - view area - it's area, which is shown on screen
 *
 * Fields:
 * 	element - fields from the element structure.
 * 	d - the mask real width
 * 	w - the mask real height
 * 	xp - the begin x coordinate of the view area
 * 	yp - the begin y coordinate of the view area
 * 	xs - the ammount of spaces in x coordinate between mask elements
 * 	ys - the ammount of spaces in y coordinate between mask elements
 * 	kursor - cursor position in real mask coordinates
 * 	pMapa, nMapa - the array of maps connected to the mask
 */
#define GOC_STRUCT_MASK \
	GOC_STRUCT_ELEMENT; \
	unsigned int d; \
	unsigned int w; \
	int xp, yp; \
	unsigned char xs, ys; \
	GOC_StPoint kursor; \
	GOC_HANDLER *pMapa; \
	_GOC_TABEL_SIZETYPE_ nMapa

	
typedef struct GOC_StMask
{
	GOC_STRUCT_MASK;
} GOC_StMask;

int goc_maskListener(GOC_HANDLER uchwyt, GOC_StMessage* msg);
int goc_maskSetRealArea(GOC_HANDLER uchwyt, int d, int w);
int goc_maskSet(GOC_HANDLER uchwyt, unsigned char x, unsigned char y, int val);
int goc_maskGet(GOC_HANDLER uchwyt, int x, int y);

#define goc_maskGetUnder(uchwyt) \
	goc_nbitFieldGet( ((GOC_StMask*)uchwyt)->dane, \
			((GOC_StMask*)uchwyt)->xp+((GOC_StMask*)uchwyt)->kursor.x + \
			((GOC_StMask*)uchwyt)->d*(((GOC_StMask*)uchwyt)->yp + \
			((GOC_StMask*)uchwyt)->kursor.y) )
#define goc_maskSetUnder(uchwyt, val) \
	goc_nbitFieldSet( ((GOC_StMask*)uchwyt)->dane, \
			((GOC_StMask*)uchwyt)->xp + ((GOC_StMask*)uchwyt)->kurso.x + \
			((GOC_StMask*)uchwyt)->d*(((GOC_StMask*)uchwyt)->yp + \
			((GOC_StMask*)uchwyt)->kursor.y), val )
int goc_maskPaintPoint(GOC_HANDLER uchwyt, int x, int y);
int goc_maskSetValue(GOC_HANDLER uchwyt, short pozycja, char znak, GOC_COLOR color);
#define goc_maskGetBuffer(uchwyt) ((GOC_StMask*)uchwyt)->dane->dane
// w bajtach
#define goc_maskGetBufferSize(uchwyt) (((GOC_StMask*)uchwyt)->dane->lbnw * \
		((GOC_StMask*)uchwyt)->dane->ld-1)/8+1
void goc_maskRemMaps(GOC_HANDLER uchwyt);
int goc_maskAddMap(GOC_HANDLER uchwyt, GOC_HANDLER mapa);
#define goc_maskCharSpace(uchwyt, xspace, yspace) \
	((GOC_StMask*)uchwyt)->xs = xspace;\
	((GOC_StMask*)uchwyt)->ys = yspace;

/**
 * Read the cursor's position of the mask.
 *
 * handler - mask handler
 * cursor - GOC_StPoint instance, where value will be assigned
 */
#define goc_maskGetCursor(handler, cursor) \
	cursor.x = ((GOC_StMask*)handler)->kursor.x; \
	cursor.y = ((GOC_StMask*)handler)->kursor.y;

#endif // ifndef _GOC_MASK_H_
