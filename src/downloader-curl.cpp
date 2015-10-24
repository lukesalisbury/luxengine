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
#include "gui/gui.h"
#include "gui/widget.h"
#include "tinyxml/tinyxml2ext.h"
#include "worker.h"
#include "core.h"
#include "config.h"
#include "entity.h"
#include "elix/elix_path.hpp"
#include "elix/elix_string.hpp"
#include "elix/elix_file.hpp"
#ifdef NO_ZLIB
	#define MINIZ_HEADER_FILE_ONLY
	#include "miniz.c"
#else
	#include <zlib.h>
#endif
#include <sstream>

#include <curl/curl.h>
#define RCVBUFSIZE 32

struct DownloadRequest
{
	Widget * dialog;
	std::string url, file;
};

struct MemoryStruct {
	char *memory;
	size_t size;
};

uint32_t Lux_Util_FileCheck( std::string filename )
{
	elix::File * file = new elix::File( filename );
	uint8_t buffer[RCVBUFSIZE];
	uint32_t crc = crc32(0L, Z_NULL, 0);
	uint32_t count = RCVBUFSIZE;
	if ( file->Exist() )
	{
		while ( (count = file->Read2( buffer, sizeof(uint8_t), RCVBUFSIZE )) > 0 )
		{
			crc = crc32(crc, buffer, count);
		}
	}
	delete file;
	return crc;
}

static size_t Lux_Util_CurlWriteMemory(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
	/* out of memory! */
	printf("not enough memory (realloc returned NULL)\n");
	return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

size_t Lux_Util_CurlWrite(void *ptr, size_t size, size_t nmemb, elix::File * stream)
{
	return stream->Write( ptr, size, nmemb ) * (size * nmemb);
}

int Lux_Util_CurlProgress(DownloadRequest * request, double t, double d, double ultotal, double ulnow)
{
/*  printf("%d / %d (%g %%)\n", d, t, d*100.0/t);*/
	std::stringstream message;
	message.setf(std::ios_base::fixed);
	message.precision(0);
	message << "Downloading\n" << request->url << "\nDownloaded " << d << " of " << t << " bytes" << std::endl;

	request->dialog->SetText(message.str(), message.str().length());

	return 0;
}

CURLcode Lux_Util_FileDownloaderHandler( DownloadRequest * request )
{
	CURL * curl;
	CURLcode res = CURLE_UNSUPPORTED_PROTOCOL;
	elix::File * output_file = NULL;

	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Downloading " << request->url << " to " << request->file  << std::endl;


	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if ( curl )
	{
		output_file = new elix::File( request->file, true );

		curl_easy_setopt( curl, CURLOPT_URL, request->url.c_str() );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, output_file );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, Lux_Util_CurlWrite );
		curl_easy_setopt( curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt( curl, CURLOPT_PROGRESSFUNCTION, Lux_Util_CurlProgress);
		curl_easy_setopt( curl, CURLOPT_PROGRESSDATA, request );
		curl_easy_setopt( curl, CURLOPT_USERAGENT, PROGRAM_NAME"/"PROGRAM_VERSION );
		curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );
		curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0 );

		res = curl_easy_perform(curl);

		/* Check for errors */
		if ( res != CURLE_OK )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);

		delete output_file;
	}

	curl_global_cleanup();

	return res;
}

int32_t Lux_Util_FileDownloaderThread( void * data )
{

	WorkerThread * thread = (WorkerThread *)data;
	DownloadRequest * request = (DownloadRequest *)thread->data;

	CURLcode res;

	res = Lux_Util_FileDownloaderHandler( request );

	thread->_active = false;
	thread->results = ( res == CURLE_OK );

	return ( res == CURLE_OK );
}

int32_t Lux_Util_FileDownloader( std::string urlArg, std::string origFile, UserInterface * ui )
{
	CURLcode res;
	DownloadRequest request;

	request.dialog = NULL;
	request.url = urlArg;
	request.file = origFile;

	if ( ui )
	{
		LuxRect region = {ui->ui_region.w, ui->ui_region.h, ui->ui_region.w, 140, 0 };
		region.x /= 4;
		region.y = (region.y / 2) - 25;
		region.w /= 2;
		request.dialog = ui->AddChild(region, THROBBER, "Downloading\n" + urlArg + "\n");
		request.dialog->SetText("Downloading\n" + urlArg + "\n", urlArg.length() );


		ui->Loop();
	}

	res = Lux_Util_FileDownloaderHandler( &request );

	if ( ui )
	{
		ui->Loop();
		ui->RemoveChild( request.dialog );
	}

	delete request.dialog;

	return ( res == CURLE_OK );
}

int32_t Lux_Util_FileDownloaderEntityCallback( void * data )
{

	WorkerThread * thread = (WorkerThread *)data;
	DownloadEntityRequest * request = (DownloadEntityRequest *)thread->data;

	MemoryStruct chunk;

	CURL * curl;
	CURLcode res;

	chunk.memory = (char*)malloc(1);
	chunk.size = 0;


	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if ( curl )
	{

		curl_easy_setopt( curl, CURLOPT_URL, request->url.c_str() );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, &chunk );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, Lux_Util_CurlWriteMemory );
		curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );


		res = curl_easy_perform(curl);

		/* Check for errors */
		if ( res != CURLE_OK )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();


	thread->_active = false;
	thread->results = ( res == CURLE_OK );

	if (chunk.size )
	{
		if ( request->entity )
		{
			thread->Lock();
			request->entity->Call( request->callback, "sd", chunk.memory, chunk.size );
			thread->Unlock();
		}
	}

	if ( chunk.memory )
		free(chunk.memory);

	return ( res == CURLE_OK );

}

int32_t Lux_Util_FileDownloaderBackground( std::string urlArg, std::string origFile, UserInterface * ui )
{
	int32_t res = 0;
	LuxRect region = {ui->ui_region.w, ui->ui_region.h, ui->ui_region.w, 50, 0 };
	region.x /= 4;
	region.y = (region.y / 2) - 25;
	region.w /= 2;

	DownloadRequest request;
	request.dialog = ui->AddChild( region, THROBBER, "Downloading " + urlArg + "\n");
	request.url = urlArg;
	request.file = origFile;

	WorkerThread * thread = new WorkerThread(Lux_Util_FileDownloaderThread, (void*) &request);

	while ( thread->Active() )
	{
		ui->Loop();
	}

	res = thread->results;

	ui->RemoveChild(request.dialog);

	delete thread;
	return res;

}





