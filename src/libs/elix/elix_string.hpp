#ifndef _ELIX_STRING_H_
#define _ELIX_STRING_H_

#include "elix/elix_intdef.h"
#include <string>
#include <vector>

namespace elix {
	namespace string {
		void StripUnwantedCharacters( std::string & str );

		void Replace( std::string & str, std::string find, std::string replace );
		void Split( std::string str, std::string delim, std::vector<std::string> * results );
		void Trim( std::string * source );
		void Truncate(std::string & source , size_t requested_length);
		void TruncateLines( std::string & source, size_t requested_length );

		bool HasPrefix( const std::string source, const std::string prefix );
		bool HasSuffix( const std::string source, const std::string suffix );

		uint32_t Hash( std::string str );

		std::string FromInt(int32_t value);
		std::string FromInt64(int64_t value);
		std::string HexFromInt64(int64_t value);
		std::string HexFromInt32(int32_t value);

		int32_t ToInt32(std::string string);
		uint16_t ToIntU16(std::string string);
		uint8_t ToIntU8(std::string string);
	}
}

#endif

