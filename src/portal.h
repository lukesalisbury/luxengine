/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _PORTAL_H_
	#define _PORTAL_H_

#include <test_code.h>
	namespace LuxPortal {
		extern bool active;
		extern bool use;
		extern bool testmode;
		extern bool opengl;
		bool run();
		int32_t recent_page(LuxRect page_rect);
		int32_t online_page(LuxRect page_rect);
		int32_t browse_page(LuxRect page_rect);
		int32_t demos_page(LuxRect page_rect);
		int32_t directory_page();
		void add_previous_game( std::string path );
		void open();
		void close();
		void clear_listed_games();

	}
#endif /* _PORTAL_H_ */
