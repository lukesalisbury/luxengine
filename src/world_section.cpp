/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "world_section.h"
#include "elix_string.hpp"
#include "mokoi_game.h"
#include "entity_section.h"
#include "config.h"
#include "world.h"
#include "display.h"
#include <cstdlib>


#define GRID_LOCATION(x, y, gw, gh) ( x + (y*gw) )

WorldSection::WorldSection( LuxMapIdent ident, std::string file, uint8_t width, uint8_t height )
{
	this->id = ident.value;
	this->name = file;

	this->InitialSetup( width, height );


	this->LoadFile();
}


WorldSection::WorldSection( elix::File * current_save_file )
{
	this->Restore( current_save_file );
}


WorldSection::~WorldSection()
{
	lux::gameworld->DeleteSection(this->id);

	delete [] this->grid;
}




void WorldSection::InitialSetup( uint8_t width, uint8_t height )
{
	this->allocated_size = width * height;
	this->flag = 1;
	this->width = width;
	this->height = height;

	this->grid = new uint16_t[this->allocated_size];

	memset(&this->grid, 0xFFFF, this->allocated_size * 2 ); /* Reset Grid */
}
/**/
bool WorldSection::AddMap( std::string file_name, const uint8_t x, const uint8_t y )
{
	MokoiMap * map_object = new MokoiMap( file_name );

	if ( this->AddMap( map_object, x, y ) == false )
	{
		delete map_object;
		return false;
	}
	else
	{
		return true;
	}

}

/**/

bool WorldSection::AddMap( MokoiMap * map_object, const uint8_t x, const uint8_t y )
{
	uint16_t location = 0xFFFF;

	map_object->LoadDimension();
	if ( map_object->dimension_width && map_object->dimension_height )
	{
		location = GRID_LOCATION( x, y, this->width, this->height );

		map_object->SetGridIdent( location, this->id );

		for ( uint8_t grid_x = 0; grid_x < map_object->dimension_width; grid_x++ )
		{
			for ( uint8_t grid_y = 0; grid_y < map_object->dimension_height; grid_y++ )
			{
				location = GRID_LOCATION( (grid_x + x), (grid_y + y), this->width, this->height );
				if ( location < this->allocated_size )
				{
					this->grid[location] = map_object->GridIdent();
				}
			}
		}
		lux::gameworld->map_list.insert( std::make_pair( map_object->Ident(), map_object ) );

		return true;
	}
	return false;
}



void WorldSection::LoadFile( )
{
	uint32_t grid[2];
	std::string buffer;
	std::stringstream world_file;
	std::vector<std::string> world_array;

	if ( lux::game_data->GetStream("./sections/" + this->name + ".txt", &world_file) )
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
					grid[0] = elix::string::ToIntU16( world_array[1] );
					grid[1] = elix::string::ToIntU16( world_array[2] );

					this->AddMap( world_array[0], grid[0], grid[1] );
				}
			}
			world_array.clear();
		}
		world_file.clear();
	}

}

void WorldSection::SaveFile( )
{

}

bool WorldSection::Save( elix::File * current_save_file )
{
	current_save_file->WriteWithLabel( "Section Size", this->allocated_size );
	current_save_file->WriteWithLabel( "Section ID", this->id );
	current_save_file->WriteWithLabel( "Section file", this->name );
	current_save_file->WriteWithLabel( "Section flag", this->flag );
	return true;
}

bool WorldSection::Restore( elix::File * current_save_file )
{
	this->allocated_size = current_save_file->ReadUint16WithLabel( "Section Size", true );
	this->id = current_save_file->ReadUint32WithLabel( "Section ID", true );
	this->name = current_save_file->ReadStringWithLabel( "Section file" );
	this->flag = current_save_file->ReadUint8WithLabel( "Section flag" );


	this->LoadFile();

	return true;
}

uint16_t WorldSection::GetGridID( uint8_t grid_x, uint8_t grid_y )
{
	if ( grid_x >= this->width )
		return 0xFFFF;
	if ( grid_y >= this->height )
		return 0xFFFF;

	uint16_t location = GRID_LOCATION( grid_x, grid_y, this->width, this->height );

	return this->grid[location];
}

uint32_t WorldSection::GetMapID( const uint8_t grid_x, const uint8_t grid_y  )
{
	if ( grid_x >= this->width )
		return 0x0;
	if ( grid_y >= this->height )
		return 0x0;

	uint16_t location = GRID_LOCATION( grid_x, grid_y, this->width, this->height );

	return this->GetMapID(location);
}

uint32_t WorldSection::GetMapID( const uint16_t grid_id )
{
	LuxMapIdent map;
	map.value = 0;
	map.grid.section = this->id;
	map.grid.map = grid_id;
	return map.value;
}


MokoiMap * WorldSection::GetMap( const uint16_t grid_id  )
{
	MokoiMap * map = NULL;
	uint32_t ident = this->GetMapID( grid_id );

	if ( ident )
	{
		map = lux::gameworld->GetMap( ident );
	}

	return map;
}

MokoiMap * WorldSection::GetMap( const uint8_t grid_x, const uint8_t grid_y )
{
	MokoiMap * map = NULL;
	uint32_t ident = this->GetMapID( grid_x, grid_y );

	if ( ident )
	{
		map = lux::gameworld->GetMap( ident );
	}

	return map;
}



