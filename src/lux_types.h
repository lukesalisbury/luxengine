/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef LUX_TYPES_H
	#define LUX_TYPES_H

#include "stdheader.h"
#include <map>

class MapObject;

#ifdef __GNUWIN32__
	#define LUX_DIR_SEPARATOR '\\'
	#define LUX_DIR_SSEPARATOR "\\"
#else
	#define LUX_DIR_SEPARATOR '/'
	#define LUX_DIR_SSEPARATOR "/"
#endif

typedef enum {
	NOTRUNNING,
	RUNNING,
	PAUSED,
	NOUPDATE,
	GUIMODE,
	IDLE,
	SAVING,
	LOADING,
	RESETGAME,
	EXITING,
	INVALIDPOSITION,
	RELOADENTITIES,
	GAMEERROR = -1
} LuxState;

#define DIALOG_YES 1
#define DIALOG_OKAY 1
#define DIALOG_NO 2
#define DIALOG_CANCEL 3

typedef enum {
	GUIBACKGROUND,
	EMPTYWINDOW,
	DIALOG,
	THROBBER,
	CHECKBOX,
	TEXT,
	INPUTTEXT,
	BUTTON,
	LIST,
	IMAGEBUTTON,
	IMAGE,
	BOX,
	DIALOGEXT = 1024,
	DIALOGOK = 1025,
	DIALOGYESNO = 1026,
	DIALOGYESNOCANCEL = 1027,
	DIALOGTEXT = 1028
} LuxWidget;

inline LuxWidget operator^(LuxWidget a, LuxWidget b) {
	return LuxWidget(int(a) ^ int(b));
}

inline LuxWidget& operator^=(LuxWidget& a, LuxWidget b) {
	a = a ^ b;
	return a;
}


typedef struct {
	int32_t x, y;
	uint16_t w, h;
	fixed z;
} LuxRect;

inline LuxRect & operator+=(LuxRect& a, LuxRect& b) {
	a.x += b.x;
	a.y += b.y;
	a.w += b.w;
	a.h += b.h;
	a.z += b.z;

	if ( b.x < 0 )
		a.x += a.w;

	if ( b.y < 0 )
		a.y += a.h;

	return a;
}


typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} LuxColour;


union LuxColour2 {
	uint32_t hex;
	struct {
		uint8_t a;
		uint8_t b;
		uint8_t g;
		uint8_t r;
	};
};

union cell_colour {
	uint32_t hex;
	LuxColour rgba;
};


union LuxMapIdent {
	uint32_t value;
	struct {
		uint32_t section:20;
		uint32_t map:12;
	} grid;
};


typedef struct {
	int16_t x;
	int16_t y;
	uint32_t ms_length;
} LuxPath;


typedef struct {
	uint32_t entity;
	uint16_t type;
	int16_t id;
	LuxRect rect;
	bool added;
} CollisionObject;

typedef struct {
	uint32_t entity_id;
	uint16_t rect_x, rect_y, hit_x, hit_y;
	uint8_t rect, type, hit_rect;
} CollisionResult;

typedef struct {
	uint32_t ip;
	uint16_t in_port;
	uint16_t out_port;
	bool connected;
} LuxNetworkInfo;

typedef enum {
	NOINPUT,
	KEYBOARD,
	MOUSEAXIS,
	MOUSEBUTTON,
	MOUSEWHEEL,
	CONTROLAXIS,
	CONTROLBUTTON,
	CONTROLHAT,
	CONTROLBALL,
	TOUCHSCREEN,
	VIRTUALBUTTON,
	VIRTUALAXIS,
	HARDWARE
} InputDevice;

typedef struct {
	uint16_t width, height, ref;
	uint32_t length;
	uint8_t * data;
} Lux_Mask;

typedef std::map<uint32_t, MapObject *> MapObjectList;
typedef std::map<uint32_t, MapObject *>::iterator MapObjectListIter;

#define OBJECT_SPRITE 's'
#define OBJECT_RECTANGLE 'r'
#define OBJECT_CIRCLE 'c'
#define OBJECT_CANVAS 'M'
#define OBJECT_POLYGON 'p'
#define OBJECT_LINE 'l'
#define OBJECT_TEXT 't'
#define OBJECT_THOBBER 'q'
#define OBJECT_IMAGE 'i'
#define OBJECT_VIRTUAL_SPRITE 'v'
#define OBJECT_UNKNOWN 'U'


enum {
	SHADER_DEFAULT,
	SHADER_GLOW,
	SHADER_NEGATIVE,
	SHADER_REPLACE,
	SHADER_GREY,
	SHADER_CUSTOM1,
	SHADER_CUSTOM2,
	SHADER_CUSTOM3,
	SHADER_CUSTOM4,
	SHADER_CUSTOM5,
	SHADER_CUSTOM6,
	NUM_SHADERS
};

#endif /* LUX_TYPES_H */
