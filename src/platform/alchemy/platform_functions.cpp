/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "../lux_types.h"

std::string Lux_Util_GetBaseFolder(std::string path)
{
	int lastslash = path.find_last_of( "/", path.length() );
	if (lastslash >= 1)
	{
		return path.substr(0, lastslash + 1);
	}
	else if (lastslash == -1 )
	{
		return "./";
	}
	return path;
}

std::string Lux_Util_GetGlobalPath()
{
	return "/";
}

std::string Lux_Util_GetUserPath()
{
	return "/";
}

std::string Lux_Util_GetTempPath()
{
	return "/";
}

bool Lux_Util_ValidPath(std::string path)
{
	return true;
}
