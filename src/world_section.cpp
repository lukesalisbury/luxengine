/****************************
Copyright © 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "world_section.h"
#include "elix_string.h"
#include "mokoi_game.h"
#include "entity_section.h"
#include "config.h"
#include "world.h"
#include "display.h"
#include <cstdlib>


WorldSection::WorldSection( std::string file, uint32_t id, uint8_t width, uint8_t height )
{
	this->_id = id;
	this->file = file;

	this->InitialSetup( width, height );
	this->LoadFile();
}


WorldSection::WorldSection( uint8_t width, uint8_t height )
{
	this->InitialSetup( width, height );
}


void WorldSection::InitialSetup( uint8_t width, uint8_t height )
{
	this->flag = 1;
	this->_width = width;
	this->_height = height;
	memset(&this->_grid, 0xFFFF, 8192); /* Reset Grid */
}

void WorldSection::LoadFile( )
{
	uint32_t grid[2];
	std::string buffer;
	std::stringstream world_file;
	std::vector<std::string> world_array;
	if ( lux::game->GetStream("./sections/" + this->file + ".txt", &world_file) )
	{
		while ( !world_file.eof() )
		{
			std::getline(world_file, buffer);
			elix::string::Trim(&buffer);
			if ( !buffer.empty() )
			{
				elix::string::Split(buffer, "\t", &world_array);
				if ( world_array.size() == 3 || world_array.size() == 5)
				{
					MokoiMap * new_map = new MokoiMap( world_array[0] );
					if ( world_array.size() == 5 )
					{
						new_map->dimension_width = atoi( world_array[3].c_str() );
						new_map->dimension_height = atoi( world_array[4].c_str() );
					}
					else
						new_map->LoadDimension();

					if ( new_map->dimension_width && new_map->dimension_height )
					{
						grid[0] = atoi( world_array[1].c_str() );
						grid[1] = atoi( world_array[2].c_str() );

						new_map->SetGridIdent( (grid[1] * this->_height) + grid[0], this->_id );

						for ( uint8_t x = 0; x < new_map->dimension_width; x++ )
						{
							for ( uint8_t y = 0; y < new_map->dimension_height; y++ )
							{
								if ( (grid[0] + x) < this->_width && (grid[1] + y) < this->_height )
								{
									this->_grid[grid[0] + x][grid[1] + y] = new_map->GridIdent();
								}
							}
						}
						lux::world->map_list.insert( std::make_pair( new_map->Ident(), new_map ) );
					}
				}
			}
			world_array.clear();
		}
		world_file.clear();
	}

}

WorldSection::~WorldSection()
{
	lux::world->DeleteSection(this->_id);
}


bool WorldSection::Save(elix::File * current_save_file)
{
	current_save_file->WriteWithLabel( "Section ID", this->_id );
	current_save_file->WriteWithLabel( "Section file", this->file );
	current_save_file->WriteWithLabel( "Section flag", this->flag );
	return true;
}

bool WorldSection::Restore(elix::File * current_save_file)
{
	this->_id = current_save_file->Read_uint32WithLabel( "Section ID", true );
	current_save_file->ReadWithLabel( "Section file", &this->file );
	this->flag = current_save_file->Read_uint8WithLabel( "Section flag" );
	this->LoadFile();

	return true;
}

