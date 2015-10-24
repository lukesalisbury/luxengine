/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "widget.h"
#include <cstdlib>

/**
 * @brief Widget::Widget
 * @param region
 * @param type
 * @param style
 */
Widget::Widget( LuxRect region, LuxWidget type, CSSParser * style )
{
	this->background_object = this->border_object[0] = this->border_object[1] = this->border_object[2] = this->border_object[3] = this->text_object = NULL;
	this->data = NULL;
	this->_value = 0;
	this->original_state = this->state = ENABLED;
	this->_type = type;
	this->_region = region;
	this->parent = NULL;
	this->state = ENABLED;

	this->Generate(style);
}

/**
 * @brief Widget::~Widget
 */
Widget::~Widget()
{
	while ( this->objects.begin() != this->objects.end() )
	{
		delete (*this->objects.begin());
		this->objects.erase( this->objects.begin() );
	}
}

/**
 * @brief Widget::Generate
 * @param style
 */
void Widget::Generate( CSSParser * style )
{
	switch ( this->_type )
	{
		case GUIBACKGROUND:
		{
			this->background_object = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, this->_region.w, this->_region.h );

			break;
		}
		case EMPTYWINDOW:
		{
			break;
		}
		case DIALOG:
		{
			this->GenerateBox( style );
			this->text_object = this->InsertWidgetObject( OBJECT_TEXT, 2, 2, 0, 0 );
			this->GenerateColours( style, this->text_object, false );

			break;
		}
		case THROBBER:
		{
			int16_t center_point = (this->_region.w/2);

			this->_region.h += 40;

			this->text_object = this->InsertWidgetObject( OBJECT_TEXT, 4, 32, this->_region.w, this->_region.h - 4 );

			this->border_object[0] = this->InsertWidgetObject( OBJECT_THOBBER, center_point - 8, 16, 8, 8 );
			this->border_object[1] = this->InsertWidgetObject( OBJECT_THOBBER, center_point, 8, 8, 8 );
			this->border_object[2] = this->InsertWidgetObject( OBJECT_THOBBER, center_point + 8, 16, 8, 8 );
			this->border_object[3] = this->InsertWidgetObject( OBJECT_THOBBER, center_point, 24, 8, 8 );

			this->GenerateBorderColours( style, this->border_object[0], 0 );
			this->GenerateBorderColours( style, this->border_object[1], 0 );
			this->GenerateBorderColours( style, this->border_object[2], 0 );
			this->GenerateBorderColours( style, this->border_object[3], 0 );
			this->GenerateColours( style, this->text_object, false );

			break;
		}
		case CHECKBOX:
		{
			WidgetObject * toggle = this->InsertWidgetObject( OBJECT_CIRCLE, 2, 2, 6, 6 );

			this->text_object = this->InsertWidgetObject( OBJECT_TEXT, 16, 4, this->_region.w, this->_region.h );

			this->GenerateColours( style, this->text_object, false);
			this->GenerateColours( style, toggle, true);

			break;
		}
		case TEXT:
		{
			if ( style && style->HasKey(TEXT, ENABLED, "background-color") )
			{
				this->GenerateBox(style);
			}
			this->text_object = this->InsertWidgetObject( OBJECT_TEXT, 2, 2, 0, 0 );

			this->GenerateColours(style, this->text_object, false);

			break;
		}
		case INPUTTEXT:
		{
			this->GenerateBox(style);
			this->text_object = this->InsertWidgetObject( OBJECT_TEXT, 2, 2, 0, 0 );

			this->GenerateColours(style, this->text_object, false);

			break;
		}
		case BUTTON:
		{
			this->GenerateBox(style);
			this->text_object = this->InsertWidgetObject( OBJECT_TEXT, 2, 2, 0, 0 );

			this->GenerateColours(style, this->text_object, false);

			break;
		}
		case LIST:
		{
			/* TODO */
			break;
		}
		case IMAGEBUTTON:
		{
			this->GenerateBox(style);
			this->text_object = this->InsertWidgetObject( OBJECT_TEXT, 40, 4, this->_region.w, this->_region.h ); /* Text */
			this->GenerateColours(style, this->text_object, false);
			this->InsertWidgetObject( OBJECT_SPRITE, 2, 2, 32, 32 );

			break;
		}
		case IMAGE:
		{
			this->InsertWidgetObject( OBJECT_SPRITE, 0, 0, this->_region.w, this->_region.h );
			break;
		}

		case BOX:
		{
			this->GenerateBox(style);
			break;
		}

		default:
			break;
	}

}

