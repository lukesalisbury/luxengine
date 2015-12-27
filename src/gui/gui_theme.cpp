/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "game_config.h"
#include "luxengine.h"
#include "gui.h"
#include "misc_functions.h"


void UserInterface::SetTheme()
{
	this->css = new CSSParser("dialog { border-style:inset; color:#FFFFFFFF; background-color:#703030FF; min-height: 40px;  }\
imagebutton, button, dialog { \
	border-color:#703030FF;\
	border-width:2px;\
	border-style:outset;\
	color:#FFFFFFFF;\
	background-color:#703030FF;\
	padding: 4px;\
}\
imagebutton, button,{ \
	text-align:center;\
}\
button { border-style:outset; }\
imagebutton:hover, button:hover { \
	border-color:#804040FF;\
	border-style:outset;\
	color:#000000FF;\
	background-color:#804040FF;\
}\
imagebutton:active, button:active { \
	border-color:#905050FF;\
	border-style:solid;\
	color:#000000FF;\
	background-color:#804040FF;\
}\
text { color: #eeeeeeFF; } \
throbber { border-color: #000000FF; color:#000000FF; background-color:#FFFFFFFF; } \
throbber:pressed  { border-color: #444444FF; } \
throbber:hover  { border-color: #888888FF; } \
throbber:clicked  { border-color: #444444FF; } \
guibackground { background-color: #007DBDFF; }\
checkbox { color:#000000FF; background-color:#FFFFFFFF; } \
checkbox:disabled  { background-color: #AAAAAA99;} \
checkbox:pressed  { color: #554444FF; background-color: #DDDDDDFF;} \
checkbox:hover, checkbox:activehover  {background-color: #554444FF;} \
imagebutton { min-height:36px; } \
inputtext { border-width:1px; background-color: #223333FF; border-color: #444444FF; border-style:inset; } \
");


	if ( lux::config )
	{
		if ( lux::config->Has("gui.style") )
		{
			std::string css = lux::config->GetString("gui.style");
			this->css->AddContent( css );
		}
	}

	if ( this->css->HasKey(GUIBACKGROUND, ENABLED, "background-color") )
	{
		this->SetBackground( this->css->GetEffect(GUIBACKGROUND, ENABLED) );
	}

}

