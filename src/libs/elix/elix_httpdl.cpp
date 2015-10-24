/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "elix_private.hpp"
#include <string>
#include <map>
#include <iostream>
#include <iostream>
#include "elix/elix_string.hpp"

#ifdef __GNUWIN32__
	#include <winsock.h>
#elif __NDS__
	#include <nds.h>
	#include <dswifi9.h>
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

namespace elix {
	namespace httpdl {
		const std::string http_split = "\r\n\r\n";

		/*****************************
		* Fill in sockaddr_in 'addr' variable
		*/
		std::string FillAddress(std::string address, uint16_t port, uint16_t http_version, sockaddr_in &addr)
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
				uint16_t port_t = elix::string::ToIntU16( address.substr(cut_at+1) );
				if ( port_t > 80 )
					port = port_t;
				address = address.erase(cut_at);
			}
			header = "GET " + path + " HTTP/1.0\r\nAccept: */*\r\nHost: " + address + "\r\nUser-Agent:Elix-httpdl "ELIX_VERSION_STRING"\r\n\r\n";
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

			return header;
		}

		/*****************************
		* ParseHeader
		*
		* Returns: status code
		*/
		uint16_t ParseHeader( std::string header, std::map<std::string, std::string> & header_value )
		{
			uint16_t status_code;
			std::vector<std::string> lines;
			std::string::size_type slipt_point;

			elix::string::Split( header, "\r\n", &lines );
			if ( lines.size() )
			{
				for( std::string::size_type i = 0; i < lines.size(); i++ )
				{
					if ( i == 0 )
					{
						status_code = elix::string::ToIntU16( lines[i].substr(9, 3) );
						std::cout << "Header: '" << lines[i].substr(5,3) << "' '" << lines[i].substr(9, 3) << "' '" << lines[i].substr(13) << "'" << std::endl;
					}
					else
					{
						slipt_point = lines[i].find_first_of(':', 0);
						if ( std::string::npos != slipt_point )
						{
							//header.response[lines[i].substr(0, slipt_point)] = lines[i].substr(slipt_point+2);
							std::cout << "Header: '" << lines[i].substr(0, slipt_point) << "' '" << lines[i].substr(slipt_point+2) << "'" << std::endl;
						}
					}
				}
			}
			return status_code;
		}


		/*****************************
		* ScanHeader
		* Download the data until the header is finished
		* Returns: Success if return header has a 200 status
		*/
		uint32_t ScanHeader(int32_t sockDesc, std::map<std::string, std::string> & header_value, std::string & urlData )
		{
			int32_t length = -1;
			std::string::size_type cut_at = 0;
			std::string header = "";
			uint8_t rev[RCVBUFSIZE];

			#if defined (__GAMECUBE__) || defined (__WII__)
			while ( ( length = net_recv(sockDesc, (char*)rev, RCVBUFSIZE, 0)) > 0 )
			#else
			while ( ( length = recv(sockDesc, (char*)rev, RCVBUFSIZE, 0)) > 0 )
			#endif
			{
				urlData.append((char*)rev, length);
				memset(rev, 0, RCVBUFSIZE);

				cut_at = urlData.find(http_split);
				if ( cut_at != urlData.npos )
				{
					header = urlData.substr( 0, cut_at );
					urlData.erase( 0, cut_at+4 );

					return (ParseHeader( header, header_value ) == 200);
				}
			}

			return false;
		}


		/*****************************
		* Lux_Util_ReadHTTPData
		*
		*/
		bool ReadHTTP(int32_t sockDesc, std::string & urlData, uint32_t & count )
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


		/*****************************
		* OpenConnection
		*
		*/
		void OpenConnection(std::string requestHeader, int32_t sockDesc, sockaddr_in &serv_addr )
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
		* SocketInit
		*
		*/

		int32_t SocketInit(int &sockDesc)
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

		/*****************************
		* SocketClose
		*
		*/

		void SocketClose(int &sockDesc)
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
	}
}
