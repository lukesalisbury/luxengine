/****************************
Copyright © 2006-2011 Luke Salisbury
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
#include "gui_types.h"
#include "css.h"
#include "object_effect.h"
#include "lux_sprite.h"
#include "sprite_sheet.h"
#include <list>

typedef struct {
	uint8_t type, style;
	LuxRect position;
	std::string text;
	ObjectEffect colour;
	ObjectEffect colours[WIDGET_STATES_SIZE];
	uint16_t image_width;
	uint16_t image_height;
} WidgetObject;

class Widget
{
	friend class UserInterface;
	public:
		Widget( LuxRect region, LuxWidget type, CSSParser * style = NULL );
		~Widget();

		void * _data;
	private:
		LuxRect _region;
		LuxRect padding;
		std::list<WidgetObject *> objects;
		WidgetObject * textbox;
		WidgetObject * bgbox;
		WidgetObject * borders[4];

	private:
		Widget * _parent;
		_WidgetStates _state;
		_WidgetStates _original;

		int32_t _value;
		bool _focus;
		LuxSprite * _sprite;
		LuxWidget _type;

		void GenerateBox(CSSParser * style);
		void GenerateColours( CSSParser * style, WidgetObject * object, bool background );
		void GenerateBorderColours( CSSParser * style, WidgetObject * object, uint8_t position );

	public:
		void Exit();
		void ResetState() { this->_state = this->_original; }
		void Refresh();

		WidgetObject * InsertTextObject(uint8_t type, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::string t );
		WidgetObject * InsertWidgetObject(uint8_t type, uint32_t x, uint32_t y, uint32_t w, uint32_t h );

		_WidgetStates GetState();
		_WidgetStates SetState(_WidgetStates state);

		std::string GetText();
		std::string SetText(std::string text, uint32_t length = 0); // don't worry about length, it's just  a hack

		int32_t GetValue() { return this->_value; }
		int32_t SetValue(int32_t value);

		void * GetData() { return this->_data; }
		void SetData(void * data) { this->_data = data; }

		bool SendEvent(int32_t event_code);

		uint32_t GetAreaHeight() { return this->_region.h + this->padding.h; }





};


#endif /* _WIDGET_H_ */
