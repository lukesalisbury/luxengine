/****************************
Copyright © 2006-2015 Luke Salisbury
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
#include "elix/elix_string.hpp"
#include "display/display_functions.h"
#include "mokoi_game.h"

ObjectEffect default_cursor_fx( colour::black, colour::white );
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

UserInterface::UserInterface(DisplaySystem * display )
{
	if ( display == NULL )
	{
		this->internal_display = new DisplaySystem( 640, 480, 16 );
	}
	else
	{
		this->internal_display = display;
	}
	this->ui_region = this->internal_display->screen_dimension;

	this->controller = NULL;
	this->active_child = this->main_widget = NULL;

	this->SetTheme();

	this->controller = new Player(1,LOCAL);
	this->children_iter = this->children_widget.begin();

	this->last_mouse = 0;
}



UserInterface::~UserInterface()
{
	this->sprites.clear();
	this->children_widget.clear();

	if ( this->controller )
	{
		delete this->controller;
	}

	NULLIFY( this->css );


	if ( this->internal_display != lux::display )
	{
		NULLIFY( this->internal_display );
	}

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

		if ( !lux::game_data )
		{
			check_local_filesystem = true;
		}
		else
		{
			check_local_filesystem = !lux::game_data->HasFile("./sprites/" + file);
		}

		current_sheet = new LuxSheet( file, this->internal_display->graphics );


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

			local_filesystem_path = elix::directory::Resources("", file);

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

Widget * UserInterface::AddWidgetChild(Widget * parent, LuxRect region, LuxWidget type, std::string text, uint32_t text_length)
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
	child->SetText(text, text_length);
	this->children_widget.push_back(child);
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
	this->children_widget.push_back(child);

	if ( text.length() )
	{
		child->SetText(text);
	}
	return child;
}

#define DIALOG_YES 1
#define DIALOG_OKAY 1
#define DIALOG_NO 2
#define DIALOG_CANCEL 3

Widget * UserInterface::AddChild(LuxRect region, LuxWidget type, std::string text)
{
	static LuxRect button_positions[3] = {
		{-50, -24, 40, 20, 6000},
		{-100, -24, 40, 20, 6000},
		{-150, -24, 40, 20, 6000}
	};

	if ( type == DIALOGTEXT )
	{
		if ( region.w < 200 )
		{
			region.w = 200;
		}
	}


	uint16_t border_width = css->GetSize( (type >= DIALOGEXT ? DIALOG : type), ENABLED, "border-width" );
	std::string box_model = css->GetString( (type >= DIALOGEXT ? DIALOG : type), ENABLED, "box-model" );

	if ( border_width && !box_model.compare("border") )
	{
		region.x -= border_width;
		region.y -= border_width;
		region.w += border_width*2;
		region.h += border_width*2;
	}

	if ( type >= DIALOG )
	{
		region.y -= button_positions[0].h/2;
		region.h += button_positions[0].h/2;
	}

	Widget * child = new Widget( region, ( type >= DIALOGEXT ? DIALOG : type), this->css );
	child->SetText(text);
	this->children_widget.push_back(child);


	if ( type == DIALOGYESNO )
	{
		Widget * new_child2 = this->AddWidgetChild( child, button_positions[0], BUTTON, "No" );
		new_child2->SetValue( DIALOG_NO );

		Widget * new_child1 = this->AddWidgetChild( child, button_positions[1], BUTTON, "Yes" );
		new_child1->SetValue( DIALOG_YES );

		this->active_child = child;
	}
	else if ( type == DIALOGOK )
	{
		Widget * new_child1 = this->AddWidgetChild( child, button_positions[0], BUTTON, "Okay" );

		new_child1->SetValue( DIALOG_OKAY );

		this->active_child = child;

	}
	else if ( type == DIALOGYESNOCANCEL )
	{
		Widget * no_child = this->AddWidgetChild( child, button_positions[0], BUTTON, "No" );
		no_child->SetValue( DIALOG_NO );

		Widget * yes_child = this->AddWidgetChild( child, button_positions[1], BUTTON, "Yes" );
		yes_child->SetValue( DIALOG_YES );

		Widget * cancel_child = this->AddWidgetChild( child, button_positions[2], BUTTON, "Cancel" );
		cancel_child->SetValue( DIALOG_CANCEL );

		this->active_child = child;

	}
	else if ( type == DIALOGTEXT )
	{
		LuxRect input_rect = {10, -24, 100, 20, 6000};

		input_rect.w = region.w - 70;

		Widget * input_child = this->AddWidgetChild( child, input_rect, INPUTTEXT );
		Widget * button_child = this->AddWidgetChild( child, button_positions[0], BUTTON, "Enter" );

		button_child->SetValue(DIALOG_OKAY);
		input_child->SetValue(DIALOG_OKAY);
		input_child->_focus = true;


		this->active_child = input_child;
		this->main_widget = input_child;
	}

	return child;
}

void UserInterface::DrawWidget(Widget * child, uint16_t z)
{
	if ( child == NULL || !this->internal_display )
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
					this->internal_display->graphics.DrawRect( area, object->colours[cur_state] );
					break;
				case 'q': /* Thobber */
				{
					child->_value++;
					if ( child->_value > 4 )
						child->_value = 0;
					else if ( child->_value < 0 )
						child->_value = 0;
					_WidgetStates q_state = (_WidgetStates)child->_value;

					this->internal_display->graphics.DrawCircle( area, object->colours[q_state] );
					break;
				}
				case OBJECT_CIRCLE:
					this->internal_display->graphics.DrawCircle( area, object->colours[cur_state] );
					break;
				case OBJECT_LINE:
					this->internal_display->graphics.DrawLine( area, object->colours[cur_state] );
					break;
				case OBJECT_TEXT:
					this->internal_display->graphics.DrawText(object->text, area, object->colours[cur_state], false );
					break;
				case OBJECT_IMAGE:
					spr = this->GetSprite(object->text, object->image_width, object->image_height );
					if ( spr )
						this->internal_display->graphics.DrawSprite( spr, area, default_fx );
					else
						this->internal_display->graphics.DrawRect( area, default_fx );
					spr = NULL;
					break;
				case OBJECT_SPRITE:
					if ( child->GetData() )
					{
						spr = (LuxSprite*)child->GetData();
						this->internal_display->graphics.DrawSprite( spr, area, default_fx );
						spr = NULL;
					}
					else
						this->internal_display->graphics.DrawRect( area, default_fx );
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
		this->children_widget.push_back(child);
		this->active_child = child;
	}
}

