/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <SDL/SDL.h>
#include "../../luxengine.h"
#include "../../widget.h"
#include "../../gui.h"
#include "../../misc_functions.h"

int32_t UserInterface::Loop()
{
	if ( !this->_display )
		return 0;

	SDL_Event event;
	std::list<Widget *>::iterator wid_iter;
	int32_t return_value = 0;
	uint8_t delay_time = 5;
	SDL_EnableUNICODE(1);
	int32_t mousex, mousey;
	while ( SDL_PollEvent(&event) )
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				if ( event.key.keysym.sym == SDLK_TAB )
				{
					wid_iter = this->_children.begin();
					while( wid_iter != this->_children.end())
					{
						if ((*wid_iter)->_focus == true)
						{
							if (wid_iter == this->_children.end())
							{
								(*wid_iter)->_focus = false;
								(*this->_children.begin())->_focus = true;
							}
							else
							{
								wid_iter++;
								(*wid_iter)->_focus = true;
								break;
							}
						}
						else
						{
							(*wid_iter)->_focus = false;
							wid_iter++;
						}
					}
				}
				else
				{
					if ( this->_activechild != NULL )
					{
						if ( !this->_activechild->SendEvent( event.key.keysym.unicode ) )
						{
							return_value = this->_activechild->GetValue();
						}
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP: 
			case SDL_MOUSEMOTION: 
				wid_iter = this->_children.begin();
				mousex = event.motion.x;
				mousey = event.motion.y;
				if ( this->_display->Display2Screen )
				{
					this->_display->Display2Screen(&mousex, &mousey);
				}
				
				while( wid_iter != this->_children.end())
				{
					if ( Lux_Util_PointCollide((*wid_iter)->_region, mousex, mousey) )
					{
						if (event.type == SDL_MOUSEBUTTONDOWN)
						{
							(*wid_iter)->SetState(PRESSED);
						}
						else if (event.type == SDL_MOUSEBUTTONUP)
						{
							(*wid_iter)->SetState(CLICKED);
							if ( !(*wid_iter)->SendEvent(1) )
							{
								return_value = (*wid_iter)->GetValue();
							}
							else
							{
								this->_activechild = (*wid_iter);
							}
						}
						else
						{
							(*wid_iter)->SetState(HOVER);
						}
					}
					else
					{
						(*wid_iter)->ResetState();
					}
					wid_iter++;
				}
				break;
			case SDL_QUIT:
			{
					if ( this->_activechild )
						return_value = this->_activechild->GetValue();
					else
						return_value = 0;
				break;
			}
			case SDL_ACTIVEEVENT: 
			{
				if ( event.active.state & SDL_APPACTIVE) {
					if ( event.active.gain )
						delay_time = 10;
					else
						delay_time = 100;
				}
				break;
			}
				default:
				break;
		}
	}
	if ( this->_activechild )
		this->_activechild->SetState(ACTIVE);
	this->_display->DrawRect(this->_region, this->background);
	if (this->_children.size())
	{
		for ( wid_iter = this->_children.begin(); wid_iter != this->_children.end(); wid_iter++ )
		{
			this->DrawWidget((*wid_iter));
		}
	}
	this->_display->Show();
	SDL_Delay(delay_time);
	return return_value;
}

int32_t UserInterface::Show()
{
	if ( !this->_display )
		return 1;
	std::list<Widget *>::iterator wid_iter;

	this->_display->DrawRect(this->_region, this->background);
	if ( this->_children.size() )
	{
		for ( wid_iter = this->_children.begin(); wid_iter != this->_children.end(); wid_iter++ )
		{
			this->DrawWidget((*wid_iter));
		}
	}
	this->_display->Show();
	return 1;
}