/**
 * @brief Widget::InsertTextObject
 * @param type
 * @param x
 * @param y
 * @param w
 * @param h
 * @param t
 * @param alignment
 * @return
 */
WidgetObject * Widget::InsertTextObject(uint8_t type, int16_t x, int16_t y, int16_t w, int16_t h, std::string t , uint8_t alignment)
{
	WidgetObject * object = new WidgetObject;
	object->style = 0;
	object->type = type;
	object->text = t;
	object->offset.x = x;
	object->offset.y = y;
	object->offset.w = w;
	object->offset.h = h;
	object->offset.alignment = alignment;
	object->image_width = (uint16_t)abs(w);
	object->image_height = (uint16_t)abs(h);
	object->background = false;

	this->objects.push_back(object);

	return object;
}

/**
 * @brief Widget::InsertWidgetObject
 * @param type
 * @param x
 * @param y
 * @param w
 * @param h
 * @param alignment
 * @return
 */
WidgetObject * Widget::InsertWidgetObject(uint8_t type, int16_t x, int16_t y, int16_t w, int16_t h , uint8_t alignment)
{
	WidgetObject * object = new WidgetObject;
	object->style = (type == OBJECT_TEXT ? 3 : 0);
	object->type = type;
	object->text = "";
	object->offset.x = x;
	object->offset.y = y;
	object->offset.w = w;
	object->offset.h = h;
	object->offset.alignment = alignment;
	object->image_width = 0;
	object->image_height = 0;
	object->background = false;

	this->objects.push_back(object);

	return object;
}

/**
 * @brief Widget::GenerateColours
 * @param style
 * @param object
 * @param background
 */
void Widget::GenerateColours( CSSParser * style, WidgetObject * object, bool background )
{
	if ( !object || !style )
		return;

	LuxColour default_colour = style->GetColour(this->_type, ENABLED, (background ? "background-color" : "color") );
	for ( _WidgetStates q = ENABLED; q < WIDGET_STATES_SIZE; q++)
	{
		object->colours[q].style = STYLE_NORMAL;
		if ( style->HasKey(this->_type, q, (background ? "background-color" : "color")) )
			object->colours[q].primary_colour = style->GetColour( this->_type, q, (background ? "background-color" : "color") );
		else
			object->colours[q].primary_colour = default_colour;

		if ( style->HasKey(this->_type, q, (background ? "background-color-secondary" : "color-secondary")) )
			object->colours[q].secondary_colour = style->GetColour( this->_type, q, (background ? "background-color-secondary" : "color-secondary") );
		else
			object->colours[q].secondary_colour = object->colours[q].primary_colour;

		if ( style->HasKey(this->_type, q, (background ? "background-style" : "style")) )
			object->colours[q].style = style->GetStyle( this->_type, q, (background ? "background-style" : "style") );
	}

	object->alignment = style->GetAlign( this->_type, ENABLED, "text-align" );

}

/**
 * @brief Widget::GenerateBorderColours
 * @param style
 * @param object
 * @param position
 */
