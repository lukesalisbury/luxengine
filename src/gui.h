/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _GUI_H_
#define _GUI_H_

#include "luxengine.h"
#include "display.h"
#include "gui_types.h"
#include "widget.h"
#include "player.h"
#include "css.h"

namespace colour {
	extern LuxColour white;
	extern LuxColour black;
	extern LuxColour red;
	extern LuxColour blue;
	extern LuxColour green;
	extern LuxColour yellow;
}

class UserInterface
{
	public:
		UserInterface( DisplaySystem * display );
		~UserInterface();

		LuxRect ui_region;

	private:
		ObjectEffect background;
		DisplaySystem * internal_display;
		std::list<Widget *> children_widget;
		std::list<Widget *>::iterator children_iter;

		Widget * active_child, * main_widget;
		int16_t last_mouse;
		Player * controller;
		CSSParser * css;
		LuxPolygon * cursor;

		std::map<uint32_t, LuxSprite *> sprites;
		std::map<std::string, LuxSheet *> sheets;

	public:
		void SetRegion( LuxRect region ) { ui_region = region; }

		CSSParser * GetCSSParser();
		Widget * GetMainWidget() { return main_widget; }
		DisplaySystem * GetDisplaySystem() { return internal_display; }

		void SetBackground( ObjectEffect effect );
		int32_t Loop( );
		int32_t Show( );

		void AddChild( Widget * child );
		Widget * AddChild(LuxRect region, LuxWidget type, std::string text );
		Widget * AddWidgetChild( Widget * parent, LuxRect region, LuxWidget type, std::string text = "", uint32_t text_length = 0 );
		Widget * AddWidgetChild( Widget * parent, int32_t x, int32_t y, uint16_t w, uint16_t h, LuxWidget type, std::string text = "");
		void RemoveChild( Widget * child );
		void RemoveAll();
		int32_t ReturnResult();
		void DrawWidget(Widget * child , uint16_t z);


	private:
		LuxSheet * GetSheet( std::string file, uint16_t width, uint16_t height );
		LuxSprite * GetSprite(std::string file , uint16_t width, uint16_t height );
		void SetTheme();
};


namespace lux
{
	extern UserInterface * gui;
}

#endif /* _GUI_H_ */
