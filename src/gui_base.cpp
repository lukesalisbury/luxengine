/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "stdheader.h"
#include "core.h"
#include "gui.h"
#include "misc_functions.h"
#include "css.h"
#include "elix_string.hpp"
#include "display_functions.h"
#include "mokoi_game.h"

ObjectEffect default_cursor_fx( (LuxColour){0,0,0,255}, (LuxColour){255,255,255,255} );
extern ObjectEffect default_fx;

void LuxWidget_MergeRect(LuxRect & a, WidgetObjectPosition& b)
{

	if ( b.alignment == RIGHTBOTTOM || b.alignment == RIGHTTOP )
	{
		a.x += a.w;
	}

	if ( b.alignment == RIGHTBOTTOM || b.alignment == LEFTBOTTOM )
	{
		a.y += a.h;
	}

	if ( b.alignment == CENTERCENTER )
	{
		a.x += (a.w - b.w)/2;
		a.y += (a.h - b.h)/2;
	}
	else
	{
		a.x += b.x;
		a.y += b.y;

		if ( b.w < 0 )
			a.w += b.w;
		else if ( b.w > 0 )
			a.w = b.w;

		if ( b.h < 0 )
			a.h += b.h;
		else if ( b.h > 0 )
			a.h = b.h;
	}

}

UserInterface::UserInterface(LuxRect region, DisplaySystem * display)
{
	this->controller = NULL;
	this->_region = region;
	this->_activechild = this->_mainwidget = NULL;
	this->_display = display;

	this->SetTheme();

	this->controller = new Player(1,LOCAL);
	this->children_iter = this->_children.begin();

	this->last_mouse = 0;
}

UserInterface::UserInterface()
{
	this->_display = NULL;
	this->controller = NULL;
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "UserInterface Error: Region & DisplaySystem must be set" << std::endl;
}

UserInterface::~UserInterface()
{
	this->sprites.clear();
	this->_children.clear();

	this->_display = NULL;
	if ( this->controller )
		delete this->controller;
}


CSSParser * UserInterface::GetCSSParser()
{
	return this->css;
}

LuxSheet * UserInterface::GetSheet( std::string file, uint16_t width, uint16_t height )
{
	bool check_local_filesystem = false;
	LuxSheet * current_sheet = NULL;
	LuxSheetIter iter_sheet = this->sheets.find(file);

	if ( iter_sheet == this->sheets.end() )
	{
		uint16_t n = 0;
		std::stringstream content;

		if ( !lux::game )
		{
			check_local_filesystem = true;
		}
		else
		{
			check_local_filesystem = !lux::game->HasFile("./sprites/" + file);
		}

		current_sheet = new LuxSheet( file, this->_display->graphics );


		for ( n = 0; n < 9; n++ )
		{
			content << n << "\t" << ((n%3)*width) << "\t" << ((n/3)*height) << "\t" << width << "\t" << height;
			current_sheet->ParseSimpleText( content.str() );
			content.str("");
		}

		if ( check_local_filesystem )
		{
			std::string local_filesystem_path;
			elix::Image * image_file;
			elix::File * image_source_file;

			elix::string::StripUnwantedCharacters(file);
			local_filesystem_path = elix::directory::Resources("") + file;

			image_source_file = new elix::File( local_filesystem_path );
			if ( image_source_file->Exist() )
			{
				image_file = new elix::Image;
				image_file->LoadFile(image_source_file);
				current_sheet->LoadFromImage(image_file);

				delete image_file;

			}
			else
			{
				delete current_sheet;
				current_sheet = NULL;
			}

		}
		else
		{
			current_sheet->Load();
		}

		if ( current_sheet )
		{
			this->sheets.insert( std::pair<std::string, LuxSheet*>(file, current_sheet) );
		}

	}
	else
	{
		current_sheet = iter_sheet->second;

	}

	return current_sheet;

}

