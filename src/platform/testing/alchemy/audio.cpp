/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "../luxengine.h"
#include "../functions.h"
#include "../gamepack.h"


AudioSystem::AudioSystem ( )
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Audio System" << std::endl;
}

AudioSystem::~AudioSystem ( )
{
}

bool AudioSystem::Init()
{
	return true;
}

bool AudioSystem::LoadAudio(std::string name)
{
	return false;
}

bool AudioSystem::PlayEffect ( std::string requestSound )
{
	return false;
}
bool AudioSystem::PlayDialog ( int requestSound )
{
	return false;
}
bool AudioSystem::PlayMusic ( std::string requestMusic, int loop, int fadeLength )
{
	return false;
}

int AudioSystem::SetMusicVolume(int volume)
{
	return 0;
}

int AudioSystem::SetEffectsVolume(int volume)
{
	return 0;
}
void AudioSystem::Pause()
{

}


