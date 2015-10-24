/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "../../engine.h"
#include "../../misc_functions.h"
#include "../../mokoi_game.h"
#include "audio.h"
#include <nds.h>
#include <maxmod9.h>

AudioSystem::AudioSystem ( )
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Audio System" << std::endl;
	mm_ds_system sys;
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit( &sys );
}

AudioSystem::~AudioSystem ( )
{

}

bool AudioSystem::Init()
{

	return true;
}

bool AudioSystem::Loop()
{
	mmStreamUpdate();
	return true;
}

bool AudioSystem::Close()
{

	return true;
}


bool AudioSystem::LoadAudio( std::string name )
{
	return false;
}

int32_t AudioSystem::PlayEffect( std::string requestSound, int32_t x, int32_t y )
{

	return true;
}

int32_t AudioSystem::PlayDialog( int requestSound )
{
	return false;
}

int32_t AudioSystem::PlayMusic( std::string requestMusic, int loop, int fadeLength )
{

	return false;
}

int AudioSystem::SetMusicVolume( int volume )
{
	return 0;
}

int AudioSystem::SetEffectsVolume( int volume )
{
	return 0;
}

void AudioSystem::Pause()
{

}

void AudioSystem::StopDialog()
{
}
