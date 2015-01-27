/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "audio.h"
#include "engine.h"
#include "game_config.h"
#include "elix_string.hpp"
#include "mokoi_game.h"
#include "core.h"

/**
 * @brief AudioSystem::AudioSystem
 */
AudioSystem::AudioSystem()
{
	enabled = lux::config->GetBoolean("audio.able");

	this->master_volume = 128;
	this->music_volume = 96;
	this->effects_volume = 96;
	this->dialog_volume = 96;
	this->music = NULL;
	this->primary_dialog = this->secondary_dialog = NULL;

	if ( enabled )
	{
		lux::screen::display("Loading Audio System");

		this->paused = false;

		this->bits = 0;
		this->buffers = 512;
		this->frequency = 22050;
		this->outformat = MIX_DEFAULT_FORMAT;
		this->channels = 2;

		Mix_OpenAudio(this->frequency, this->outformat, this->channels, this->buffers);
		Mix_QuerySpec(&this->frequency, &this->outformat, &this->channels);

		Mix_ReserveChannels(2); // Reserve channels for dialog

		this->bits = this->outformat&0xFF;
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Audio System: " << this->frequency << "Hz " << this->bits << " bit " << ( this->channels > 1 ? "stereo" : "mono" ) << " - Buffer Size: " << this->buffers << " bytes." << std::endl;


		/* Preload Audio Files into memory */
		std::string buffer;
		std::stringstream audiolist_file;
		if ( lux::game_data->GetStream( "./preload/audio.txt", &audiolist_file ) )
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
	}
}

/**
 * @brief AudioSystem::~AudioSystem
 */
AudioSystem::~AudioSystem ( )
{

	for( std::map<std::string, Mix_Chunk*>::iterator iter = this->soundfx.begin(); iter != this->soundfx.end(); ++iter )
	{
		this->UnloadAudio( iter->second );
	}
	this->soundfx.clear();

	this->UnloadAudio( this->primary_dialog );
	this->UnloadAudio( this->secondary_dialog );

	Mix_FreeMusic( this->music );

	Mix_CloseAudio();
}

/**
 * @brief AudioSystem::Loop
 * @param engine_state
 * @return
 */
bool AudioSystem::Loop(LuxState engine_state)
{
	return true;
}

/**
 * @brief AudioSystem::LoadAudio
 * @param filename
 * @return
 */
bool AudioSystem::LoadAudio( std::string filename )
{
	if ( !enabled )
		return false;

	Mix_Chunk * audio = this->ReturnAudio( filename, true );

	return (audio != NULL);
}

/**
 * @brief AudioSystem::UnloadAudio
 * @param audio
 * @return
 */
bool AudioSystem::UnloadAudio( Mix_Chunk * audio )
{
	if ( audio )
	{
		Mix_FreeChunk(audio);
		return true;
	}

	return false;
}

/**
 * @brief AudioSystem::ReturnAudio
 * @param filename
 * @param cache
 * @return
 */
Mix_Chunk * AudioSystem::ReturnAudio( std::string filename, bool cache )
{
	if ( !enabled )
		return NULL;

	uint8_t * data = NULL;
	uint32_t size = lux::game_data->GetFile(filename, &data, false);

	if ( size )
	{
		SDL_RWops * src = SDL_RWFromMem(data, size);
		if ( src )
		{
			Mix_Chunk * audio = Mix_LoadWAV_RW(src, 1);
			if ( cache )
			{
				this->soundfx[filename] = audio;
			}
			return audio;
		}
	}
	return NULL;
}

/**
 * @brief AudioSystem::FindEffect
 * @param name
 * @return
 */
Mix_Chunk * AudioSystem::FindEffect(std::string name)
{
	std::map<std::string,Mix_Chunk*>::iterator p;
	p = this->soundfx.find(name);
	if (p != this->soundfx.end())
	{
		return p->second;
	}

	/* if it's not in cache, load it. */
	return this->ReturnAudio(name, true );

}

/**
 * @brief AudioSystem::EffectLength
 * @param chunk
 * @return
 */
uint32_t AudioSystem::EffectLength( Mix_Chunk * chunk )
{
	uint32_t points = 0;
	uint32_t frames = 0;
	int freq = 0;
	uint16_t fmt = 0;
	int chans = 0;

	/* Chunks are converted to audio device format... */
	if ( !Mix_QuerySpec(&freq, &fmt, &chans) )
		return 0; /* never called Mix_OpenAudio()?! */

	/* bytes / samplesize == sample points */
	points = (chunk->alen / ((fmt & 0xFF) / 8));

	/* sample points / channels == sample frames */
	frames = (points / chans);

	/* (sample frames * 1000) / frequency == play length in ms */
	return (frames * 1000) / freq;
}

/**
 * @brief AudioSystem::PlayEffect
 * @param requestSound
 * @param x
 * @param y
 * @return
 */
