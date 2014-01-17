/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "base_audio.h"


class AudioSystem: public BaseAudioSystem
{
	public:
		AudioSystem();
		~AudioSystem();

		bool Init();
		bool Loop();
		bool Close();

		/* Resource Management */
		bool LoadAudio(std::string filename);

		/* Playing */
		int32_t PlayEffect( std::string requestSound, int32_t x = -1, int32_t y = -1 );
		int32_t PlayDialog( int32_t requestSound );
		int32_t PlayMusic( std::string requestMusic, int32_t loop, int32_t fadeLength );
		void Pause();
		void StopDialog();

		/* Volume */
		int SetMusicVolume( int32_t volume);
		int SetEffectsVolume( int32_t volume);
};

namespace lux
{
	extern AudioSystem * audio;
}
#endif
