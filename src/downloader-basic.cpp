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
#ifdef NO_ZLIB
	#define MINIZ_HEADER_FILE_ONLY
	#include "miniz.c"
#else
	#include <zlib.h>
#endif
#include <sstream>

#ifdef __GNUWIN32__
	#include <winsock.h>
#elif __NDS__
	#include <nds.h>
	#include <dswifi9.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <netinet/in.h>
#elif __3DS__
	#include <3ds.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <netinet/in.h>
#elif defined (__GAMECUBE__) || defined (__WII__)
	#include <ogcsys.h>
	#include <gccore.h>
	#include <network.h>
#elif DREAMCAST
	#include <kos.h>
	#include <sys/socket.h>
	#include <sys/queue.h>
	#include <netinet/in.h>
	#define SOCK_STREAM 1
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <netinet/in.h>
#endif

#define RCVBUFSIZE		32

const std::string lux_http_split = "\r\n\r\n";

struct HeaderInfo
{
	std::map<std::string, std::string> response;
	std::string text;
	uint16_t status;
};

struct DownloadRequest
{
	Widget * dialog;
	std::string url, file;
};

/*****************************
* Lux_Util_FillAddress
* Fill in 'addr'
*/
std::string Lux_Util_FillAddress(std::string address, uint16_t port, sockaddr_in &addr)
{

	memset(&addr, 0, sizeof(addr));

	std::string header;
	std::string path = "/";
	std::string::size_type cut_at = address.find_first_of("/");
	if ( cut_at != address.npos )
	{
		path = address.substr(cut_at);
		address = address.erase(cut_at);
	}

	cut_at = address.find_first_of(":");

	if ( cut_at != address.npos )
	{
		uint16_t port_t = (uint16_t)atoi(address.substr(cut_at+1).c_str());
		if ( port_t > 80 )
			port = port_t;
		address = address.erase(cut_at);
	}
	header = "GET " + path + " HTTP/1.0\r\nAccept: */*\r\nHost: " + address + "\r\nUser-Agent:LuxEngine " PROGRAM_VERSION "\r\n\r\n";
	addr.sin_family = AF_INET;

	#if defined (__GAMECUBE__) || defined (__WII__)
		hostent * host = net_gethostbyname( address.c_str() );
	#else
		hostent * host = gethostbyname( address.c_str() );
	#endif
	if ( host )
		addr.sin_addr.s_addr = *((uint32_t *) host->h_addr_list[0]);
	else
		addr.sin_addr.s_addr = 0;
	addr.sin_port = htons(port);

	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Downloading:" << path << " from " << address << ":" << port<< std::endl;

	return header;
}

/*****************************
* Lux_Util_HTTPConnect
*
*/
void Lux_Util_HTTPConnect(std::string requestHeader, int32_t sockDesc, sockaddr_in &serv_addr )
{
	#if defined (__GAMECUBE__) || defined (__WII__)
	net_connect(sockDesc, (struct sockaddr*)&serv_addr, sizeof serv_addr);
	net_send(sockDesc, requestHeader.c_str(), requestHeader.length(), 0);
	#else
	connect(sockDesc, (struct sockaddr*)&serv_addr, sizeof serv_addr);
	send(sockDesc, requestHeader.c_str(), requestHeader.length(), 0);
	#endif
}

/*****************************
* Lux_Util_ParseHTTPHeader
*
*/
uint16_t Lux_Util_ParseHTTPHeader( HeaderInfo & header )
{
	std::map<std::string, std::string> array;
	std::vector<std::string> lines;
	std::string::size_type slipt_point;

	elix::string::Split( header.text, "\r\n", &lines );
	if ( lines.size() )
	{
		for( std::string::size_type i = 0; i < lines.size(); i++ )
		{
			if ( i == 0 )
			{
				std::stringstream stream(lines[i].substr(9, 3));
				stream >> header.status;
				lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Header: '" << lines[i].substr(5,3) << "' '" << lines[i].substr(9, 3) << "' '" << lines[i].substr(13) << "'" << std::endl;
			}
			else
			{
				slipt_point = lines[i].find_first_of(':', 0);
				if ( std::string::npos != slipt_point )
				{
					header.response[lines[i].substr(0, slipt_point)] = lines[i].substr(slipt_point+2);
					lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Header: '" << lines[i].substr(0, slipt_point) << "' '" << lines[i].substr(slipt_point+2) << "'" << std::endl;
				}
			}
		}
	}
	return header.status;
}