void Widget::GenerateBorderColours( CSSParser * style, WidgetObject * object, uint8_t position )
{
	if ( !object || !style )
		return;

	LuxColour default_colour = style->GetColour(this->_type, ENABLED, "border-color");
	int8_t border_style = style->GetSize(this->_type, ENABLED, "border-style");
	for ( _WidgetStates q = ENABLED; q < WIDGET_STATES_SIZE; q++)
	{
		object->colours[q].style = STYLE_NORMAL;
		if ( style->HasKey(this->_type, q, "border-color") )
			object->colours[q].primary_colour = style->GetColour(this->_type, q, "border-color");
		else
			object->colours[q].primary_colour = default_colour;

		if ( (border_style == 2 && (position == 0 || position == 1)) || (border_style == 1 && (position == 2 || position == 3)) )
		{
			object->colours[q].primary_colour.r += 10;
			object->colours[q].primary_colour.g += 10;
			object->colours[q].primary_colour.b += 10;
		}
		else if ( (border_style == 1 && (position == 0 || position == 1)) || (border_style == 2 && (position == 2 || position == 3)) )
		{
			object->colours[q].primary_colour.r -= 10;
			object->colours[q].primary_colour.g -= 10;
			object->colours[q].primary_colour.b -= 10;
		}
	}
}

/**
 * @brief Widget::GenerateBox
 * @param style
 */
void Widget::GenerateBox( CSSParser * style )
{
	if ( style == NULL )
		return;

	LuxRect boxregion = this->_region;
	uint16_t border_width = style->GetSize( this->_type, this->state, "border-width" );
	uint16_t min_height = style->GetSize( this->_type, this->state, "min-height" );

	if ( min_height > this->_region.h )
	{
		this->_region.h = boxregion.h = min_height;
	}

	this->padding.w = style->GetSize(this->_type, this->state, "padding");
	this->padding.h = style->GetSize(this->_type, this->state, "padding");


	if ( style->HasKey(this->_type, this->state, "border-image") )
	{
		std::string image = style->GetString(this->_type, this->state, "border-image");

		/* Resize box to be a multiple of the border widths */
		if ( border_width )
		{
			boxregion.x += border_width;
			boxregion.y += border_width;
			boxregion.w -= border_width*2;
			boxregion.h -= border_width*2;

			/* To have an even repeat, divide by border width and round up */
			boxregion.w = ((boxregion.w/border_width) + (boxregion.w%border_width ? 1 : 0)) * border_width;
			boxregion.h = ((boxregion.h/border_width) + (boxregion.h%border_width ? 1 : 0)) * border_width;
		}

		this->background_object = this->InsertTextObject( OBJECT_IMAGE, 0, 0, 0, 0, image + ":4", 0); /* box background  */
		this->border_object[0] = this->InsertTextObject( OBJECT_IMAGE, 0, -border_width, 0, border_width, image + ":1", LEFTTOP ); /* top border */
		this->border_object[1] = this->InsertTextObject( OBJECT_IMAGE, 0, 0, border_width, 0, image + ":5", RIGHTTOP ); /* right border */
		this->border_object[2] = this->InsertTextObject( OBJECT_IMAGE, 0, 0, 0, border_width, image + ":7", LEFTBOTTOM ); /* bottom border */
		this->border_object[3] = this->InsertTextObject( OBJECT_IMAGE, -border_width, 0, border_width, 0, image + ":3", LEFTTOP ); /* left border */

		this->InsertTextObject( OBJECT_IMAGE, -border_width, -border_width, border_width, border_width, image + ":0", LEFTTOP ); /* TOP LEFT */
		this->InsertTextObject( OBJECT_IMAGE, 0, -border_width, border_width, border_width, image + ":2", RIGHTTOP );/* TOP RIGHT */
		this->InsertTextObject( OBJECT_IMAGE, -border_width, 0, border_width, border_width, image + ":6", LEFTBOTTOM );/* bottom right */
		this->InsertTextObject( OBJECT_IMAGE, 0, 0, border_width, border_width, image + ":8", RIGHTBOTTOM ); /* bottom left */

		this->background_object->image_width = this->border_object[0]->image_width = this->border_object[1]->image_width = this->border_object[2]->image_width = this->border_object[3]->image_width = border_width;
		this->background_object->image_height = this->border_object[0]->image_height = this->border_object[1]->image_height = this->border_object[2]->image_height = this->border_object[3]->image_height = border_width;
	}
	else
	{
		this->background_object = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, 0, 0 ); /* box background */

		if ( border_width > 0 )
		{
			this->border_object[0] = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, 0, border_width, LEFTTOP ); /* top border */
			this->border_object[1] = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, border_width, 0, RIGHTTOP  ); /* right border */
			this->border_object[2] = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, -border_width, 0, border_width, LEFTBOTTOM ); /* bottom border */
			this->border_object[3] = this->InsertWidgetObject( OBJECT_RECTANGLE, -border_width, 0, border_width, 0, LEFTTOP ); /* left border */
		}
	}

	this->GenerateColours( style, this->background_object, true );

	if ( this->border_object[0] )
	{
		this->GenerateBorderColours( style, this->border_object[0], 0 );
		this->GenerateBorderColours( style, this->border_object[1], 1 );
		this->GenerateBorderColours( style, this->border_object[2], 2 );
		this->GenerateBorderColours( style, this->border_object[3], 3 );
	}

	this->_region = boxregion;
}