int32_t AudioSystem::PlayEffect ( std::string requestSound, int32_t x, int32_t y )
{
	if ( !enabled )
		return 0;

	Mix_Chunk * sample = this->FindEffect( "./soundfx/" + requestSound );
	if ( sample )
	{
		uint32_t len = this->EffectLength( sample );
		int chan = Mix_PlayChannel(-1, sample, 0);

		if ( chan == -1 )
		{
			lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | PlayEffect: " << Mix_GetError() << std::endl;
			return -1;
		}
		else
		{
			//TODO
			//Mix_SetDistance(chan, 0);
			//Mix_SetPosition(chan, Sint16 angle, Uint8 distance);
		}
		return (int32_t)len;
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << requestSound << " not found" << std::endl;
	return 0;
}

/**
 * @brief AudioSystem::PlayDialog
 * @param requestSound
 * @param channel
 * @return
 */
int32_t AudioSystem::PlayDialog( int32_t requestSound, int8_t channel )
{
	if ( !enabled )
		return 0;

	int32_t used_channel;
	uint32_t len = 0;
	uint8_t * data = NULL;
	uint32_t size;
	std::string filename;

	filename.assign( "./dialog/en." );
	filename.append( elix::string::FromInt(requestSound) );
	filename.append( ".ogg" );

	size = lux::game_data->GetFile( filename, &data, false );
	if ( size )
	{
		SDL_RWops * src = SDL_RWFromMem(data, size);
		if ( src )
		{
			this->primary_dialog = Mix_LoadWAV_RW(src, 1);

			len = this->EffectLength( this->primary_dialog );
			used_channel = Mix_PlayChannel(-1, this->primary_dialog, 0);

			if ( used_channel == -1 )
			{
				lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | PlayEffect: " << Mix_GetError() << std::endl;
				return -1;
			}
			else
			{
				//TODO
				//Mix_SetPanning(int channel, Uint8 left, Uint8 right)
				//Mix_SetDistance(used_channel, 0);
				//Mix_SetPosition(used_channel, Sint16 angle, Uint8 distance);
			}

		}
	}
	return (int32_t)len;
}

/**
 * @brief AudioSystem::PlayMusic
 * @param requestMusic
 * @param loop
 * @param fadeLength
 * @return
 */
int32_t AudioSystem::PlayMusic( std::string requestMusic, int32_t loop, int32_t fadeLength )
{
	if ( !enabled )
		return 0;

	uint8_t * data = NULL;
	uint32_t size = lux::game_data->GetFile("./music/" + requestMusic, &data, false);

	if ( size )
	{
		SDL_RWops * src = SDL_RWFromMem(data, size);
		if ( src )
		{
			Mix_Music * next_music = Mix_LoadMUS_RW( src, 0 );

			if ( !next_music )
			{
				lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | PlayMusic: '" << requestMusic << "' " << Mix_GetError() << std::endl;
			}
			else
			{
				/* Unload Existing Music */
				if ( this->music )
				{
					Mix_FreeMusic( this->music );
					this->music = NULL;
				}

				if ( Mix_FadeInMusic( next_music, loop, fadeLength ) == -1 )
				{
					Mix_FreeMusic( next_music );
					lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | PlayMusic: '" << requestMusic << "' " << Mix_GetError() << std::endl;
					return 0;
				}
				this->music = next_music;
				return 1;
			}
			return 0;
		}
	}
	lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | PlayMusic Error: '" << requestMusic << "' "  << std::endl;
	return 0;
}

/**
 * @brief AudioSystem::StopDialog
 * @param channel
 */
void AudioSystem::StopDialog(int8_t channel)
{
	Mix_HaltChannel(channel);
}

/**
 * @brief AudioSystem::StopEffects
 */
void AudioSystem::StopEffects()
{
	Mix_HaltChannel(-1);
}

/**
 * @brief AudioSystem::StopMusic
 */
void AudioSystem::StopMusic()
{
	Mix_HaltMusic();
}

/**
 * @brief AudioSystem::PauseDialog
 * @param channel
 */
void AudioSystem::PauseDialog(int8_t channel)
{
	if ( Mix_Paused(channel) )
		Mix_Pause(channel);
	else
		Mix_Resume(channel);

}

/**
 * @brief AudioSystem::PauseEffects
 */
void AudioSystem::PauseEffects()
{
	if ( Mix_Paused(-1) )
		Mix_Pause(-1);
	else
		Mix_Resume(-1);
}

/**
 * @brief AudioSystem::PauseMusic
 */
void AudioSystem::PauseMusic()
{
	if ( Mix_PausedMusic() )
		Mix_ResumeMusic();
	else
		Mix_PauseMusic();
}


/**
 * @brief AudioSystem::SetMasterVolume
 * @param volume
 * @return
 */
int32_t AudioSystem::SetMasterVolume(int32_t volume)
{
	if ( !enabled )
		return 0;

	if ( volume >= 0 )
	{
		this->master_volume = clamp(volume, 0, 128 );
	}
	return this->master_volume;
}

/**
 * @brief AudioSystem::SetMusicVolume
 * @param volume
 * @return
 */
int32_t AudioSystem::SetMusicVolume(int32_t volume)
{
	if ( !enabled )
		return 0;

	if ( volume >= 0 )
	{
		this->music_volume = clamp(volume, 0, 128 );

		Mix_VolumeMusic(this->music_volume);
	}
	return this->music_volume;

}

/**
 * @brief AudioSystem::SetEffectsVolume
 * @param volume
 * @return
 */
int32_t AudioSystem::SetEffectsVolume(int32_t volume)
{
	if ( !enabled )
		return 0;

	if ( volume >= 0 )
	{
		this->effects_volume = clamp(volume, 0, 128 );
		Mix_Volume(-1, this->effects_volume);
	}
	return this->effects_volume;

}


/**
 * @brief AudioSystem::SetDialogVolume
 * @param volume
 * @return
 */
int32_t AudioSystem::SetDialogVolume(int32_t volume)
{
	if ( !enabled )
		return 0;

	if ( volume >= 0 )
	{
		this->dialog_volume = clamp(volume, 0, 128 );

		Mix_Volume(0, this->dialog_volume);
		Mix_Volume(1, this->dialog_volume);
	}
	return this->dialog_volume;
}

/**
 * @brief AudioSystem::PauseAll
 */
void AudioSystem::PauseAll()
{
	if ( !enabled )
		return;

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
}

