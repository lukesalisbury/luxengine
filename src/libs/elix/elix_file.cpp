/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include "elix/elix_endian.hpp"
#include "elix/elix_file.hpp"

namespace elix {
	File::File(std::string filename, bool write)
	{
		this->error = "";
		this->path = filename;
		this->_length = 0;
		this->ErrorCallback = nullptr;
		this->writable = false;
		this->handle = nullptr;

		if ( write )
		{
			this->handle = fopen(this->path.c_str(), "wb");
		}
		else
		{
			struct stat buffer;
			if ( stat(this->path.c_str(), &buffer) == 0 )
			{
				if ( S_ISREG(buffer.st_mode) )
				{
					this->handle = fopen(this->path.c_str(), "rb" );
				}
			}

		}

		if ( !this->handle )
		{
			this->error = "Can't open '" + this->path + "'";

		}
		else
		{
			this->writable = write;
			if ( !this->writable )
			{
				fseek( (FILE *)this->handle, 0, SEEK_END );
				this->_length = ftell( (FILE *)this->handle );
				fseek( (FILE *)this->handle, 0, SEEK_SET );
			}
		}
	}

	File::~File()
	{
		if ( this->handle )
		{
			fclose( (FILE*)this->handle );
		}

	}

	bool File::Exist()
	{
		return (this->handle ? true : false);
	}

	uint32_t File::Length()
	{
		int32_t len = 0;
		int32_t pos = ftell( (FILE *)this->handle );
		fseek( (FILE *)this->handle, 0, SEEK_END );
		len = ftell( (FILE *)this->handle );
		fseek( (FILE *)this->handle, pos, SEEK_SET );
		return (uint32_t) len;
	}

	int32_t File::Seek( int32_t pos )
	{
		return fseek( (FILE *)this->handle, (int32_t)pos, SEEK_SET );
	}

	int32_t File::Tell()
	{
		return ftell( (FILE *)this->handle );
	}

	bool File::EndOfFile()
	{
		if ( this->handle )
		{
			if ( feof( (FILE *)this->handle ) == 0 )
			{
				return false;
			}
		}
		return true;
	}

	int32_t File::Scan( int32_t startPostion, uint8_t * needle, uint32_t needleLength )
	{
		int32_t position = -1;
		bool needle_found = false;
		if ( this->handle && needleLength > 0 )
		{

			int32_t c = 0;
			uint8_t check = 0;

			this->Seek( startPostion );
			do {
				c = fgetc( (FILE *)this->handle );
				check = c;

				if ( check == needle[0] )
				{
					uint32_t scan_count = 1;

					position = this->Tell()-1; //want to go back to the first character

					while ( scan_count < needleLength )
					{
						c = fgetc( (FILE *)this->handle );
						check = c;

						if ( check != needle[scan_count] )
						{
							position = -1;
							scan_count = needleLength;
						}
						scan_count++;
					}

					if ( position > 0 )
					{
						 needle_found = true;
					}


				}


			} while ( !EndOfFile() && !needle_found );

		}

		this->Seek( 0 );
		return position;
	}