/**
 * @brief Widget::GetText
 * @return
 */
std::string Widget::GetText()
{
	if ( this->text_object )
	{
		return this->text_object->text;
	}
	return "";
}

/**
 * @brief Widget::SetText
 * @param text
 * @param length
 * @return
 */
std::string Widget::SetText( std::string text, uint32_t length )
{
	if ( this->text_object )
	{
		this->text_object->text = text;
		if ( length )
		{
			this->text_object->offset.w = (length * 8);

			if ( this->text_object->alignment )
			{
				this->text_object->offset.alignment = CENTERCENTER;
				this->text_object->offset.h = 8;
			}
		}
	}
	return this->GetText();
}

/**
 * @brief Widget::SendEvent
 * @param event_code
 * @return
 */
bool Widget::SendEvent( int32_t event_code )
{
	if ( this->_type == DIALOG )
	{
		if ( event_code == 13 ) // Enter
		{
			this->SetValue(1);
			return false;
		}
		else if ( event_code == 27 ) // Escape
		{
			this->SetValue(2);
			return false;
		}
	}
	else if ( this->_type == BUTTON || this->_type == IMAGEBUTTON )
	{
		/* Return true to continue */
		if ( event_code == 1 || event_code == 13 )
		{
			return false;
		}
	}
	else if ( this->_type == INPUTTEXT )
	{
		if ( event_code >= 30 )
		{
			this->text_object->text.append(1, event_code);
		}
		if ( event_code == '\b' )
		{
			if ( this->text_object->text.length() > 0 )
			{
				this->text_object->text.erase( this->text_object->text.length()-1 );
			}
		}
		if ( event_code == 1 )
		{
			if ( !this->_focus )
			{
				this->SetText("");
			}
			this->_focus = true;
		}
		if ( event_code == 13 )
		{
			this->_focus = false;
			return false;
		}
	}
	else if ( this->_type == CHECKBOX )
	{
		if ( event_code == 32 || event_code == 1 || event_code == 13  )
		{
			this->_value = !this->_value;
			this->original_state = (this->_value == 1 ? ENABLED : DISABLED);
		}
	}
	else
	{
		/* Return true to continue */
		if ( event_code )
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Widget::Refresh
 */
void Widget::Refresh()
{

}

void Widget::SetRegion( LuxRect q )
{
	this->_region = q;
}

LuxRect Widget::GetRegion( )
{
	return this->_region;
}

int32_t Widget::SetValue(int32_t value)
{
	this->_value = value;
	return this->GetValue();
}

_WidgetStates Widget::GetState()
{
	if (this->_type == CHECKBOX && this->state <= DISABLED )
	{
		return (this->_value == 1 ? ENABLED : DISABLED);
	}
	else
		return this->state;
}

_WidgetStates Widget::SetState(_WidgetStates state)
{
	if (this->_type == CHECKBOX  )
	{
		if ( state == ACTIVE )
			this->state = this->original_state;
		else
			this->state = state;
	}
	else if (this->_type > CHECKBOX)
	{
		this->state = state;
	}
	return this->state;
}

uint32_t Widget::GetAreaHeight()
{
	return this->_region.h + this->padding.h;
}