LuxSprite * UserInterface::GetSprite( std::string file, uint16_t width, uint16_t height )
{

	LuxSheet * sheet = NULL;
	LuxSprite * return_sprite = NULL;
	std::vector<std::string> name;

	elix::string::Split(file, ":", &name);
	if ( name.size() == 2 )
	{
		sheet = this->GetSheet(name[0], width, height);
		if ( sheet )
		{
			return_sprite = sheet->GetSprite(name[1]);
		}
	}

	name.clear();
	return return_sprite;
}

Widget * UserInterface::AddWidgetChild(Widget * parent, LuxRect region, LuxWidget type)
{
	Widget * child = NULL;

	if ( region.x < 0 )
		region.x += ( parent->_region.x + parent->_region.w );
	else
		region.x += parent->_region.x;

	if ( region.y < 0 )
		region.y += ( parent->_region.y + parent->_region.h );
	else
		region.y += parent->_region.y;

	child = new Widget(region, ( type >= DIALOGEXT ? DIALOG : type), this->css);
	this->_children.push_back(child);
	return child;
}

Widget * UserInterface::AddWidgetChild(Widget * parent, int32_t x, int32_t y, uint16_t w, uint16_t h, LuxWidget type, std::string text)
{
	LuxRect region;
	Widget * child = NULL;

	region.x = x;
	region.y = y;
	region.w = w;
	region.h = h;

	if ( region.x < 0 )
		region.x += ( parent->_region.x + parent->_region.w );
	else
		region.x += parent->_region.x;

	if ( region.y < 0 )
		region.y += ( parent->_region.y + parent->_region.h );
	else
		region.y += parent->_region.y;

	child = new Widget(region, ( type >= DIALOGEXT ? DIALOG : type), this->css);
	this->_children.push_back(child);
	if ( text.length() )
	{
		child->SetText(text);
	}
	return child;
}


Widget * UserInterface::AddChild(LuxRect region, LuxWidget type, LuxColour colour, std::string text)
{
	Widget * child = NULL;

	child = new Widget( region, ( type >= DIALOGEXT ? DIALOG : type), this->css );
	child->SetText(text);
	this->_children.push_back(child);
	if ( type == DIALOGYESNO )
	{
		Widget * new_child1 = this->AddWidgetChild( child, (LuxRect){-100, -24, 40, 20, 6000}, BUTTON );
		new_child1->SetText("Yes");
		new_child1->SetValue(1);

		Widget * new_child2 = this->AddWidgetChild( child, (LuxRect){-50, -24, 40, 20, 6000}, BUTTON );
		new_child2->SetText("No");
		new_child2->SetValue(2);

		this->_activechild = child;
	}
	else if ( type == DIALOGOK )
	{
		Widget * new_child1 = this->AddWidgetChild( child, (LuxRect){-50, -24, 40, 20, 6000}, BUTTON );
		new_child1->SetText("Okay");
		new_child1->SetValue(1);
		this->_activechild = child;

	}
	else if ( type == DIALOGTEXT )
	{
		LuxRect child1_rect = {10, 24, 100, 20, 6000};
		LuxRect child2_rect = {-50, -24, 40, 20, 6000};

		child1_rect.w = region.w - 70;

		Widget * new_child1 = this->AddWidgetChild( child, child1_rect, INPUTTEXT );
		Widget * new_child2 = this->AddWidgetChild( child, child2_rect, BUTTON );
		new_child2->SetText("Okay");
		new_child2->SetValue(1);
		new_child1->_focus = true;
		new_child1->SetValue(1);
		this->_activechild = new_child1;
		this->_mainwidget = new_child1;
	}

	return child;
}