void UserInterface::RemoveChild(Widget * child)
{
	if ( child )
	{
		if ( this->active_child == child )
		{
			this->active_child = NULL;
		}
		this->children_widget.remove(child);
		this->children_iter = this->children_widget.begin();
	}
}

void UserInterface::RemoveAll()
{
	this->active_child = NULL;
	this->children_widget.clear();
	this->children_iter = this->children_widget.begin();
}

int32_t UserInterface::ReturnResult()
{
	int32_t return_value = 0;
	while ( !return_value )
	{

		this->internal_display->Display(PAUSED);
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
	if ( !this->internal_display )
		return 1;

	uint16_t z = 7000;
	std::list<Widget *>::iterator wid_iter;

	this->ui_region.z = z;
	this->internal_display->graphics.DrawRect(this->ui_region, this->background);
	if ( this->children_widget.size() )
	{
		for ( wid_iter = this->children_widget.begin(); wid_iter != this->children_widget.end(); wid_iter++ )
		{
			this->DrawWidget((*wid_iter), ++z);
		}
	}
	this->internal_display->graphics.Show();
	return 1;
}


int32_t UserInterface::Loop()
{
	if ( !this->internal_display || !this->controller || !lux::core )
		return 0;

	std::list<Widget *>::iterator wid_iter;
	int32_t return_value = 0;
	uint16_t z = 7000;
	uint16_t key_event = 0;

	// Set a Active Child Widget
	if ( this->children_widget.size() )
	{
		if ( !this->active_child )
		{
			this->active_child = (*this->children_widget.begin());
		}
	}


	/* Update Inputs */
	lux::core->RefreshInput( this->internal_display );
	this->controller->Loop();

	int16_t confirm = (this->controller->GetButton(PLAYER_CONFIRM) > 0);
	int16_t cancel = (this->controller->GetButton(PLAYER_CANCEL) > 0);
	int16_t mouse_button = (this->controller->GetButton(PLAYER_POINTER) > 0);
	int16_t move_x = this->controller->GetControllerAxis(0);
	int16_t move_y = this->controller->GetControllerAxis(1);
	int32_t mouse_x = this->controller->GetPointer(0);
	int32_t mouse_y = this->controller->GetPointer(1);

	if (move_x != 0)
		move_x /= 160;
	if (move_y != 0)
		move_y /= 160;

	// Pointer Events
	wid_iter = this->children_widget.begin();
	while( wid_iter != this->children_widget.end())
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
						this->active_child = w;
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
		if ( this->children_widget.begin() == this->children_widget.end() ) //do nothing
		{
			this->active_child = NULL;
		}
		else if ( this->children_iter == this->children_widget.end() )
		{
			this->children_iter = this->children_widget.begin();
			this->active_child = (*this->children_iter);
		}
		else
		{
			this->children_iter++;
			if ( this->children_iter != this->children_widget.end() )
			{
				this->active_child = (*this->children_iter);
			}
			else
			{
				this->active_child = NULL;
			}
		}

	}
	else if ( move_y < 0 )
	{
		if ( this->children_widget.begin() == this->children_widget.end() ) //do nothing
		{
			this->active_child = NULL;
		}
		else if ( this->children_iter == this->children_widget.begin() )
		{
			this->children_iter = this->children_widget.end();
			this->children_iter--;
			this->active_child = (*this->children_iter);
		}
		else
		{
			this->children_iter--;
			this->active_child = (*this->children_iter);
		}
	}



	// Button Events
	if ( this->active_child && !return_value )
	{
		this->active_child->_focus = true;

		if ( this->active_child->GetState() == HOVER )
			this->active_child->SetState( ACTIVEHOVER );
		else if ( this->active_child->GetState() == PRESSED )
			this->active_child->SetState( ACTIVEPRESSED );
		else
			this->active_child->SetState( ACTIVE );

		if ( confirm )
			if ( !this->active_child->SendEvent( confirm ) )
				return_value = this->active_child->GetValue();

		if ( cancel )
			if ( !this->active_child->SendEvent( cancel ) )
				return_value = this->active_child->GetValue();
	}

	// Keyboard Events
	while ( lux::core->InputLoopGet(this->internal_display, key_event) )
	{
		if ( key_event == 27 )
		{
			return_value = GUI_EXIT;
		}

		if ( this->active_child != NULL && !return_value )
		{
			if ( !this->active_child->SendEvent( key_event ) )
			{
				return_value = this->active_child->GetValue();
			}
		}
	}


	/* Start Drawing */
	this->ui_region.z = z;
	this->internal_display->graphics.DrawRect(this->ui_region, this->background);

	if (this->children_widget.size())
	{
		for ( wid_iter = this->children_widget.begin(); wid_iter != this->children_widget.end(); wid_iter++ )
		{
			this->DrawWidget((*wid_iter), ++z);
		}
	}

	last_mouse = mouse_button;

	this->internal_display->DisplayOverlay();

	this->internal_display->graphics.DisplayPointer(1, mouse_x, mouse_y, this->controller->PlayerColour );
	this->internal_display->graphics.Show();
	lux::core->Idle( );

	return return_value;
}
