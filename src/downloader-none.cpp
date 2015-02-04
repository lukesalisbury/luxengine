/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "misc_functions.h"
#include "gui.h"
#include "widget.h"
#include "tinyxml/tinyxml2ext.h"
#include "worker.h"
#include "core.h"
#include "config.h"
#include "elix_path.hpp"
#ifdef NO_ZLIB
	#define MINIZ_HEADER_FILE_ONLY
	#include "miniz.c"
#else
	#include <zlib.h>
#endif

int32_t Lux_Util_SocketInit(int &sockDesc)
{
	return 0;
}

void Lux_Util_SocketClose(int &sockDesc)
{

}

uint32_t Lux_Util_FileCheck( std::string filename )
{
	elix::File * file = new elix::File( filename );
	uint8_t buffer[32];
	uint32_t crc = crc32(0L, Z_NULL, 0);
	uint32_t count = 32;
	if ( file->Exist() )
	{
		while ( (count = file->Read2( buffer, sizeof(uint8_t), 32 )) > 0 )
		{
			crc = crc32(crc, buffer, count);
		}
	}
	delete file;
	return crc;
}

int32_t Lux_Util_FileDownloader( std::string urlArg, std::string origFile, UserInterface * ui )
{
	return 0;
}


int32_t Lux_Util_FileDownloaderThread( void * data )
{
	WorkerThread * thread = (WorkerThread *)data;
	thread->_active = false;
	thread->results = 0;
	return 0;
}

int32_t Lux_Util_FileDownloaderEntityCallback( void * data )
{
	WorkerThread * thread = (WorkerThread *)data;
	thread->_active = false;
	return 0;
}

int32_t Lux_Util_FileDownloaderBackground( std::string urlArg, std::string origFile, UserInterface * ui )
{
	LuxRect region = {10, (ui->ui_region.h/2)-25, (ui->ui_region.w-20), 50, 0 };
	Widget * dialog = ui->AddChild( region, DIALOGOK, "Downloading code has been disabled");

	while ( !ui->Loop() )
	{

	}

	ui->RemoveChild(dialog);

	return 0;
}





