/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "map_screen.h"
#include "display/display.h"

namespace colour {
	extern LuxColour white;
	extern LuxColour black;
	extern LuxColour red;
	extern LuxColour blue;
	extern LuxColour green;
	extern LuxColour yellow;
}


MokoiMapScreen::MokoiMapScreen( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
{
	this->_x = x;
	this->_y = y;
	this->_width = width;
	this->_height = height;
	this->_offset_x = width * x;
	this->_offset_y = height * y;
	this->initialised = false;
	this->_mask = nullptr;
}

MokoiMapScreen::~MokoiMapScreen()
{
	this->Close();
}

bool MokoiMapScreen::Init()
{
	if ( this->initialised )
		return false;

	this->initialised = true;
	if ( !this->_mask )
		this->_mask = Lux_Mask_New( (this->_width), (this->_height) );

	MapObject * object = nullptr;
	LuxSprite * current = nullptr;
	std::list<MapObject *>::iterator p;

	if ( this->_objects.size() )
	{
		for ( p = this->_objects.begin(); p != this->_objects.end(); p++ )
		{
			object = (*p);
			object->SetData( object->type);
			if ( (object->type == OBJECT_SPRITE) && object->has_data )
			{
				current = (LuxSprite *)object->GetData();
				if ( current->mask_value )
				{
					if ( object->effects.flip_image == 1 || object->effects.flip_image == 3 )
						this->FillMask( object->position.x, object->position.y, ( object->position.h ? object->position.h : current->sheet_area.h), ( (*p)->position.w ? object->position.w : current->sheet_area.w), current->mask_value);
					else
						this->FillMask( object->position.x, object->position.y, ( object->position.w ? object->position.w : current->sheet_area.w), ( object->position.h ? object->position.h : current->sheet_area.h), current->mask_value);
				}
			}

			if ( !lux::display->AddObjectToLayer(object->layer, object, true) )
			{
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << object->TypeName() << " (" << object->sprite << ") add failed" << std::endl;
				object->type = 0;
			}

		}
	}
	return true;
}

bool MokoiMapScreen::Close()
{
	this->initialised = false;
	std::list<MapObject *>::iterator p;
	for ( p = this->_objects.begin(); p != this->_objects.end(); p++ )
	{
		if ( lux::display->RemoveObject((*p)->layer, (*p)) )
		{
			(*p)->FreeData();
		}
	}
	Lux_Mask_Free(this->_mask);
	this->_mask = nullptr;

	return true;
}

/* Masks */
uint16_t MokoiMapScreen::GetMaskValue(uint16_t x, uint16_t y)
{
	if ( !this->initialised )
		return 0xFFFF;

	if ( this->_mask )
	{
		return Lux_Mask_GetValue(this->_mask, x - this->_offset_x, y - this->_offset_y);
	}
	return 0xFFFF;
}

void MokoiMapScreen::FillMask(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t value)
{
	if ( !this->initialised )
		return;
	if ( this->_mask )
	{
		LuxRect r;
		r.x = x - this->_offset_x;
		r.y = y - this->_offset_y;
		r.w = width;
		r.h = height;
		if ( r.x < 0 )
		{
			r.w += r.x;
			r.x = 0;
		}
		if ( r.y < 0 )
		{
			r.h += r.y;
			r.y = 0;
		}
		Lux_Mask_FillArea( this->_mask, (uint16_t)r.x, (uint16_t)r.y, r.w, r.h, value );
	}
}

void MokoiMapScreen::BuildMask()
{
	LuxSprite * current = nullptr;
	std::list<MapObject *>::iterator p;
	for ( p = this->_objects.begin(); p != this->_objects.end(); p++ )
	{
		(*p)->SetData((*p)->type);
		if ( ((*p)->type == OBJECT_SPRITE) && (*p)->has_data )
		{
			current = (LuxSprite *)(*p)->GetData();
			if ( (*p)->effects.flip_image == 1 || (*p)->effects.flip_image == 3 )
				this->FillMask( (*p)->position.x, (*p)->position.y, ( (*p)->position.h ? (*p)->position.h : current->sheet_area.h), ( (*p)->position.w ? (*p)->position.w : current->sheet_area.w), current->mask_value);
			else
				this->FillMask( (*p)->position.x, (*p)->position.y, ( (*p)->position.w ? (*p)->position.w : current->sheet_area.w), ( (*p)->position.h ? (*p)->position.h : current->sheet_area.h), current->mask_value);
		}
	}

	//Lux_Mask * sprite_mask = Lux_Mask_New(0, 0);
	//Lux_Mask_Load("test.mask", sprite_mask);
	//Lux_Mask_CopyArea(sprite_mask, map_mask, 10, 10);
	//Lux_Mask_FillArea(map_mask, 5, 0, 10, 20, 69);
}

void MokoiMapScreen::DrawMask( fixed position[3] )
{
	LuxRect rect = {0,0,2,2, 7000};
	ObjectEffect mask_colour;

	int32_t y = this->_offset_y - MAKE_FIXED_INT(position[1]);
	int32_t x = this->_offset_x - MAKE_FIXED_INT(position[0]);

	mask_colour.primary_colour = colour::red;

	if ( this->_mask )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "Mask Memory:" << this->_mask->length  << "bytes. Offset:" << this->_offset_x << "x" << this->_offset_y << std::endl;
		lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << this->_mask->width << "x" << this->_mask->height << std::endl;

		for( uint32_t i = 0; i < this->_mask->length; i += 2 )
		{
			if ( this->_mask->data[i] )
			{
				rect.y = (i / this->_mask->width);
				rect.x = i - ( rect.y * this->_mask->width );

				rect.y += y;
				rect.x += x;
				mask_colour.primary_colour.a = 128;
				mask_colour.primary_colour.r = this->_mask->data[i];

				lux::display->graphics.DrawRect( rect, mask_colour );
			}
		}


	}
}

//bool MapObjectSort(MapObject * a, MapObject * b )
//{
//	if ( a->layer < b->layer )
//		return true;
//	if ( a->position.z < b->position.z )
//		return true;
//	if ( a->position.y < b->position.y )
//		return true;
//	if ( a->position.x < b->position.x )
//		return true;

//	return false;
//}

bool ObjectSort(MapObject * a, MapObject * b );
void MokoiMapScreen::SortObjects()
{
	this->_objects.sort( ObjectSort );
}

