/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <SDL.h>
#include <SDL_endian.h>
#include <SDL_thread.h>


#include "core.h"
#include "game_config.h"
#include "engine.h"
#include "display/display.h"
#include "entity_manager.h"
#include "game_system.h"
#include "elix/elix_endian.hpp"
#include <sstream>

#include "network_types.h"

/* Network Thread */
SDL_Thread * sdlcore_thread = NULL;
bool net_active = false;
ENetPeer * peer = NULL;
ENetHost * client;
ENetAddress address;
ENetEvent event;
ENetPacket * message_packet;
uint8_t net_id = 0;
std::map<uint32_t, Player*> net_players;
SDL_mutex * mutex = NULL;

extern std::string server_messages_text[13];

template <typename T> void ReadPacket( ENetPacket * packet, int pos, T & v)
{
	if ( pos + sizeof(T) <= packet->dataLength )
	{
		memcpy(&v, packet->data+pos, sizeof(T));
		if ( sizeof(T) == 4 )
			v = elix::endian::host32(v);
		else if ( sizeof(T) == 2 )
			v = elix::endian::host16(v);
	}
}

Player * NetworkGetPlayer( uint32_t network_id )
{
	std::map<uint32_t, Player *>::iterator p;
	p = net_players.find(network_id);
	if ( p != net_players.end() )
	{
		return p->second;
	}
	return NULL;
}

