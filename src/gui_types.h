/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _GUITYPES_H_
#define _GUITYPES_H_


#define GUI_NOTHING 0
#define GUI_EXIT -1
#define GUI_CONFIRM -2
#define GUI_CANCEL -3

#define GUI_BG 0
#define GUI_INSET 1
#define GUI_OUTSET 2
#define GUI_TEXT 3


typedef enum {
	ENABLED,
	DISABLED,
	PRESSED,
	HOVER,
	CLICKED,
	ACTIVE,
	ACTIVEHOVER,
	ACTIVEPRESSED,
	WIDGET_STATES_SIZE
} _WidgetStates;

inline void operator++(_WidgetStates& a, int) { a = _WidgetStates(a+1); };



#endif /* _GUITYPES_H_ */
