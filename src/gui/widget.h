/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _WIDGET_H_
#define _WIDGET_H_

class Widget;

#include "luxengine.h"
#include "gui/gui_types.h"
#include "gui/css.h"
#include "object_effect.h"
#include "lux_sprite.h"
#include "sprite_sheet.h"
#include <list>



typedef enum {
	LEFTTOP,
	RIGHTTOP,
	LEFTBOTTOM,
	RIGHTBOTTOM,
	CENTERCENTER
} WidgetAlignment;

typedef struct {
	int16_t x, y;
	int16_t w, h;
	uint8_t alignment;
} WidgetObjectPosition;


typedef struct {
	uint8_t type, style, alignment;
	WidgetObjectPosition offset;
	std::string text;
	ObjectEffect colour;
	ObjectEffect colours[WIDGET_STATES_SIZE];
	uint16_t image_width;
	uint16_t image_height;
	bool background;
} WidgetObject;

class Widget
{
	friend class UserInterface;
	public:
		Widget( LuxRect region, LuxWidget type, CSSParser * style = nullptr );
		~Widget();

	private:
		LuxRect _region;
		LuxRect padding;
		std::list<WidgetObject *> objects;

		WidgetObject * text_object;
		WidgetObject * background_object;
		WidgetObject * border_object[4];

	private:
		Widget * parent;
		_WidgetStates state;
		_WidgetStates original_state;

		void * data;
		int32_t _value;
		bool _focus;
		LuxSprite * _sprite;
		LuxWidget _type;

		void Generate( CSSParser * style );
		void GenerateBox( CSSParser * style );
		void GenerateColours( CSSParser * style, WidgetObject * object, bool background );
		void GenerateBorderColours( CSSParser * style, WidgetObject * object, uint8_t position );

	public:
		void Exit();
		void ResetState() { this->state = this->original_state; }
		void Refresh();

		void SetRegion( LuxRect q );
		LuxRect GetRegion( );

		WidgetObject * InsertTextObject(uint8_t type, int16_t x, int16_t y, int16_t w, int16_t h, std::string t, uint8_t alignment = LEFTTOP );
		WidgetObject * InsertWidgetObject(uint8_t type, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t alignment = LEFTTOP );

		_WidgetStates GetState();
		_WidgetStates SetState(_WidgetStates state);

		std::string GetText();
		std::string SetText(std::string text, uint32_t length = 0); // don't worry about length, it's just a hack

		int32_t GetValue() { return this->_value; }
		int32_t SetValue(int32_t value);

		void * GetData() { return this->data; }
		void SetData(void * data) { this->data = data; }

		void SetIcon(LuxSprite * data) { this->data = (void *)data; }

		bool SendEvent(int32_t event_code);

		uint32_t GetAreaHeight();





};


#endif /* _WIDGET_H_ */