	bool File::Read( data_pointer buffer, uint32_t size, uint32_t count )
	{
		if ( !Exist() || !buffer )
			return false;

		if ( (count * size) > this->_length )
		{
			std::cerr << __FUNCTION__ << ": Reading only " << this->_length / size << " instead of " << count<< std::endl;
			count = this->_length / size;
		}

		uint32_t read = fread(buffer, size, count, (FILE *)this->handle);

		if ( read == count*size )
		{
			return true;
		}
		else
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			std::cerr << __FUNCTION__ << ": Read only  " << read << " byte(s), instead of " << (size*count) << " byte(s) from " << this->path << std::endl;
		}
		return false;
	}

	uint32_t File::Read2( data_pointer buffer, uint32_t size, uint32_t count )
	{
		if ( !this->handle || this->EndOfFile() || !this->_length )
		{
			return 0;
		}
		if ( buffer )
		{
			uint32_t read = fread(buffer, size, count, (FILE *)this->handle);
			return read;
		}
		return 0;
	}

	uint32_t File::ReadAll( data_pointer * buffer, bool addnull )
	{
		if ( !this->handle || this->EndOfFile() || !this->_length )
		{
			return false;
		}

		if ( addnull )
		{
			*buffer = new char[this->_length+1];
			memset(*buffer, 0,this->_length+1);
		}
		else
		{
			*buffer = new char[this->_length];
			memset(*buffer, 0,this->_length);
		}
		if ( *buffer != nullptr )
		{

			if ( fread(*buffer, this->_length, 1, (FILE *)this->handle) )
			{
				//if ( addnull )
				//	buffer[this->_length] = 0;
				return this->_length;
			}
		}
		return 0;
	}

	uint32_t File::ReadUint32( bool sysvalue )
	{
		uint32_t value = 0;
		uint32_t read = fread(&value, sizeof(uint32_t), 1, (FILE *)this->handle);
		if ( !read )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			std::cerr << __FUNCTION__ << ": Reading only '" << read << "' from '" << this->path << "'. " << std::endl;
			return 0;
		}
		if ( sysvalue )
		{
			return elix::endian::host32( value );
		}
		return value;
	}

	uint16_t File::ReadUint16( bool sysvalue )
	{
		uint16_t value = 0;
		if ( !fread(&value, sizeof(uint16_t), 1, (FILE *)this->handle) )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			std::cerr << __FUNCTION__ << ": Error reading from '" << this->path << "'. " << std::endl;
			return 0;
		}
		if ( sysvalue )
		{
			return elix::endian::host16( value );
		}
		return value;
	}

	uint8_t File::ReadUint8( )
	{
		uint8_t value = 0;

		if ( !this->handle )
		{
			return 0;
		}
		if ( !fread( &value, sizeof(uint8_t), 1, (FILE *)this->handle ) )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			if ( !this->EndOfFile() )
				std::cerr << __FUNCTION__ << ": Error reading from '" << this->path << "'. " << std::endl;
			return 0;
		}
		if ( this->EndOfFile() )
			return 0;
		else
			return value;
	}

	std::string File::ReadString()
	{
		std::string str;
		this->Read( &str );
		return str;
	}

	bool File::Read( std::string * str, uint32_t length )
	{
		str->clear();

		uint8_t value = 255;
		uint32_t character_count = 0;
		while ( value )
		{
			value = this->ReadUint8( );
			character_count++;
			if ( value && !this->EndOfFile() )
			{
				str->append( 1, value );
			}
			if ( length == character_count )
			{
				return true;
			}
		}
		return true;
	}

	bool File::ReadLine( std::string * str )
	{
		if (!this->handle || this->EndOfFile() )
		{
			return false;
		}
		uint8_t value = 0;
		uint32_t pos = this->Tell();

		while ( this->_length > pos )
		{
			fread( &value, sizeof(uint8_t), 1, (FILE *)this->handle );
			if ( this->EndOfFile() || value == 0 )
				return false;
			else if ( value == 10 )
				return true;
			else
				str->append( 1, value );
			pos++;
		}
		return (bool)str->length();
	}

	bool File::ReadStruct(data_pointer buffer, uint8_t size)
	{
		if ( !this->handle || this->EndOfFile() || !this->_length || !buffer )
		{
			return 0;
		}

		uint8_t data_size = 0;

		if ( fread( &data_size, sizeof(uint8_t), 1, (FILE *)this->handle ) )
		{
			if ( data_size == size )
			{
				fread(buffer, size, 1, (FILE *)this->handle);
				return true;
			}
			else
			{
				if ( this->ErrorCallback )
					this->ErrorCallback( "ReadStruct data_size != size" );
			}
		}

		return false;
	}

	bool File::Write( data_pointer buffer, uint32_t size, uint32_t count )
	{
		if ( !Exist() )
			return false;
		return (fwrite( buffer, size, count, (FILE *)this->handle ) ? true : false);
	}

	bool File::Write( uint32_t value, bool sysvalue )
	{
		if ( !Exist() )
			return false;
		if ( !sysvalue )
		{
			uint32_t uvalue = elix::endian::big32( value );
			return ( fwrite( &uvalue, sizeof(uint32_t), 1, (FILE *)this->handle ) ? true : false );
		}
		else
		{
			return ( fwrite( &value, sizeof(uint32_t), 1, (FILE *)this->handle ) ? true : false );
		}
	}

	bool File::Write( uint16_t value, bool sysvalue )
	{
		if ( !Exist() )
			return false;
		if ( !sysvalue )
		{
			uint16_t uvalue = elix::endian::big16( value );
			return ( fwrite( &uvalue, sizeof(uint16_t), 1, (FILE *)this->handle ) ? true : false );
		}
		else
		{
			return ( fwrite( &value, sizeof(uint16_t), 1, (FILE *)this->handle ) ? true : false );
		}
	}

	bool File::Write( uint8_t value )
	{
		if ( Exist() )
			return ( fwrite( &value, sizeof(uint8_t), 1, (FILE *)this->handle ) ? true : false );
		return false;
	}

	bool File::Write( std::string value )
	{
		if ( !Exist() )
			return false;
		bool result = false;
		if ( value.length() )
		{
			result = ( fwrite( value.c_str(), sizeof(uint8_t), value.length(), (FILE *)this->handle ) ? true : false );
		}
		result = this->Write( (uint8_t)'\0' );
		return result;
	}

	bool File::WriteString( std::string value )
	{
		if ( !Exist() )
			return false;
		bool result = false;
		if ( this->handle )
		{
			result = ( fwrite( value.c_str(), sizeof(uint8_t), value.length(), (FILE *)this->handle ) ? true : false );
		}
		return result;
	}

	bool File::WriteStruct(data_pointer buffer, uint8_t size)
	{
		if ( !Exist() )
			return false;
		fwrite( &size, sizeof(uint8_t), 1, (FILE *)this->handle );
		return (fwrite( buffer, size, 1, (FILE *)this->handle ) ? true : false);
	}



	bool File::WriteWithLabel( std::string label, data_pointer buffer, uint32_t size, uint32_t count )
	{
	#ifdef DEBUG
		if ( !this->write_log_handle.is_open() )
			this->write_log_handle.open("write.log",  std::fstream::out );
		this->write_log_handle << "<" << label << "> " << "Data Size " << size*count << std::endl;
	#endif
		if ( this->Write( buffer, size, count ) )
		{
			return true;
		}
		return false;
	}

	bool File::WriteWithLabel( std::string label, uint32_t value, bool sysvalue)
	{
	#ifdef DEBUG
		if ( !this->write_log_handle.is_open() )
			this->write_log_handle.open("write.log",  std::fstream::out );
		this->write_log_handle << "<" << label << "> " << value << std::endl;
	#endif

		if ( this->Write( value, sysvalue ) )
		{
			return true;
		}
		return false;
	}
	bool File::WriteWithLabel( std::string label, uint16_t value, bool sysvalue)
	{
	#ifdef DEBUG
		if ( !this->write_log_handle.is_open() )
			this->write_log_handle.open("write.log",  std::fstream::out );
		this->write_log_handle << "<" << label << "> " << value << std::endl;
	#endif

		if ( this->Write( value, sysvalue ) )
		{
			return true;
		}
		return false;
	}
	bool File::WriteWithLabel( std::string label, uint8_t value )
	{
	#ifdef DEBUG
		if ( !this->write_log_handle.is_open() )
			this->write_log_handle.open("write.log",  std::fstream::out );
		this->write_log_handle << "<" << label << "> " << +value << std::endl;
	#endif
		if ( this->Write( value ) )
		{
			return true;
		}
		return false;
	}
	bool File::WriteWithLabel( std::string label, std::string value )
	{
	#ifdef DEBUG
		if ( !this->write_log_handle.is_open() )
			this->write_log_handle.open("write.log",  std::fstream::out );
		this->write_log_handle << "<" << label << "> '" << value << "'" << std::endl;
	#endif
		if ( this->Write( value ) )
		{
			return true;
		}
		return false;
	}

	bool File::WriteStructWithLabel(std::string label, data_pointer buffer, uint8_t size)
	{
		bool result = WriteStruct( buffer, size );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("write.log",  std::fstream::out );
		this->read_log_handle  << "<" << label << "> " << "Data Size " << ( size ) << std::endl;
	#endif
		return result;
	}

	bool File::ReadWithLabel( std::string label, data_pointer buffer, uint32_t size, uint32_t count )
	{
		bool result = Read( buffer, size, count );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle  << "<" << label << "> " << "Data Size " << ( size * count ) << std::endl;
	#endif
		return result;
	}

	uint32_t File::Read2WithLabel( std::string label, data_pointer buffer, uint32_t size, uint32_t count )
	{
		uint32_t result = Read2( buffer, size, count );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle  << "<" << label << "> " << "Data Size " << ( size * count ) << std::endl;
	#endif
		return result;
	}

	uint32_t File::ReadUint32WithLabel( std::string label, bool sysvalue )
	{
		uint32_t result = ReadUint32( sysvalue );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle << "<" << label << "> " << result << std::endl;
	#endif
		return result;
	}
	uint16_t File::ReadUint16WithLabel( std::string label, bool sysvalue )
	{
		uint16_t result = ReadUint16( sysvalue );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle << "<" << label << "> " << result << std::endl;
	#endif
		return result;
	}
	uint8_t File::ReadUint8WithLabel( std::string label )
	{
		uint8_t result = ReadUint8(  );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle << "<" << label << "> " << +result << std::endl;
	#endif
		return result;
	}
	std::string File::ReadStringWithLabel( std::string label )
	{
		std::string result = ReadString(  );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle << "<" << label << "> '" << result << "'" << std::endl;
	#endif
		return result;
	}


	bool File::ReadWithLabel( std::string label, std::string * str, uint32_t length )
	{
		bool result = Read( str, length );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle << "<" << label << "> '" << *str << "'" << std::endl;
	#endif
		return result;
	}

	bool File::ReadStructWithLabel(std::string label, data_pointer buffer, uint8_t size)
	{
		bool result = ReadStruct( buffer, size );
	#ifdef DEBUG
		if ( !this->read_log_handle.is_open() )
			this->read_log_handle.open("read.log",  std::fstream::out );
		this->read_log_handle << "<" << label << "> '" << size << "'" << std::endl;
	#endif
		return result;
	}

}