/*****************************
* Lux_Util_ReadHTTPHeader
*
*/
uint32_t Lux_Util_ReadHTTPHeader(int32_t sockDesc, HeaderInfo & header, std::string & urlData )
{
	int32_t length = -1;
	std::string::size_type cut_at = 0;

	uint8_t rev[RCVBUFSIZE];

	#if defined (__GAMECUBE__) || defined (__WII__)
	while ( ( length = net_recv(sockDesc, (char*)rev, RCVBUFSIZE, 0)) > 0 )
	#else
	while ( ( length = recv(sockDesc, (char*)rev, RCVBUFSIZE, 0)) > 0 )
	#endif
	{
		urlData.append((char*)rev, length);
		memset(rev, 0, RCVBUFSIZE);

		cut_at = urlData.find(lux_http_split);
		if ( cut_at != urlData.npos )
		{
			header.text = urlData.substr( 0, cut_at );
			urlData.erase( 0, cut_at+4 );

			return (Lux_Util_ParseHTTPHeader( header ) == 200);
		}
	}

	return false;
}



/*****************************
* Lux_Util_ReadHTTPData
*
*/
bool Lux_Util_ReadHTTPData(int32_t sockDesc, std::string & urlData, uint32_t & count )
{
	int32_t length = -1;

	uint8_t rev[RCVBUFSIZE];

	#if defined (__GAMECUBE__) || defined (__WII__)
	length = net_recv(sockDesc, (char*)rev, RCVBUFSIZE, 0);
	#else
	length = recv(sockDesc, (char*)rev, RCVBUFSIZE, 0);
	#endif
	if ( length > 0 )
	{
		urlData.append((char*)rev, length);
		memset(rev, 0, RCVBUFSIZE);
		count += length;
		return true;
	}
	return false;
}

