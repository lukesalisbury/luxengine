/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "widget.h"
#include <cstdlib>

Widget::Widget( LuxRect region, LuxWidget type, CSSParser * style )
{
	this->bgbox = this->borders[0] = this->borders[1] = this->borders[2] = this->borders[3] = this->textbox = NULL;
	this->data = NULL;
	this->_value = 0;
	this->original_state = this->state = ENABLED;
	this->_type = type;
	this->_region = region;
	this->parent = NULL;
	this->state = ENABLED;

	if ( type == EMPTYWINDOW )
	{
		return;
	}
	else if (  type == THROBBER )
	{
		int16_t center_point = (this->_region.w/2);
		this->_region.h += 40;

		this->bgbox = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, this->_region.w, this->_region.h - 2 );
		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, 4, 32, this->_region.w, this->_region.h - 4 ); /* Text */

		this->InsertWidgetObject( OBJECT_THOBBER, center_point - 8, 16, 8, 8 );
		this->InsertWidgetObject( OBJECT_THOBBER, center_point, 8, 8, 8 );
		this->InsertWidgetObject( OBJECT_THOBBER, center_point + 8, 16, 8, 8 );
		this->InsertWidgetObject( OBJECT_THOBBER, center_point, 24, 8, 8 );

		this->GenerateBorderColours( style, this->borders[0], 0 );
		this->GenerateBorderColours( style, this->borders[1], 0 );
		this->GenerateBorderColours( style, this->borders[2], 0 );
		this->GenerateBorderColours( style, this->borders[3], 0 );
		this->GenerateColours( style, this->textbox, false );
		this->GenerateColours( style, this->bgbox, true );
		return;
	}

	if ( type == DIALOG || type >= DIALOGTEXT )
	{
		this->GenerateBox( style );
	}
	else if ( type == IMAGE )
	{
		this->InsertWidgetObject( OBJECT_SPRITE, 0, 0, this->_region.w, this->_region.h );
		return;
	}
	else if ( type != TEXT && type != CHECKBOX )
	{
		this->GenerateBox(style);
	}

	if ( type == TEXT )
	{
		if ( style->HasKey(TEXT, ENABLED, "background-color") )
		{
			this->GenerateBox(style);
		}
	}

	if ( type == INPUTTEXT )
	{
		this->bgbox->style = 4;
		this->borders[0]->style = this->borders[1]->style = 2;
		this->borders[2]->style = this->borders[3]->style = 1;
	}

	if ( type == IMAGEBUTTON )
	{
		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, 40, 4, this->_region.w, this->_region.h ); /* Text */

		this->InsertWidgetObject( OBJECT_SPRITE, 2, 2, 32, 32 );

		this->GenerateColours(style, this->textbox, false);

	}
	else if ( type == CHECKBOX )
	{
		WidgetObject * toggle;

		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, 16, 4, this->_region.w, this->_region.h ); /* Text */

		toggle = this->InsertWidgetObject( OBJECT_CIRCLE, 2, 2, 6, 6 );

		this->GenerateColours( style, this->textbox, false);
		this->GenerateColours( style, toggle, true);
	}
	else
	{

		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, 2, 2, 0, 0 ); /* Text */
		this->GenerateColours(style, this->textbox, false);
	}
}

Widget::~Widget()
{
	while ( this->objects.begin() != this->objects.end())
	{
		delete (*this->objects.begin());
		this->objects.erase( this->objects.begin() );
	}
	this->objects.clear();
}

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

void Widget::GenerateColours( CSSParser * style, WidgetObject * object, bool background )
{
	if ( !object || !style )
		return;
	LuxColour default_colour = style->GetColour(this->_type, ENABLED, (background ? "background-color" : "color") );
	for ( _WidgetStates q = ENABLED; q < WIDGET_STATES_SIZE; q++)
	{
		object->colours[q].style = STYLE_NORMAL;
		if ( style->HasKey(this->_type, q, (background ? "background-color" : "colour")) )
			object->colours[q].primary_colour = style->GetColour(this->_type, q, (background ? "background-color" : "color") );
		else
			object->colours[q].primary_colour = default_colour;

		if ( style->HasKey(this->_type, q, (background ? "background-color-secondary" : "color-secondary")) )
			object->colours[q].secondary_colour = style->GetColour(this->_type, q, (background ? "background-color-secondary" : "color-secondary") );
		else
			object->colours[q].secondary_colour = object->colours[q].primary_colour;

		if ( style->HasKey(this->_type, q, (background ? "background-style" : "style")) )
			object->colours[q].style = style->GetStyle(this->_type, q, (background ? "background-style" : "style") );
	}

	object->alignment = style->GetAlign( this->_type, ENABLED, "text-align" );

}

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

