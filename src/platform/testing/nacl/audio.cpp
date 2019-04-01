/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "audio.h"
#include "engine.h"
#include "config.h"
#include "elix/elix_string.hpp"
#include "mokoi_game.h"


void Lux_Audio_DialogEnded()
{
	lux::audio->StopDialog();
}

AudioSystem::AudioSystem()
{
	lux::screen::display("Loading Audio System");
	this->paused = false;
	
	/*
	this->music = nullptr;
	this->bits = 0; 
	this->buffers = 512;
	this->frequency = 22050;

	this->outformat = MIX_DEFAULT_FORMAT;
	this->channels = 2;

	Mix_OpenAudio(this->frequency, this->outformat, this->channels, this->buffers);
	Mix_QuerySpec(&this->frequency, &this->outformat, &this->channels);

	this->bits = this->outformat&0xFF;
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Audio System: " << this->frequency << "Hz " << this->bits << " bit " << ( this->channels > 1 ? "stereo" : "mono" ) << " - Buffer Size: " << this->buffers << " bytes." << std::endl;
*/
	std::string buffer;
	std::stringstream audiolist_file;
	if ( lux::game->GetStream( "./preload/audio.txt", &audiolist_file ) )
	{
		do
		{
			std::getline( audiolist_file, buffer );
			elix::string::Trim(&buffer);
			if ( !buffer.empty() )
			{
				lux::screen::display("Loading Audio: " + buffer);
				this->LoadAudio("./soundfx/" + buffer);
			}
		} while ( !audiolist_file.eof() );
		audiolist_file.clear();
	}

	if ( !lux::config->GetBoolean("audio.able") )
	{
		this->SetMusicVolume(0);
		this->SetEffectsVolume(0);
	}
}


AudioSystem::~AudioSystem ( )
{
	//this->soundfx.clear();
	//Mix_CloseAudio();
}

bool AudioSystem::Loop()
{
	return true;
}


bool AudioSystem::LoadAudio(std::string filename)
{
	/*
	uint8_t * data = nullptr;
	uint32_t size;
	size = lux::game->GetFile(filename, &data, false);
	if ( size )
	{
		SDL_RWops * src = SDL_RWFromMem(data, size);
		if ( src )
		{
			this->soundfx[filename] = Mix_LoadWAV_RW(src, 1);
			return true;
		}
	}*/
	return false;
}
/*
Mix_Chunk * AudioSystem::ReturnAudio(std::string filename)
{
	uint8_t * data = nullptr;
	uint32_t size;
	size = lux::game->GetFile(filename, &data, false);
	if ( size )
	{
		SDL_RWops * src = SDL_RWFromMem(data, size);
		if ( src )
		{
			Mix_Chunk * audio = Mix_LoadWAV_RW(src, 1);
			this->soundfx[filename] = audio;
			return audio;
		}
	}

	return nullptr;
}
*/

int32_t AudioSystem::PlayEffect ( std::string requestSound, int32_t x, int32_t y )
{
	/*
	Mix_Chunk * sample = this->FindEffect( "./soundfx/" + requestSound);
	if ( sample == nullptr )
		sample = this->ReturnAudio("./soundfx/" +requestSound);

	if ( sample )
	{
		uint32_t len = this->EffectLength( sample );
		int chan = Mix_PlayChannel(-1, sample, 0);
		
		if ( chan == -1 ) 
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | PlayEffect: " << Mix_GetError() << std::endl;
			return 0;
		}
		else
		{
			Mix_SetDistance(chan, 0);//to do.
			//Mix_SetPosition(chan, Sint16 angle, Uint8 distance);
		}
		return (int32_t)len;
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << requestSound << " not found" << std::endl;
	*/
	return 0;
}

void AudioSystem::StopDialog()
{
	/* Mix_FreeMusic should also free up SDL_RWops */
	/*
	Mix_FreeMusic( this->dialog );
	this->dialog = nullptr;
	if ( this->music )
	{
		Mix_PlayMusic(this->music, -1);
		Mix_HookMusicFinished(nullptr);
	}
	*/
}

int32_t AudioSystem::PlayDialog( int32_t requestSound )
{
	/*
	std::ostringstream filename("");
	filename << "./dialog/en." << requestSound << ".ogg" ;
	uint8_t * data = nullptr;
	uint32_t size;
	size = lux::game->GetFile(filename.str(), &data, false);
	if ( size )
	{
		SDL_RWops * src = SDL_RWFromMem(data, size);
		if ( src )
		{
			this->dialog = Mix_LoadMUS_RW(src);
			if ( Mix_PlayMusic(this->dialog, 1) == -1 )
			{
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | PlayDialog: " << Mix_GetError() << std::endl;
				return 0;
			}
			Mix_HookMusicFinished( &Lux_Audio_DialogEnded );
			return 1;
		}
	}
	*/
	return 0;
}

int32_t AudioSystem::PlayMusic ( std::string requestMusic, int32_t loop, int32_t fadeLength )
{
	/*
	uint8_t * data = nullptr;
	uint32_t size;
	size = lux::game->GetFile("./music/" + requestMusic, &data, false);
	if ( size )
	{
		SDL_RWops * src = SDL_RWFromMem(data, size);
		if ( src )
		{
			if ( this->music )
			{
				Mix_FreeMusic( this->music );
				this->music = nullptr;
			}

			this->music = Mix_LoadMUS_RW( src );

			if ( !this->music )
			{
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | PlayMusic: '" << requestMusic << "' " << Mix_GetError() << std::endl;
			}

			if ( Mix_FadeInMusic( this->music, loop, fadeLength ) == -1 )
			{
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | PlayMusic: '" << requestMusic << "' " << Mix_GetError() << std::endl;
				return 0;
			}
			return 1;
		}
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | PlayMusic Error: '" << requestMusic << "' "  << std::endl;
	*/
	return 0;
}

int32_t AudioSystem::SetMusicVolume(int32_t volume)
{
	//return Mix_VolumeMusic(volume);
	return 0;
}

int32_t AudioSystem::SetEffectsVolume(int32_t volume)
{
	//return Mix_Volume(-1, volume);
	return 0;
}

void AudioSystem::Pause()
{
	/*
	if ( this->paused )
	{
		Mix_ResumeMusic();
		Mix_Resume(-1);
		this->paused = false;
	}
	else
	{
		Mix_PauseMusic();
		Mix_Pause(-1);
		this->paused = true;
	}
	*/
}
/*
Mix_Chunk * AudioSystem::FindEffect(std::string name) 
{
	std::map<std::string,Mix_Chunk*>::iterator p;
	p = this->soundfx.find(name);
	if (p != this->soundfx.end())
	{
		return p->second;
	}
	return nullptr;
}

uint32_t AudioSystem::EffectLength( Mix_Chunk * chunk )
{
	uint32_t points = 0;
	uint32_t frames = 0;
	int freq = 0;
	uint16_t fmt = 0;
	int chans = 0; 
	

	if ( !Mix_QuerySpec(&freq, &fmt, &chans) )
		return 0; 
	points = (chunk->alen / ((fmt & 0xFF) / 8)); 
	frames = (points / chans);
	return (frames * 1000) / freq; 
}
*/
