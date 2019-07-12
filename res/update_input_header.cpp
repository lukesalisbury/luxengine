/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

/*
include/%2/%1.h
res/input/%1.png > %1_image_data
res/input/%1.txt > %1_sheet_data


*/

std::string source_image_path = "";
std::string source_sheet_path = "";
std::string target_header_path = "";
std::string file_name = "";
std::stringstream target_file_content;


void read_image( std::string file )
{
	size_t file_size = 0;
	int c;
	std::ifstream source_file;

	source_file.open( source_image_path.c_str(), std::ios_base::binary );

	target_file_content.flags( std::ios::showbase );

	target_file_content << "uint8_t " << file << "_image_data[] = { \n\t";

	while ( source_file.good() )
	{
		file_size++;
		c = source_file.get();

		target_file_content << std::hex << c;

		if ( source_file.peek() == std::char_traits<char>::eof() )
		{
			break;
		}

		target_file_content << std::dec << ",";
		if ( !(file_size % 20) )
			target_file_content << "\n\t";

	}
	target_file_content << "\n};" << std::endl;
	target_file_content << "int32_t " << file << "_image_size = " << std::dec << file_size << ";" << std::endl;


	source_file.close();

}

void read_sheet( std::string file )
{
	size_t file_size = 0;
	std::string c;
	std::ifstream source_file;

	source_file.open( source_sheet_path.c_str(), std::ios_base::binary );

	target_file_content.flags( std::ios::showbase );

	target_file_content << "std::string " << file << "_sheet_data[] = { \n";

	while ( source_file.good() )
	{
		file_size++;
		std::getline(source_file, c);

		target_file_content << "\t\"" << c << "\"";

		if ( source_file.peek() == std::char_traits<char>::eof() )
		{
			break;
		}

		target_file_content << "," << std::endl;

	}
	target_file_content << "\n};" << std::endl;
	target_file_content << "int32_t " << file << "_sheet_size = " << file_size << ";" << std::endl;


	source_file.close();

}


void write_header()
{
	std::ofstream target_file;
	target_file.open( target_header_path.c_str(), std::ios_base::binary );

	target_file << target_file_content.str() << std::endl;

	target_file.close();
}

void set_paths( std::string file )
{
	source_image_path = "res/input/";
	source_image_path.append(file);
	source_image_path += ".png";

	source_sheet_path = "res/input/";
	source_sheet_path.append(file);
	source_sheet_path += ".txt";

	target_header_path = "include/input/";
	target_header_path.append(file);
	target_header_path += ".h";
}

void GetNameNoExtension( std::string & path )
{
	std::string new_path = path;
	int lastslash = path.find_last_of( '/', path.length() );
	int lastdot = path.find_last_of( '.', path.length() );


	if ( lastdot )
	{
		path.erase( lastdot, path.length() - lastdot );
	}

	if ( lastslash )
	{
		path.erase( 0, lastslash+1 );

	}

}

int main(int argc, char *argv[])
{
	if ( argc > 1 )
	{
		file_name.assign(argv[1]);
		GetNameNoExtension( file_name );

		std::cout << "Converting Input Icon " << argv[1] << " to header file." << std::endl;

		set_paths( file_name );

		std::cout << "Source:" << source_image_path << std::endl;
		std::cout << "Source:" << source_sheet_path << std::endl;
		std::cout << "Target:" << target_header_path << std::endl;

		read_image( file_name );
		read_sheet( file_name );



		write_header();



		return 0;
	}

	return 0;
}