int pc_network_thread( void *data )
{
	uint8_t message_type = 0;
	uint8_t pid = 0;
	fixed x = 0;
	fixed y = 0;
	uint32_t mapid = 0;
	std::string client_entity = lux::config->GetString("client.entity");

	mutex = SDL_CreateMutex();

	while ( net_active )
	{
		if ( enet_host_service(client, &event, 1000) > 0 )
		{
			if ( event.type == ENET_EVENT_TYPE_RECEIVE )
			{
				if ( event.packet->dataLength >= 2 )
				{
					message_type = (uint8_t)event.packet->data[0];
					pid = (uint8_t)event.packet->data[1];
					if ( message_type == LENS_PLAYERREMOVE ) /* Logged in, Set ID */
					{
						std::map<uint32_t, Player *>::iterator p;
						p = net_players.find(net_id);
						if ( p != net_players.end() )
						{
							delete p->second;
							net_players.erase(p);
						}
					}
					else if ( message_type == LENS_PLAYERADD ) /*  New User logged in, add local player  */
					{
						if (net_id != pid)
						{
							Player * new_player =  new Player(lux::engine->_players.size() + 1, REMOTE);
							if ( event.packet->dataLength > 2 )
							{
								char * username = new char[event.packet->dataLength-1];
								memcpy(username, event.packet->data+2, event.packet->dataLength-2);
								username[event.packet->dataLength-2] = 0;
								Lux_Util_PushMessage( "Player (%s) Joined", username );
								new_player->SetName(username);
								delete username;
							}
							else
								Lux_Util_PushMessage( "Player Joined" );
							if ( client_entity.length() )
							{
								Entity * player_entity = lux::entities->NewEntity("*", client_entity, 0);
								if ( !player_entity )
								{
									std::cout << "Error Creating Network Client Entity " << client_entity << std::endl;
								}
								else
								{
									player_entity->AddSetting("client-name", new_player->GetName() );
									new_player->SetEntity(player_entity);
								}
							}
							lux::engine->_players.push_back(new_player);
							net_players.insert(std::pair<uint8_t, Player*>(pid, new_player));
						}
					}
					else if ( message_type == LENS_PLAYERSYNC )
					{
						if ( event.packet->dataLength >= 14)
						{
							if ( net_id != pid )
							{
								ReadPacket<fixed>( event.packet, 2, x );
								ReadPacket<fixed>( event.packet, 6, y );
								ReadPacket<uint32_t>( event.packet, 10, mapid );
								Player * p = NetworkGetPlayer( pid );
								if ( p )
								{
									p->SetEntityPostion( x, y, 0, mapid );
								}
							}
						}
					}
					else if ( message_type == LENS_ENTITYMSG )
					{
						if ( net_id != pid )
						{
							uint32_t entity_id = 0;
							ReadPacket<uint32_t>( event.packet, 2, entity_id );
							Entity * entity = NULL;
							if ( entity_id )
							{
								entity = lux::entities->GetEntity( entity_id );
							}
							if ( entity )
							{
								uint32_t data_length = (event.packet->dataLength - 6)/4;
								if ( data_length )
								{
									int32_t * data = new int32_t[data_length];
									int32_t player = pid;
									for ( uint32_t n = 0; n < data_length; n++ )
									{
										ReadPacket<int32_t>( event.packet, (int)(6 + (n*4)), data[n] );
									}
									Lux_Util_PushMessage( "msg: %d", data[0] );
									lux::core->NetworkLock();
									entity->callbacks->Push( entity->_data, data_length );
									entity->callbacks->PushArray( entity->_data, data, data_length, NULL );
									entity->callbacks->Push( entity->_data, player );
									entity->Call("NetMessage", (char*)"");
									lux::core->NetworkUnlock();
									delete[] data;
								}
							}
						}
					}
					else if ( message_type == LENS_PLAYERMSG )
					{
						if ( net_id != pid )
						{
							Player * p = NetworkGetPlayer( pid );
							if ( p && event.packet->dataLength >= 6 )
							{
								Entity * entity = p->GetEntity();
								if ( entity )
								{
									uint32_t data_length = (event.packet->dataLength - 2)/4;
									if ( data_length )
									{
										int32_t * data = new int32_t[data_length];
										for ( uint32_t n = 0; n < data_length; n++ )
										{
											ReadPacket<int32_t>( event.packet, (int)(2 + (n*4)), data[n] );
										}
										lux::core->NetworkLock();
										entity->callbacks->Push( entity->_data, data_length );
										entity->callbacks->PushArray( entity->_data, data, data_length, NULL );
										entity->callbacks->Push( entity->_data, pid );
										entity->Call("NetMessage", (char*)"");
										lux::core->NetworkUnlock();
										delete[] data;
									}
								}
							}
						}
					}
					else if ( message_type == LENS_MAPSYNCRETURN )
					{
						if ( net_id == pid )
						{
							uint32_t map_id = 0;
							ReadPacket<uint32_t>( event.packet, 2, map_id );

							MokoiMap * map = lux::gameworld->GetMap(map_id);
							if ( map )
							{
								lux::core->NetworkLock();

								uint32_t data_length = 0;
								ReadPacket<uint32_t>( event.packet, 6, data_length );

								uint32_t enituty10;
								fixed x10, y10;
								Entity * entity = NULL;

								if ( data_length )
								{
									for ( uint32_t n = 0 ; n < data_length; n++ )
									{
										ReadPacket<uint32_t>( event.packet, 10 + (n*12), enituty10 );
										entity = map->GetEntities()->GetEntity( enituty10 );
										if ( entity )
										{
											ReadPacket<fixed>( event.packet, 14 + (n*12), x10 );
											ReadPacket<fixed>( event.packet, 18 + (n*12), y10 );
											entity->x = x10;
											entity->y = y10;
											entity->Update();
										}
										entity = NULL;
									}
								}
								lux::engine->SetState(RUNNING);
								map->server = true;
								lux::core->NetworkUnlock();
							}
						}
					}
				}
			}
			enet_packet_destroy( event.packet );
			event.packet->data = NULL;
			event.packet->dataLength = 0;
		}
	}
	if ( peer )
	{
		enet_peer_disconnect(peer, 0);
	}
	if ( client )
	{
		enet_host_destroy(client);
	}
	enet_deinitialize();
	SDL_DestroyMutex(mutex);
	mutex = NULL;
	return 0;
}