void UserInterface::DrawWidget(Widget * child, uint16_t z)
{
	if ( child == NULL || !this->_display )
	{
		return;
	}

	LuxSprite * spr = NULL;
	WidgetObject * object = NULL;
	std::list<WidgetObject*>::iterator l_object;

	if ( child->objects.size() )
	{
		for ( l_object = child->objects.begin(); l_object != child->objects.end(); l_object++ )
		{
			_WidgetStates cur_state = child->GetState();
			LuxRect area = child->GetRegion();

			object = (*l_object);
			area.z = z;

			LuxWidget_MergeRect(area, object->offset);

			switch( object->type )
			{
				case 'r':
					this->_display->graphics.DrawRect( area, object->colours[cur_state] );
					break;
				case 'q': /* Thobber */
				{
					child->_value++;
					if ( child->_value > 4 )
						child->_value = 0;
					else if ( child->_value < 0 )
						child->_value = 0;
					_WidgetStates q_state = (_WidgetStates)child->_value;

					this->_display->graphics.DrawCircle( area, object->colours[q_state] );
					break;
				}
				case OBJECT_CIRCLE:
					this->_display->graphics.DrawCircle( area, object->colours[cur_state] );
					break;
				case OBJECT_LINE:
					this->_display->graphics.DrawLine( area, object->colours[cur_state] );
					break;
				case OBJECT_TEXT:
					this->_display->graphics.DrawText(object->text, area, object->colours[cur_state], false );
					break;
				case OBJECT_IMAGE:
					spr = this->GetSprite(object->text, object->image_width, object->image_height );
					if ( spr )
						this->_display->graphics.DrawSprite( spr, area, default_fx );
					else
						this->_display->graphics.DrawRect( area, default_fx );
					spr = NULL;
					break;
				case OBJECT_SPRITE:
					if ( child->GetData() )
					{
						spr = (LuxSprite*)child->GetData();
						this->_display->graphics.DrawSprite( spr, area, default_fx );
						spr = NULL;
					}
					else
						this->_display->graphics.DrawRect( area, default_fx );
					break;
				default:
					break;
			}

		}
	}
}

void UserInterface::AddChild(Widget * child)
{
	if ( child )
	{
		this->_children.push_back(child);
		this->_activechild = child;
	}
}

void UserInterface::RemoveChild(Widget * child)
{
	if (child)
	{
		if ( this->_activechild == child )
		{
			this->_activechild = NULL;
		}
		this->_children.remove(child);
		this->children_iter = this->_children.begin();
	}
}

void UserInterface::RemoveAll()
{
	this->_activechild = NULL;
	this->_children.clear();
	this->children_iter = this->_children.begin();
}

int32_t UserInterface::ReturnResult()
{
	int32_t return_value = 0;
	while ( !return_value )
	{

		this->_display->DrawGameStatic();
		return_value = this->Loop();
	}
	return return_value;
}

void UserInterface::SetBackground( ObjectEffect effect )
{
	this->background = effect;

}

int32_t UserInterface::Show()
{
	if ( !this->_display )
		return 1;

	uint16_t z = 7000;
	std::list<Widget *>::iterator wid_iter;

	this->_region.z = z;
	this->_display->graphics.DrawRect(this->_region, this->background);
	if ( this->_children.size() )
	{
		for ( wid_iter = this->_children.begin(); wid_iter != this->_children.end(); wid_iter++ )
		{
			this->DrawWidget((*wid_iter), ++z);
		}
	}
	this->_display->graphics.Show();
	return 1;
}