int32_t Lux_Util_SocketInit(int &sockDesc)
{
	#ifdef __GNUWIN32__
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		return 0;
	}
	#endif

	#ifdef __NDS__
	if ( !Wifi_InitDefault(WFC_CONNECT) )
	{
		return 0;
	}
	if ( (sockDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		return 0;
	}
	#elif defined (__GAMECUBE__) || defined (__WII__)
	if ( (sockDesc = net_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
	{
		return 0;
	}
	#else
	if ( (sockDesc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
	{
		return 0;
	}
	#endif
	return 1;
}

void Lux_Util_SocketClose(int &sockDesc)
{
	#ifdef __GNUWIN32__
	closesocket(sockDesc);
	WSACleanup();
	#elif __NDS__
	closesocket(sockDesc);
	#elif defined (__GAMECUBE__) || defined (__WII__)

	#else
	close(sockDesc);
	#endif
}

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

int32_t Lux_Util_FileDownloader( std::string urlArg, std::string origFile, UserInterface * ui )
{
	DownloadRequest request;
	struct sockaddr_in serv_addr;

	int32_t ret = 0;
	int32_t sockDesc = 0;
	uint32_t length_count = 0;
	HeaderInfo header;
	std::string urlData = "";
	std::string requestHeader = "";

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

	if ( Lux_Util_SocketInit(sockDesc) )
	{
		requestHeader = Lux_Util_FillAddress(urlArg, 80, serv_addr);
		Lux_Util_HTTPConnect( requestHeader, sockDesc, serv_addr );
		if ( !Lux_Util_ReadHTTPHeader( sockDesc, header, urlData ) )
		{
			if ( header.status == 302 )
			{
				std::string new_location = header.response["Location"].substr(7);
				lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "downloaded moved to " << new_location << std::endl;
				requestHeader = Lux_Util_FillAddress( new_location, 80, serv_addr );

				urlData.clear();
				header.response.clear();
				header.text.clear();

				Lux_Util_SocketClose(sockDesc);
				Lux_Util_SocketInit(sockDesc);
				Lux_Util_HTTPConnect( requestHeader, sockDesc, serv_addr );
				Lux_Util_ReadHTTPHeader( sockDesc, header, urlData );
			}
		}
		if ( header.status == 200 )
		{
			while ( Lux_Util_ReadHTTPData( sockDesc, urlData, length_count ) )
			{
				request.dialog->SetText("Downloading\n" + urlArg + "\n", urlArg.length() );
				ui->Loop();
			}

			if ( urlData.length() )
			{
				request.dialog->SetText("Save File to " + origFile );
				elix::File * file = new elix::File( origFile, true );
				file->WriteString( urlData );
				delete file;
				ret = 1;
			}
		}
	}
	if ( ui )
	{
		ui->RemoveChild( request.dialog );
	}
	Lux_Util_SocketClose(sockDesc);

	delete request.dialog;
	return ret;
}


int32_t Lux_Util_FileDownloaderThread( void * data )
{
	struct sockaddr_in serv_addr;

	int32_t ret = 0;
	int32_t sockDesc = 0;
	std::string length_file = "Unknown";
	uint32_t length_count = 0;

	HeaderInfo header;
	std::string urlData = "";
	std::string requestHeader = "";

	std::stringstream message;
	WorkerThread * thread = (WorkerThread *)data;
	DownloadRequest * request = (DownloadRequest *)thread->data;

	if ( Lux_Util_SocketInit(sockDesc) )
	{
		requestHeader = Lux_Util_FillAddress( request->url, 80, serv_addr );
		Lux_Util_HTTPConnect( requestHeader, sockDesc, serv_addr );
		if ( !Lux_Util_ReadHTTPHeader( sockDesc, header, urlData ) )
		{
			if ( header.status == 302 )
			{
				std::string new_location = header.response["Location"].substr(7);
				lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Downloaded moved to " << new_location << std::endl;
				requestHeader = Lux_Util_FillAddress( new_location, 80, serv_addr );

				urlData.clear();
				header.response.clear();
				header.text.clear();

				Lux_Util_SocketClose(sockDesc);
				Lux_Util_SocketInit(sockDesc);
				Lux_Util_HTTPConnect( requestHeader, sockDesc, serv_addr );
				Lux_Util_ReadHTTPHeader( sockDesc, header, urlData );
			}
		}
		if ( header.status == 200 )
		{
			while ( Lux_Util_ReadHTTPData( sockDesc, urlData, length_count ) )
			{
				message << "Downloading\n" << request->url << "\nDownloaded " << length_count << " of " << length_file << " bytes" << std::endl;
				request->dialog->SetText(message.str(), request->url.length());
				message.str("");
			}

			if ( urlData.length() )
			{
				request->dialog->SetText("Save File to " + request->file);
				elix::File * file = new elix::File(  request->file, true );
				file->WriteString( urlData );
				delete file;
				ret = 1;
			}
			else
			{
				request->dialog->SetText("Error Downloading File");
			}
		}
	}

	Lux_Util_SocketClose(sockDesc);

	thread->_active = false;
	thread->results = ret;
	return ret;
}

int32_t Lux_Util_FileDownloaderEntityCallback( void * data )
{
	struct sockaddr_in serv_addr;

	int32_t ret = 0;
	int32_t sockDesc = 0;
	uint32_t length_count = 0;
	std::string length_file = "Unknown";

	HeaderInfo header;
	std::string urlData = "";
	std::string requestHeader = "";

	WorkerThread * thread = (WorkerThread *)data;
	DownloadEntityRequest * request = (DownloadEntityRequest *)thread->data;

	if ( !request->entity )
	{
		return 0;
	}

	if ( Lux_Util_SocketInit(sockDesc) )
	{
		requestHeader = Lux_Util_FillAddress( request->url, 80, serv_addr );
		Lux_Util_HTTPConnect( requestHeader, sockDesc, serv_addr );
		Lux_Util_ReadHTTPHeader( sockDesc, header, urlData );
		while ( Lux_Util_ReadHTTPData( sockDesc, urlData, length_count ) )
		{

		}
		/*
	#if defined (__GAMECUBE__) || defined (__WII__)
		net_connect(sockDesc, (struct sockaddr*)&serv_addr, sizeof serv_addr);
		net_send(sockDesc, requestHeader.c_str(), requestHeader.length(), 0);
		while ( ( length = net_recv(sockDesc, (char*)rev, RCVBUFSIZE, 0)) > 0 )
	#else
		connect(sockDesc, (struct sockaddr*)&serv_addr, sizeof serv_addr);
		send(sockDesc, requestHeader.c_str(), requestHeader.length(), 0);
		while ( ( length = recv(sockDesc, (char*)rev, RCVBUFSIZE, 0)) > 0 )
	#endif
		{
			urlData.append((char*)rev, length);
			if ( !urlData.compare("HTTP/1.0 418 I\'m a teapot\r\n\r\n") )
			{
				urlData.clear();
				ret = 0;
			}
			else
			{
				memset(rev, 0, RCVBUFSIZE);
				if ( !readHeader )
				{
					cut_at = urlData.find(split);
					if ( cut_at != urlData.npos )
					{
						urlHeader = urlData.substr( 0, cut_at );
						urlData.erase( 0, cut_at+4 );
						readHeader = true;
					}
				}
			}
		}
		*/
	}
/*length := 0
read chunk-size, chunk-extension (if any) and CRLF
while (chunk-size > 0) {
   read chunk-data and CRLF
   append chunk-data to entity-body
   length := length + chunk-size
   read chunk-size and CRLF
}
read entity-header
while (entity-header not empty) {
   append entity-header to existing header fields
   read entity-header
}
Content-Length := length
Remove "chunked" from Transfer-Encoding
*/
	Lux_Util_SocketClose(sockDesc);

	thread->_active = false;
	thread->results = ret;
	if ( urlData.length() )
	{
		ret = 1;
		if ( request->entity )
		{
			thread->Lock();
			request->entity->Call( request->callback, "sd", urlData.c_str(), urlData.length() );
			thread->Unlock();
		}
	}
	return ret;
}

int32_t Lux_Util_FileDownloaderBackground( std::string urlArg, std::string origFile, UserInterface * ui )
{
	int32_t res = 0;
	LuxRect region = {10, ui->ui_region.h, ui->ui_region.w, 50, 0 };
	region.y = (ui->ui_region.h/2)-25;
	region.w -= 20;


	DownloadRequest request;
	request.dialog = ui->AddChild(region, THROBBER, "Downloading " + urlArg + "\n");
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