bool NetworkClient::Open()
{
	net_active = false;

	std::string server_ip = lux::config->GetString("server.ip");

	lux::engine->ShowDialog("Connected to '" + server_ip + "'?", DIALOGOK, NULL );

	enet_initialize();
	client = enet_host_create(NULL, 1, 0, 0);

	if ( client == NULL )
	{
		std::cout << __FILE__ << ":" << __LINE__ << " | An error occurred while trying to create an ENet client host." << std::endl;
		enet_deinitialize();
		return net_active;
	}

	/* Connect to some.server.net:8714. */
	std::cout << __FILE__ << ":" << __LINE__ << " | Connecting to " << server_ip << std::endl;
	enet_address_set_host( &address, server_ip.c_str() );
	address.port = 10514;

	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect( client, &address, 2 );

	if ( peer == NULL )
	{
		std::cout << __FILE__ << ":" << __LINE__ << " | An error occurred while trying to create an ENet client host." << std::endl;
		return net_active;
	}

	if ( enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT )
	{
		if ( enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE )
		{
			if ( event.packet->dataLength > 1 )
			{
				if ( event.packet->data[0] == LENS_SETID ) /* Logged in, Set ID */
				{
					net_id = event.packet->data[1];
				}
			}
			Lux_Util_PushMessage( "Message %d for %d.", event.packet->data[0], net_id);
			std::cout << __FILE__ << ":" << __LINE__ << " | Connection to " << server_ip << " succeeded." << std::endl;
			net_active = true;
			sdlcore_thread = SDL_CreateThread( pc_network_thread, "LuxNet", NULL );
		}
	}
	else
	{
		enet_peer_reset( peer );
		std::cout << __FILE__ << ":" << __LINE__ << " | Connection to " << server_ip << " failed." << std::endl;
	}
	return net_active;
}

bool NetworkClient::CreateMessage(uint8_t type, bool reliable)
{
	if ( !net_active )
		return false;

	message_packet = enet_packet_create(NULL, 2, (reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED));
	if ( message_packet )
	{
		message_packet->data[0] = type;
		message_packet->data[1] = net_id;
	}
	return (message_packet ? true : false);
}

bool NetworkClient::MessageAppend(fixed data)
{
	uint8_t dataLength = message_packet->dataLength;
	uint32_t p = elix::endian::big32(data);
	if ( !enet_packet_resize(message_packet, message_packet->dataLength + sizeof(uint32_t)) )
	{
		memcpy(message_packet->data + dataLength, &p, 4);
		return true;
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "MessageAppend Error" << std::endl;
	return false;
}

bool NetworkClient::MessageAppend(uint8_t data)
{
	uint8_t dataLength = message_packet->dataLength;
	if ( !enet_packet_resize(message_packet, message_packet->dataLength + sizeof(uint8_t)) )
	{
		memcpy(message_packet->data + dataLength, &data, 1);
		return true;
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "MessageAppend Error" << std::endl;
	return false;
}

bool NetworkClient::MessageAppend(uint32_t data)
{
	uint8_t dataLength = message_packet->dataLength;
	uint32_t p = elix::endian::big32(data);
	if ( !enet_packet_resize(message_packet, message_packet->dataLength + 4) )
	{
		memcpy(message_packet->data + dataLength, &p, 4);
		return true;
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "MessageAppend Error" << std::endl;
	return false;
}

bool NetworkClient::MessageSend(bool wait)
{
	if ( !peer )
	{
		Lux_Util_PushMessage( "MessageSend called, but not connected.");
		return false;
	}
	if ( net_id )
	{
		if ( enet_peer_send(peer, 0, message_packet) )
			Lux_Util_PushMessage( "Error Sending %d.", message_packet->data[0] );
		//enet_packet_destroy(message_packet);
		message_packet = NULL;
		return true;
	}
	return false;
}

int32_t NetworkClient::ReadMessage(void * data)
{
	return 0;
}

void NetworkClient::NetworkLock()
{
	if ( mutex )
	{
		SDL_mutexP(mutex);
	}
}

void NetworkClient::NetworkUnlock()
{
	if ( mutex )
	{
		SDL_mutexV(mutex);
	}
}