int32_t UserInterface::Loop()
{
	if ( !this->_display || !this->controller || !lux::core )
		return 0;

	std::list<Widget *>::iterator wid_iter;
	int32_t return_value = 0;
	uint16_t z = 7000;
	uint16_t key_event = 0;

	// Set a Active Child Widget
	if ( this->_children.size() )
	{
		if ( !this->_activechild )
		{
			this->_activechild = (*this->_children.begin());
		}
	}


	/* Update Inputs */
	lux::core->RefreshInput( this->_display );
	this->controller->Loop();

	int16_t confirm = (this->controller->GetButton(PLAYER_CONFIRM) > 0);
	int16_t cancel = (this->controller->GetButton(PLAYER_CANCEL) > 0);
	int16_t mouse_button = (this->controller->GetButton(PLAYER_POINTER) > 0);
	int16_t move_x = this->controller->GetControllerAxis(0);
	int16_t move_y = this->controller->GetControllerAxis(1);
	int16_t mouse_x = this->controller->GetPointer(0);
	int16_t mouse_y = this->controller->GetPointer(1);

	if (move_x != 0)
		move_x /= 160;
	if (move_y != 0)
		move_y /= 160;

	// Pointer Events
	wid_iter = this->_children.begin();
	while( wid_iter != this->_children.end())
	{
		Widget * w = (*wid_iter);
		if ( Lux_Util_PointCollide( w->_region, mouse_x, mouse_y ) )
		{
			if (mouse_button == 1)
			{
				w->SetState(PRESSED);
			}
			else if ( mouse_button == 0 && last_mouse == 1 )
			{
				if ( w->GetState() == PRESSED || w->GetState() == ACTIVEPRESSED )
				{
					w->SetState(CLICKED);
					if ( !w->SendEvent(1) )
						return_value = w->GetValue();
					else
						this->_activechild = w;
				}
			}
			else
			{
				w->SetState(HOVER);
			}
		}
		else
		{
			w->ResetState();
		}
		wid_iter++;
	}

	// Axis Events
	if ( move_y > 0 )
	{
		if ( this->_children.begin() == this->_children.end() ) //do nothing
		{
			this->_activechild = NULL;
		}
		else if ( this->children_iter == this->_children.end() )
		{
			this->children_iter = this->_children.begin();
			this->_activechild = (*this->children_iter);
		}
		else
		{
			this->children_iter++;
			if ( this->children_iter != this->_children.end() )
			{
				this->_activechild = (*this->children_iter);
			}
			else
			{
				this->_activechild = NULL;
			}
		}

	}
	else if ( move_y < 0 )
	{
		if ( this->_children.begin() == this->_children.end() ) //do nothing
		{
			this->_activechild = NULL;
		}
		else if ( this->children_iter == this->_children.begin() )
		{
			this->children_iter = this->_children.end();
			this->children_iter--;
			this->_activechild = (*this->children_iter);
		}
		else
		{
			this->children_iter--;
			this->_activechild = (*this->children_iter);
		}
	}



	// Button Events
	if ( this->_activechild && !return_value )
	{
		this->_activechild->_focus = true;

		if ( this->_activechild->GetState() == HOVER )
			this->_activechild->SetState( ACTIVEHOVER );
		else if ( this->_activechild->GetState() == PRESSED )
			this->_activechild->SetState( ACTIVEPRESSED );
		else
			this->_activechild->SetState( ACTIVE );

		if ( confirm )
			if ( !this->_activechild->SendEvent( confirm ) )
				return_value = this->_activechild->GetValue();

		if ( cancel )
			if ( !this->_activechild->SendEvent( cancel ) )
				return_value = this->_activechild->GetValue();
	}

	// Keyboard Events
	while ( lux::core->InputLoopGet(this->_display, key_event) )
	{
		if ( key_event == 27 )
		{
			return_value = GUI_EXIT;
		}

		if ( this->_activechild != NULL && !return_value )
		{
			if ( !this->_activechild->SendEvent( key_event ) )
			{
				return_value = this->_activechild->GetValue();
			}
		}
	}


	/* Start Drawing */
	this->_region.z = z;
	this->_display->graphics.DrawRect(this->_region, this->background);

	if (this->_children.size())
	{
		for ( wid_iter = this->_children.begin(); wid_iter != this->_children.end(); wid_iter++ )
		{
			this->DrawWidget((*wid_iter), ++z);
		}
	}

	last_mouse = mouse_button;

	this->_display->DisplayOverlay();

	this->_display->graphics.DisplayPointer(1, mouse_x, mouse_y, this->controller->PlayerColour );
	this->_display->graphics.Show();
	lux::core->Idle( );

	return return_value;
}
