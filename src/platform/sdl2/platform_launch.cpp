/****************************
Copyright © 2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "core.h"
#include "config.h"

#include <SDL.h>

extern "C" {
	void loopGame()
	{
		lux::engine->Refresh();
	}

	int32_t loadGame( const void * window_ptr, char * file )
	{
		lux::engine = new LuxEngine( window_ptr );
		return lux::engine->Start( file );
	}

	void startGame( )
	{

	}

	void pauseGame()
	{

	}

	int8_t setState( int8_t new_state)
	{
		lux::engine->SetState( (LuxState)new_state );

		return (int8_t)lux::engine->state;
	}

	void endGame()
	{
		lux::engine->Close();
		delete lux::engine;
	}

}


