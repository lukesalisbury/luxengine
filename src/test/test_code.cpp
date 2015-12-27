/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "core.h"
#include "game_config.h"
#include "engine.h"
#include "elix/elix_string.hpp"
#include "elix/elix_path.hpp"
#include "mokoi_game.h"
#include "tinyxml/tinyxml2ext.h"
#include "worker.h"
#include "gui/gui.h"
#include "downloader.h"
#include "project_media.h"

#include "test_code.h"

static std::string style_names[] = { "STYLE_NORMAL","STYLE_VGRADIENT",	"STYLE_HGRADIENT",	"STYLE_GLOW",	"STYLE_OUTLINE",	"STYLE_REPCOLOUR",	"STYLE_NEGATIVE"};

namespace luxtest
{
	DisplaySystem * display = NULL;
	UserInterface * interface = NULL;

	LuxSprite * logo = NULL;

	bool opengl = true;
	int16_t mode = 2;
	LuxRect region = {0, 0, 512, 384, 0};

	ObjectEffect initial_effect;
	Player * controller;

	bool Shapes( ObjectEffect effect );
	bool Sprites( ObjectEffect effect );

	void end();
	void loop();
	void start();

}

std::string ColorString( LuxColour c )
{
	std::string color_string;
	std::ostringstream code;

	//code.flags(std::ios::hex);

	code << (int)c.r << "," << (int)c.b << "," << (int)c.g << "," <<(int)c.a;


	color_string = code.str();
	code.str("");

	return color_string;

}

bool luxtest::Shapes( ObjectEffect effect )
{

	LuxRect text_position = {0,0,0,0,0};
	LuxRect rectangle_position = { 40, 40, 40,60, 0 };
	LuxRect circle_position = { 81, 40, 40, 60, 0};
	LuxRect line_position = { 169, 140, 10,160, 0};
	LuxRect polygon_position = { 260, 40, 10,160, 0};

	int16_t polygon_x_points[4] = { 20, 40, 20, 0 };
	int16_t polygon_y_points[4] = { 0, 80, 50, 80 };


	luxtest::display->graphics.DrawRect( rectangle_position, effect );
	luxtest::display->graphics.DrawCircle( circle_position, effect );
	luxtest::display->graphics.DrawLine( line_position, effect );
	luxtest::display->graphics.DrawPolygon( polygon_x_points, polygon_y_points, 4, polygon_position, effect, NULL );

	luxtest::display->graphics.DrawText( "\u2776 Rotate\n"
										 "\u2777 Scale\n"
										 "\u2778 Style:" + style_names[effect.style] +"\n"
										 "\u2779 Color:" + ColorString(effect.primary_colour),
										 text_position, effect, false );
	return true;
}

bool luxtest::Sprites( ObjectEffect effect )
{
	LuxRect text_position = {0,0,0,0,0};
	LuxRect single_sprite_position = { 40, 80, 0, 0, 0 };
	LuxRect tile_sprite_position = { 100, 80, 164,109, 0};

	LuxRect rotate_sprite_position = { 40, 200, 0, 0, 0 };
	ObjectEffect rotate_effect;
	rotate_effect.rotation = 45;

	luxtest::display->graphics.DrawSprite( logo, single_sprite_position, effect );
	//luxtest::display->graphics.DrawRect( tile_sprite_position, effect );
	luxtest::display->graphics.DrawSprite( logo, tile_sprite_position, effect );



	luxtest::display->graphics.DrawSprite( logo, rotate_sprite_position, effect );
	luxtest::display->graphics.DrawSprite( logo, rotate_sprite_position, rotate_effect );


	luxtest::display->graphics.DrawText( "\u2776 Rotate\n"
										 "\u2777 Scale\n"
										 "\u2778 Style:" + style_names[effect.style] +"\n"
										 "\u2779 Color:" + ColorString(effect.primary_colour),
										 text_position, effect, false );
	return true;
}

void luxtest::start()
{
	lux::config = new GameConfig();
	lux::config->SetString("window.title", "Test Mode");

	luxtest::display = new DisplaySystem( );
	luxtest::region = luxtest::display->screen_dimension;

	logo = luxtest::display->graphics.PNGtoSprite(lux_media::testimage, lux_media::testimage_size);

	controller = new Player(1,LOCAL);
	lux::engine->AddPlayer(controller);


	initial_effect.tile_object = true;
}


void luxtest::loop()
{
	lux::core->RefreshInput( luxtest::display );
	controller->Loop();

	uint16_t key_event = 0;
	while ( lux::core->InputLoopGet(luxtest::display, key_event) )
	{
		if ( key_event == 27 )
		{
			luxtest::mode = 0;
		}
	}

	if ( controller->GetButton(4) == 2 )
	{
		initial_effect.rotation = 0;
		initial_effect.flip_image = 0;
		initial_effect.style = 0;
		initial_effect.primary_colour.r = 255;
		initial_effect.primary_colour.g = 255;
		initial_effect.primary_colour.b = 255;
	}
	if ( controller->GetButton(5) == 2 )
	{
		initial_effect.tile_object = !initial_effect.tile_object;
	}
	if ( controller->GetButton(0) == 2 )
	{
		initial_effect.rotation += 8;
		if ( initial_effect.rotation >= 360 )
			initial_effect.rotation -= 360;

		//lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "rotation: " << initial_effect.rotation << std::endl;
	}

	if ( controller->GetButton(6) == 1 )
	{
		initial_effect.flip_image += 1;
		if ( initial_effect.flip_image == 4 )
			initial_effect.flip_image = 16;
		if ( initial_effect.flip_image >= 20 )
			initial_effect.flip_image -= 20;
		//lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "flip_image: " << (int)initial_effect.flip_image << std::endl;
	}
	if ( controller->GetButton(1) == 1 )
	{
		initial_effect.scale_xaxis += 1000;
		initial_effect.scale_yaxis += 1000;
		if ( initial_effect.scale_yaxis >= 4000 )
		{
			initial_effect.scale_xaxis = 500;
			initial_effect.scale_yaxis = 500;
		}
		//lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "flip_image: " << (int)initial_effect.flip_image << std::endl;
	}
	if ( controller->GetButton(2) == 1 )
	{
		initial_effect.style += 1;
		if ( initial_effect.style >= 7 )
			initial_effect.style -= 7;
	}

	if ( controller->GetButton(3) == 1 )
	{
		initial_effect.primary_colour.r = lux::core->GetTime()%255;
		initial_effect.primary_colour.g = lux::core->GetTime()*3%255;
		initial_effect.primary_colour.b = lux::core->GetTime()*9%255;
	}


	if ( controller->GetButton(PLAYER_POINTER) == 1 )
	{
		luxtest::mode++;
		if ( luxtest::mode > 2 )
			luxtest::mode -= 2;
	}


	luxtest::display->graphics.PreShow(GRAPHICS_SCREEN_FRAME);
	switch ( luxtest::mode )
	{
		case 2:
		{
			Shapes( initial_effect );
			break;
		}
		case 1:
		default:
		{
			Sprites( initial_effect );
			break;
		}
	}
	luxtest::display->graphics.PostShow(GRAPHICS_SCREEN_FRAME);
	lux::core->Idle( );
}

void luxtest::end()
{
	if ( luxtest::display )
		delete luxtest::display;
	if ( lux::config )
		delete lux::config;
	lux::config = NULL;


	lux::engine->RemovePlayer(controller);
	delete controller;
}

bool luxtest::run()
{
	luxtest::start();

	while ( luxtest::mode )
	{
		luxtest::loop();
	}
	luxtest::end();

	return 0;
}
