/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "widget.h"

Widget::Widget(LuxRect region, LuxWidget type, CSSParser * style)
{
	this->bgbox = this->borders[0] = this->borders[1] = this->borders[2] = this->borders[3] = this->textbox = NULL;
	this->_data = NULL;
	this->_value = 0;
	this->_original = this->_state = ENABLED;
	this->_type = type;
	this->_region = region;
	this->_parent = NULL;
	this->_state = ENABLED;

	if ( type == EMPTYWINDOW )
	{
		this->bgbox = this->borders[0] = this->borders[1] = this->borders[2] = this->borders[3] = this->textbox = NULL;
		return;
	}
	else if (  type == THROBBER )
	{
		this->_value = 0;

		int16_t center_point = (this->_region.w/2);
		this->_region.h += 40;
		this->bgbox = this->InsertWidgetObject( OBJECT_RECTANGLE, this->_region.x, this->_region.y, this->_region.w, this->_region.h - 2 );
		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, this->_region.x + 4, this->_region.y + 32, this->_region.w, this->_region.h - 4 ); /* Text */
		this->borders[0] = this->InsertWidgetObject( OBJECT_THOBBER, this->_region.x + center_point - 8, this->_region.y + 16, 8, 8 );
		this->borders[1] = this->InsertWidgetObject( OBJECT_THOBBER, this->_region.x + center_point, this->_region.y + 8, 8, 8 );
		this->borders[2] = this->InsertWidgetObject( OBJECT_THOBBER, this->_region.x + center_point + 8, this->_region.y + 16, 8, 8 );
		this->borders[3] = this->InsertWidgetObject( OBJECT_THOBBER, this->_region.x + center_point, this->_region.y + 24, 8, 8 );

		this->GenerateBorderColours(style, this->borders[0], 0);
		this->GenerateBorderColours(style, this->borders[1], 0);
		this->GenerateBorderColours(style, this->borders[2], 0);
		this->GenerateBorderColours(style, this->borders[3], 0);
		this->GenerateColours(style, this->textbox, false);
		this->GenerateColours(style, this->bgbox, true);
		return;
	}

	if ( type == DIALOG || type >= DIALOGTEXT )
	{
		this->GenerateBox(style);
	}
	else if ( type == IMAGE )
	{
		this->InsertWidgetObject( OBJECT_SPRITE, this->_region.x, this->_region.y, this->_region.w, this->_region.h );
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
		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, this->_region.x + 4, this->_region.y + 4, this->_region.w, this->_region.h ); /* Text */
		this->GenerateColours(style, this->textbox, false);
		this->InsertWidgetObject( OBJECT_SPRITE, this->_region.x + 2, this->_region.y + 2, 32, 32 );
		this->textbox->position.x += 36;
	}
	else if ( type == CHECKBOX )
	{
		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, this->_region.x + 4, this->_region.y + 4, this->_region.w, this->_region.h ); /* Text */
		this->GenerateColours(style, this->textbox, false);
		WidgetObject * toggle = this->InsertWidgetObject( OBJECT_CIRCLE, this->_region.x + 2, this->_region.y + 2, 6, 6 );
		this->GenerateColours(style, toggle, true);
		this->textbox->position.x += 12;
	}
	else
	{
		this->textbox = this->InsertWidgetObject( OBJECT_TEXT, this->_region.x + 4, this->_region.y + 4, this->_region.w, this->_region.h ); /* Text */
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

WidgetObject * Widget::InsertTextObject(uint8_t type, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::string t )
{
	WidgetObject * object = new WidgetObject;
	object->style = 0;
	object->type = type;
	object->text = t;
	object->position.x = x;
	object->position.y = y;
	object->position.w = w;
	object->position.h = h;
	object->position.z = 3000;
	object->image_width = (uint16_t)w;
	object->image_height = (uint16_t)h;

	this->objects.push_back(object);

	return object;
}

WidgetObject * Widget::InsertWidgetObject(uint8_t type, uint32_t x, uint32_t y, uint32_t w, uint32_t h )
{
	WidgetObject * object = new WidgetObject;
	object->style = (type == OBJECT_TEXT ? 3 : 0);
	object->type = type;
	object->text = "";
	object->position.x = x;
	object->position.y = y;
	object->position.w = w;
	object->position.h = h;
	object->position.z = 3000;
	object->image_width = 0;
	object->image_height = 0;

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

	bool using_border_model = !(style->GetString(this->_type, this->_state, "box-model").compare("border"));
	LuxRect boxregion = this->_region;
	uint16_t border_width = style->GetSize(this->_type, this->_state, "border-width");
	uint16_t min_height = style->GetSize(this->_type, this->_state, "min-height");

	if ( min_height > this->_region.h )
	{
		this->_region.h = boxregion.h = min_height;
	}

	if ( using_border_model )
	{
		this->padding.w = style->GetSize(this->_type, this->_state, "padding");
		this->padding.h = style->GetSize(this->_type, this->_state, "padding");
		boxregion.x += border_width;
		boxregion.y += border_width;
		boxregion.w -= border_width*2;
		boxregion.h -= border_width*2;

	}
	else
	{
/*
		this->_region.x += border_width;
		this->_region.y += border_width;
		this->_region.w -= border_width*2;
		this->_region.h -= border_width*2;
*/
		this->padding.w = style->GetSize(this->_type, this->_state, "padding");
		this->padding.h = style->GetSize(this->_type, this->_state, "padding");
	}

	if ( style->HasKey(this->_type, this->_state, "border-image") )
	{
		std::string image = style->GetString(this->_type, this->_state, "border-image");

		/* Resize box to be a multiple of the border widths */
		if ( border_width )
		{
			boxregion.w = (1+(boxregion.w/border_width))*border_width;
			boxregion.h = (1+(boxregion.h/border_width))*border_width;
		}


		this->bgbox = this->InsertTextObject( OBJECT_IMAGE, boxregion.x, boxregion.y, boxregion.w, boxregion.h, image + ":4"); /* box background  */
		this->borders[0] = this->InsertTextObject( OBJECT_IMAGE, boxregion.x, boxregion.y - border_width, boxregion.w, border_width, image + ":1" ); /* top border */
		this->borders[1] = this->InsertTextObject( OBJECT_IMAGE, boxregion.x + boxregion.w, boxregion.y, border_width, boxregion.h, image + ":5" ); /* right border */
		this->borders[2] = this->InsertTextObject( OBJECT_IMAGE, boxregion.x, boxregion.y + boxregion.h, boxregion.w, border_width, image + ":7" ); /* bottom border */
		this->borders[3] = this->InsertTextObject( OBJECT_IMAGE, boxregion.x - border_width, boxregion.y, border_width, boxregion.h, image + ":3" ); /* left border */

		this->InsertTextObject( OBJECT_IMAGE, boxregion.x-border_width, boxregion.y-border_width, border_width, border_width, image + ":0" );
		this->InsertTextObject( OBJECT_IMAGE, boxregion.x+boxregion.w, boxregion.y-border_width, border_width, border_width, image + ":2" );
		this->InsertTextObject( OBJECT_IMAGE, boxregion.x+boxregion.w, boxregion.y + boxregion.h, border_width, border_width, image + ":8" );
		this->InsertTextObject( OBJECT_IMAGE, boxregion.x-border_width, boxregion.y + boxregion.h, border_width, border_width, image + ":6" );

		this->bgbox->image_width = this->borders[0]->image_width = this->borders[1]->image_width = this->borders[2]->image_width = this->borders[3]->image_width = border_width;
		this->bgbox->image_height = this->borders[0]->image_height = this->borders[1]->image_height = this->borders[2]->image_height = this->borders[3]->image_height = border_width;
	}
	else
	{
		this->bgbox = this->InsertWidgetObject( OBJECT_RECTANGLE, boxregion.x, boxregion.y, boxregion.w, boxregion.h ); /* box background */

		if ( border_width > 0 )
		{
			this->borders[0] = this->InsertWidgetObject( OBJECT_RECTANGLE, boxregion.x - border_width, boxregion.y - border_width, boxregion.w+border_width+border_width, border_width ); /* top border */
			this->borders[1] = this->InsertWidgetObject( OBJECT_RECTANGLE, boxregion.x + boxregion.w, boxregion.y, border_width, boxregion.h ); /* right border */
			this->borders[2] = this->InsertWidgetObject( OBJECT_RECTANGLE, boxregion.x - border_width, boxregion.y + boxregion.h, boxregion.w + border_width + border_width, border_width ); /* bottom border */
			this->borders[3] = this->InsertWidgetObject( OBJECT_RECTANGLE, boxregion.x - border_width, boxregion.y, border_width, boxregion.h ); /* left border */
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
		if ( length && this->bgbox )
		{
			this->bgbox->position.w = (length * 8) + 4;
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
	else if ( this->_type == BUTTON )
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
			this->_original = (this->_value == 1 ? ENABLED : DISABLED);
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

int32_t Widget::SetValue(int32_t value)
{
	this->_value = value;
	return this->GetValue();
}

_WidgetStates Widget::GetState()
{
	if (this->_type == CHECKBOX && this->_state <= DISABLED )
	{
		return (this->_value == 1 ? ENABLED : DISABLED);
	}
	else
		return this->_state;
}

_WidgetStates Widget::SetState(_WidgetStates state)
{
	if (this->_type == CHECKBOX  )
	{
		if ( state == ACTIVE )
			this->_state = this->_original;
		else
			this->_state = state;
	}
	else if (this->_type > CHECKBOX)
	{
		this->_state = state;
	}
	return this->_state;
}
