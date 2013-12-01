/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _LUXTYPES_H_
	#define _LUXTYPES_H_

#include "stdheader.h"

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
	GAMEERROR = -1
} LuxState;

typedef enum {
	GUIBACKGROUND,
	EMPTYWINDOW,
	DIALOG,
	THROBBER,
	CHECKBOX,
	TEXT,
	INPUTTEXT,
	BUTTON,
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
	int16_t z;
} LuxRect;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} LuxColour;

union cell_colour {
	uint32_t hex;
	LuxColour rgba;
};


union LuxMapIdent {
	uint32_t value;
	struct {
		uint32_t id:12;
		uint32_t localmap:20;
	} grid;
	struct {
		uint32_t unused:12;
		uint32_t map:20;
	} file;
};


typedef struct {
	int16_t x;
	int16_t y;
	uint32_t ms_length;
} LuxPath;


typedef struct {
	int16_t id;
	uint16_t type;
	uint32_t entity;
	LuxRect rect;
	bool added;
} CollisionObject;

typedef struct {
	uint16_t rect, type, hit_rect, rect_x, rect_y, hit_x, hit_y;
	uint32_t entity_id;
	std::string entity_name;
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
	CONTROLAXIS,
	CONTROLBUTTON,
	CONTROLHAT,
	CONTROLBALL,
	TOUCHSCREEN,
	HARDWARE
} InputDevice;

typedef struct {
	uint16_t width, height, ref;
	uint32_t length;
	uint8_t * data;
} Lux_Mask;



#define OBJECT_SPRITE 's'
#define OBJECT_RECTANGLE 'r'
#define OBJECT_CIRCLE 'c'
#define OBJECT_CANVAS 'M'
#define OBJECT_POLYGON 'p'
#define OBJECT_LINE 'l'
#define OBJECT_TEXT 't'
#define OBJECT_THOBBER 'q'
#define OBJECT_IMAGE 'i'
#define OBJECT_UNKNOWN 'U'

#endif /* _LUXTYPES_H_ */