void Widget::GenerateBox(CSSParser * style)
{
	if ( style == NULL )
		return;

	LuxRect boxregion = this->_region;
	uint16_t border_width = style->GetSize(this->_type, this->state, "border-width");
	uint16_t min_height = style->GetSize(this->_type, this->state, "min-height");

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
			boxregion.w = ((boxregion.w%border_width ? 1 : 0)+(boxregion.w/border_width))*border_width;
			boxregion.h = ((boxregion.h%border_width ? 1 : 0)+(boxregion.h/border_width))*border_width;
		}

		this->bgbox = this->InsertTextObject( OBJECT_IMAGE, 0, 0, 0, 0, image + ":4", 0); /* box background  */
		this->borders[0] = this->InsertTextObject( OBJECT_IMAGE, 0, -border_width, 0, border_width, image + ":1", LEFTTOP ); /* top border */
		this->borders[1] = this->InsertTextObject( OBJECT_IMAGE, 0, 0, border_width, 0, image + ":5", RIGHTTOP ); /* right border */
		this->borders[2] = this->InsertTextObject( OBJECT_IMAGE, 0, 0, 0, border_width, image + ":7", LEFTBOTTOM ); /* bottom border */
		this->borders[3] = this->InsertTextObject( OBJECT_IMAGE, -border_width, 0, border_width, 0, image + ":3", LEFTTOP ); /* left border */

		this->InsertTextObject( OBJECT_IMAGE, -border_width, -border_width, border_width, border_width, image + ":0", LEFTTOP ); /* TOP LEFT */
		this->InsertTextObject( OBJECT_IMAGE, 0, -border_width, border_width, border_width, image + ":2", RIGHTTOP );/* TOP RIGHT */
		this->InsertTextObject( OBJECT_IMAGE, -border_width, 0, border_width, border_width, image + ":6", LEFTBOTTOM );/* bottom right */
		this->InsertTextObject( OBJECT_IMAGE, 0, 0, border_width, border_width, image + ":8", RIGHTBOTTOM ); /* bottom left */

		this->bgbox->image_width = this->borders[0]->image_width = this->borders[1]->image_width = this->borders[2]->image_width = this->borders[3]->image_width = border_width;
		this->bgbox->image_height = this->borders[0]->image_height = this->borders[1]->image_height = this->borders[2]->image_height = this->borders[3]->image_height = border_width;
	}
	else
	{
		this->bgbox = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, 0, 0 ); /* box background */

		if ( border_width > 0 )
		{
			this->borders[0] = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, 0, border_width, LEFTTOP ); /* top border */
			this->borders[1] = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, 0, border_width, 0, RIGHTTOP  ); /* right border */
			this->borders[2] = this->InsertWidgetObject( OBJECT_RECTANGLE, 0, -border_width, 0, border_width, LEFTBOTTOM ); /* bottom border */
			this->borders[3] = this->InsertWidgetObject( OBJECT_RECTANGLE, -border_width, 0, border_width, 0, LEFTTOP ); /* left border */
		}
	}

	this->GenerateColours( style, this->bgbox, true );
	this->GenerateBorderColours( style, this->borders[0], 0 );
	this->GenerateBorderColours( style, this->borders[1], 1 );
	this->GenerateBorderColours( style, this->borders[2], 2 );
	this->GenerateBorderColours( style, this->borders[3], 3 );


	this->_region = boxregion;
}

std::string Widget::GetText()
{
	if ( this->textbox )
	{
		return this->textbox->text;
	}
	return "";
}

std::string Widget::SetText( std::string text, uint32_t length )
{
	if ( this->textbox )
	{
		this->textbox->text = text;
		if ( length )
		{
			this->textbox->offset.w = (length * 8);

			if ( this->textbox->alignment )
			{
				this->textbox->offset.alignment = CENTERCENTER;
				this->textbox->offset.h = 8;
			}
		}
	}
	return this->GetText();
}

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
			this->textbox->text.append(1, event_code);
		}
		if ( event_code == 8 )
		{
			if ( this->textbox->text.length() > 0 )
			{
				this->textbox->text.erase( this->textbox->text.length()-1 );
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


