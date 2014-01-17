/****************************
Copyright © 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "portal.h"
#include "core.h"
#include "config.h"

#define MODULE_EXPORT extern "C"


MODULE_EXPORT int32_t luxengine_sogl_startgame( char * uri )
{
    if ( lux::engine )
    {
        std::string game_path = uri;
        return lux::engine->Start(game_path);
    }
    return 999;
}

MODULE_EXPORT int32_t luxengine_sogl_loopgame( )
{
    if ( lux::engine != NULL )
    {
        lux::engine->Refresh();
        return (int32_t)lux::engine->state;
    }
    return -999;
}

MODULE_EXPORT int32_t luxengine_sogl_pausegame( )
{


    return 999;
}

MODULE_EXPORT int32_t luxengine_sogl_closegame( )
{
    if ( lux::engine != NULL )
    {
        lux::engine->Close();
    }
    return 999;
}

MODULE_EXPORT int32_t luxengine_sogl_load()
{
    lux::engine = new LuxEngine("./");
    return (lux::engine != NULL);
}

MODULE_EXPORT int32_t luxengine_sogl_unload()
{
    if ( lux::engine != NULL )
    {
        delete lux::engine;
        lux::engine = NULL;
    }
    return 999;
}
