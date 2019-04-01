/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Required Headers */
#include "core.h"
#include "config.h"
#include "engine.h"
#include "display/display.h"
#include "misc_functions.h"
#include "mokoi_game.h"
#include "map_object.h"
#include "platform_functions.h"
#include "graphics_native.h"

int decodePNG(std::vector<uint8_t>& out_image_32bit, uint32_t& image_width, uint32_t& image_height, const uint8_t * in_png, uint32_t in_size)
{
	// picoPNG version 20080503
	// Copyright (c) 2005-2008 Lode Vandevenne
	//
	// This software is provided 'as-is', without any express or implied
	// warranty. In no event will the authors be held liable for any damages
	// arising from the use of this software.
	//
	// Permission is granted to anyone to use this software for any purpose,
	// including commercial applications, and to alter it and redistribute it
	// freely, subject to the following restrictions:
	//
	//		 1. The origin of this software must not be misrepresented; you must not
	//		 claim that you wrote the original software. If you use this software
	//		 in a product, an acknowledgment in the product documentation would be
	//		 appreciated but is not required.
	//		 2. Altered source versions must be plainly marked as such, and must not be
	//		 misrepresented as being the original software.
	//		 3. This notice may not be removed or altered from any source distribution.

	// picoPNG is a PNG decoder in one C++ function. Use picoPNG for programs that need
	// only 1 .cpp file. Apologies for the compact code style, it's to make it tiny.

	static const uint32_t LENBASE[29] =	{3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
	static const uint32_t LENEXTRA[29] = {0,0,0,0,0,0,0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,	4,	5,	5,	5,	5,	0};
	static const uint32_t DISTBASE[30] =	{1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
	static const uint32_t DISTEXTRA[30] = {0,0,0,0,1,1,2, 2, 3, 3, 4, 4, 5, 5,	6,	6,	7,	7,	8,	8,	 9,	 9,	10,	10,	11,	11,	12,	 12,	 13,	 13};
	static const uint32_t CLCL[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15}; //code length code lengths
	struct Zlib //nested functions for zlib decompression
	{
		static uint32_t readBitFromStream(size_t& bitp, const uint8_t* bits) { uint32_t result = (bits[bitp >> 3] >> (bitp & 0x7)) & 1; bitp++; return result;}
		static uint32_t readBitsFromStream(size_t& bitp, const uint8_t* bits, size_t nbits)
		{
			uint32_t result = 0;
			for(size_t i = 0; i < nbits; i++) result += (readBitFromStream(bitp, bits)) << i;
			return result;
		}
		struct HuffmanTree
		{
			int makeFromLengths(const std::vector<uint32_t>& bitlen, uint32_t maxbitlen)
			{ //make tree given the lengths
				uint32_t numcodes = (uint32_t)(bitlen.size()), treepos = 0, nodefilled = 0;
				std::vector<uint32_t> tree1d(numcodes), blcount(maxbitlen + 1, 0), nextcode(maxbitlen + 1, 0);
				for(uint32_t bits = 0; bits < numcodes; bits++) blcount[bitlen[bits]]++; //count number of instances of each code length
				for(uint32_t bits = 1; bits <= maxbitlen; bits++) nextcode[bits] = (nextcode[bits - 1] + blcount[bits - 1]) << 1;
				for(uint32_t n = 0; n < numcodes; n++) if(bitlen[n] != 0) tree1d[n] = nextcode[bitlen[n]]++; //generate all the codes
				tree2d.clear(); tree2d.resize(numcodes * 2, 32767); //32767 here means the tree2d isn't filled there yet
				for(uint32_t n = 0; n < numcodes; n++) //the codes
				for(uint32_t i = 0; i < bitlen[n]; i++) //the bits for this code
				{
					uint32_t bit = (tree1d[n] >> (bitlen[n] - i - 1)) & 1;
					if(treepos > numcodes - 2) return 55;
					if(tree2d[2 * treepos + bit] == 32767) //not yet filled in
					{
						if(i + 1 == bitlen[n]) { tree2d[2 * treepos + bit] = n; treepos = 0; } //last bit
						else { tree2d[2 * treepos + bit] = ++nodefilled + numcodes; treepos = nodefilled; } //addresses are encoded as values > numcodes
					}
					else treepos = tree2d[2 * treepos + bit] - numcodes; //subtract numcodes from address to get address value
				}
				return 0;
			}
			int decode(bool& decoded, uint32_t& result, size_t& treepos, uint32_t bit) const
			{ //Decodes a symbol from the tree
				uint32_t numcodes = (uint32_t)tree2d.size() / 2;
				if(treepos >= numcodes) return 11; //error: you appeared outside the codetree
				result = tree2d[2 * treepos + bit];
				decoded = (result < numcodes);
				treepos = decoded ? 0 : result - numcodes;
				return 0;
			}
			std::vector<uint32_t> tree2d; //2D representation of a huffman tree: The one dimension is "0" or "1", the other contains all nodes and leaves of the tree.
		};
		struct Inflator
		{
			int error;
			void inflate(std::vector<uint8_t>& out, const std::vector<uint8_t>& in, size_t inpos = 0)
			{
				size_t bp = 0, pos = 0; //bit pointer and byte pointer
				error = 0;
				uint32_t BFINAL = 0;
				while(!BFINAL && !error)
				{
					if(bp >> 3 >= in.size()) { error = 52; return; } //error, bit pointer will jump past memory
					BFINAL = readBitFromStream(bp, &in[inpos]);
					uint32_t BTYPE = readBitFromStream(bp, &in[inpos]); BTYPE += 2 * readBitFromStream(bp, &in[inpos]);
					if(BTYPE == 3) { error = 20; return; } //error: invalid BTYPE
					else if(BTYPE == 0) inflateNoCompression(out, &in[inpos], bp, pos, in.size());
					else inflateHuffmanBlock(out, &in[inpos], bp, pos, in.size(), BTYPE);
				}
				if(!error) out.resize(pos); //Only now we know the true size of out, resize it to that
			}
			void generateFixedTrees(HuffmanTree& tree, HuffmanTree& treeD) //get the tree of a deflated block with fixed tree
			{
				std::vector<uint32_t> bitlen(288, 8), bitlenD(32, 5);;
				for(size_t i = 144; i <= 255; i++) bitlen[i] = 9;
				for(size_t i = 256; i <= 279; i++) bitlen[i] = 7;
				tree.makeFromLengths(bitlen, 15);
				treeD.makeFromLengths(bitlenD, 15);
			}
			HuffmanTree codetree, codetreeD, codelengthcodetree; //the code tree for Huffman codes, dist codes, and code length codes
			uint32_t huffmanDecodeSymbol(const uint8_t* in, size_t& bp, const HuffmanTree& codetree, size_t inlength)
			{ //decode a single symbol from given list of bits with given code tree. return value is the symbol
				bool decoded; uint32_t ct;
				for(size_t treepos = 0;;)
				{
					if((bp & 0x07) == 0 && (bp >> 3) > inlength) { error = 10; return 0; } //error: end reached without endcode
					error = codetree.decode(decoded, ct, treepos, readBitFromStream(bp, in)); if(error) return 0; //stop, an error happened
					if(decoded) return ct;
				}
			}
			void getTreeInflateDynamic(HuffmanTree& tree, HuffmanTree& treeD, const uint8_t* in, size_t& bp, size_t inlength)
			{ //get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree
				std::vector<uint32_t> bitlen(288, 0), bitlenD(32, 0);
				if(bp >> 3 >= inlength - 2) { error = 49; return; } //the bit pointer is or will go past the memory
				size_t HLIT =	readBitsFromStream(bp, in, 5) + 257; //number of literal/length codes + 257
				size_t HDIST = readBitsFromStream(bp, in, 5) + 1; //number of dist codes + 1
				size_t HCLEN = readBitsFromStream(bp, in, 4) + 4; //number of code length codes + 4
				std::vector<uint32_t> codelengthcode(19); //lengths of tree to decode the lengths of the dynamic tree
				for(size_t i = 0; i < 19; i++) codelengthcode[CLCL[i]] = (i < HCLEN) ? readBitsFromStream(bp, in, 3) : 0;
				error = codelengthcodetree.makeFromLengths(codelengthcode, 7); if(error) return;
				size_t i = 0, replength;
				while(i < HLIT + HDIST)
				{
					uint32_t code = huffmanDecodeSymbol(in, bp, codelengthcodetree, inlength); if(error) return;
					if(code <= 15)	{ if(i < HLIT) bitlen[i++] = code; else bitlenD[i++ - HLIT] = code; } //a length code
					else if(code == 16) //repeat previous
					{
						if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
						replength = 3 + readBitsFromStream(bp, in, 2);
						uint32_t value; //set value to the previous code
						if((i - 1) < HLIT) value = bitlen[i - 1];
						else value = bitlenD[i - HLIT - 1];
						for(size_t n = 0; n < replength; n++) //repeat this value in the next lengths
						{
							if(i >= HLIT + HDIST) { error = 13; return; } //error: i is larger than the amount of codes
							if(i < HLIT) bitlen[i++] = value; else bitlenD[i++ - HLIT] = value;
						}
					}
					else if(code == 17) //repeat "0" 3-10 times
					{
						if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
						replength = 3 + readBitsFromStream(bp, in, 3);
						for(size_t n = 0; n < replength; n++) //repeat this value in the next lengths
						{
							if(i >= HLIT + HDIST) { error = 14; return; } //error: i is larger than the amount of codes
							if(i < HLIT) bitlen[i++] = 0; else bitlenD[i++ - HLIT] = 0;
						}
					}
					else if(code == 18) //repeat "0" 11-138 times
					{
						if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
						replength = 11 + readBitsFromStream(bp, in, 7);
						for(size_t n = 0; n < replength; n++) //repeat this value in the next lengths
						{
							if(i >= HLIT + HDIST) { error = 15; return; } //error: i is larger than the amount of codes
							if(i < HLIT) bitlen[i++] = 0; else bitlenD[i++ - HLIT] = 0;
						}
					}
					else { error = 16; return; } //error: somehow an unexisting code appeared. This can never happen.
				}
				if(bitlen[256] == 0) { error = 64; return; } //the length of the end code 256 must be larger than 0
				error = tree.makeFromLengths(bitlen, 15); if(error) return; //now we've finally got HLIT and HDIST, so generate the code trees, and the function is done
				error = treeD.makeFromLengths(bitlenD, 15); if(error) return;
			}
			void inflateHuffmanBlock(std::vector<uint8_t>& out, const uint8_t* in, size_t& bp, size_t& pos, size_t inlength, uint32_t btype)
			{
				if(btype == 1) { generateFixedTrees(codetree, codetreeD); }
				else if(btype == 2) { getTreeInflateDynamic(codetree, codetreeD, in, bp, inlength); if(error) return; }
				for(;;)
				{
					uint32_t code = huffmanDecodeSymbol(in, bp, codetree, inlength); if(error) return;
					if(code == 256) return; //end code
					else if(code <= 255) //literal symbol
					{
						if(pos >= out.size()) out.resize((pos + 1) * 2); //reserve more room
						out[pos++] = (uint8_t)(code);
					}
					else if(code >= 257 && code <= 285) //length code
					{
						size_t length = LENBASE[code - 257], numextrabits = LENEXTRA[code - 257];
						if((bp >> 3) >= inlength) { error = 51; return; } //error, bit pointer will jump past memory
						length += readBitsFromStream(bp, in, numextrabits);
						uint32_t codeD = huffmanDecodeSymbol(in, bp, codetreeD, inlength); if(error) return;
						if(codeD > 29) { error = 18; return; } //error: invalid dist code (30-31 are never used)
						uint32_t dist = DISTBASE[codeD], numextrabitsD = DISTEXTRA[codeD];
						if((bp >> 3) >= inlength) { error = 51; return; } //error, bit pointer will jump past memory
						dist += readBitsFromStream(bp, in, numextrabitsD);
						size_t start = pos, back = start - dist; //backwards
						if(pos + length >= out.size()) out.resize((pos + length) * 2); //reserve more room
						for(size_t i = 0; i < length; i++) { out[pos++] = out[back++]; if(back >= start) back = start - dist; }
					}
				}
			}
			void inflateNoCompression(std::vector<uint8_t>& out, const uint8_t* in, size_t& bp, size_t& pos, size_t inlength)
			{
				while((bp & 0x7) != 0) bp++; //go to first boundary of byte
				size_t p = bp / 8;
				if(p >= inlength - 4) { error = 52; return; } //error, bit pointer will jump past memory
				uint32_t LEN = in[p] + 256 * in[p + 1], NLEN = in[p + 2] + 256 * in[p + 3]; p += 4;
				if(LEN + NLEN != 65535) { error = 21; return; } //error: NLEN is not one's complement of LEN
				if(pos + LEN >= out.size()) out.resize(pos + LEN);
				if(p + LEN > inlength) { error = 23; return; } //error: reading outside of in buffer
				for(uint32_t n = 0; n < LEN; n++) out[pos++] = in[p++]; //read LEN bytes of literal data
				bp = p * 8;
			}
		};
		int decompress(std::vector<uint8_t>& out, const std::vector<uint8_t>& in) //returns error value
		{
			Inflator inflator;
			if(in.size() < 2) { return 53; } //error, size of zlib data too small
			if((in[0] * 256 + in[1]) % 31 != 0) { return 24; } //error: 256 * in[0] + in[1] must be a multiple of 31, the FCHECK value is supposed to be made that way
			uint32_t CM = in[0] & 15, CINFO = (in[0] >> 4) & 15, FDICT = (in[1] >> 5) & 1;
			if(CM != 8 || CINFO > 7) { return 25; } //error: only compression method 8: inflate with sliding window of 32k is supported by the PNG spec
			if(FDICT != 0) { return 26; } //error: the specification of PNG says about the zlib stream: "The additional flags shall not specify a preset dictionary."
			inflator.inflate(out, in, 2);
			return inflator.error; //note: adler32 checksum was skipped and ignored
		}
	};
	struct PNG //nested functions for PNG decoding
	{
		struct Info
		{
			uint32_t width, height, colorType, bitDepth, compressionMethod, filterMethod, interlaceMethod, key_r, key_g, key_b;
			bool key_defined; //is a transparent color key given?
			std::vector<uint8_t> palette;
		} info;
		int error;
		void decode(std::vector<uint8_t>& out, const uint8_t* in, uint32_t size)
		{
			error = 0;
			if(size == 0 || in == 0) { error = 48; return; } //the given data is empty
			readPngHeader(&in[0], size); if(error) return;
			size_t pos = 33; //first byte of the first chunk after the header
			std::vector<uint8_t> idat; //the data from idat chunks
			bool IEND = false, known_type = true;
			info.key_defined = false;
			while(!IEND) //loop through the chunks, ignoring unknown chunks and stopping at IEND chunk. IDAT data is put at the start of the in buffer
			{
				if(pos + 8 >= size) { error = 30; return; } //error: size of the in buffer too small to contain next chunk
				size_t chunkLength = read32bitInt(&in[pos]); pos += 4;
				if(chunkLength > 2147483647) { error = 63; return; }
				if(pos + chunkLength >= size) { error = 35; return; } //error: size of the in buffer too small to contain next chunk
				if(in[pos + 0] == 'I' && in[pos + 1] == 'D' && in[pos + 2] == 'A' && in[pos + 3] == 'T') //IDAT chunk, containing compressed image data
				{
					idat.insert(idat.end(), &in[pos + 4], &in[pos + 4 + chunkLength]);
					pos += (4 + chunkLength);
				}
				else if(in[pos + 0] == 'I' && in[pos + 1] == 'E' && in[pos + 2] == 'N' && in[pos + 3] == 'D')	{ pos += 4; IEND = true; }
				else if(in[pos + 0] == 'P' && in[pos + 1] == 'L' && in[pos + 2] == 'T' && in[pos + 3] == 'E') //palette chunk (PLTE)
				{
					pos += 4; //go after the 4 letters
					info.palette.resize(4 * (chunkLength / 3));
					if(info.palette.size() > (4 * 256)) { error = 38; return; } //error: palette too big
					for(size_t i = 0; i < info.palette.size(); i += 4)
					{
						for(size_t j = 0; j < 3; j++) info.palette[i + j] = in[pos++]; //RGB
						info.palette[i + 3] = 255; //alpha
					}
				}
				else if(in[pos + 0] == 't' && in[pos + 1] == 'R' && in[pos + 2] == 'N' && in[pos + 3] == 'S') //palette transparency chunk (tRNS)
				{
					pos += 4; //go after the 4 letters
					if(info.colorType == 3)
					{
						if(4 * chunkLength > info.palette.size()) { error = 39; return; } //error: more alpha values given than there are palette entries
						for(size_t i = 0; i < chunkLength; i++) info.palette[4 * i + 3] = in[pos++];
					}
					else if(info.colorType == 0)
					{
						if(chunkLength != 2) { error = 40; return; } //error: this chunk must be 2 bytes for greyscale image
						info.key_defined = 1; info.key_r = info.key_g = info.key_b = 256 * in[pos] + in[pos + 1]; pos += 2;
					}
					else if(info.colorType == 2)
					{
						if(chunkLength != 6) { error = 41; return; } //error: this chunk must be 6 bytes for RGB image
						info.key_defined = 1;
						info.key_r = 256 * in[pos] + in[pos + 1]; pos += 2;
						info.key_g = 256 * in[pos] + in[pos + 1]; pos += 2;
						info.key_b = 256 * in[pos] + in[pos + 1]; pos += 2;
					}
					else { error = 42; return; } //error: tRNS chunk not allowed for other color models
				}
				else //it's not an implemented chunk type, so ignore it: skip over the data
				{
					if(!(in[pos + 0] & 32)) { error = 69; return; } //error: unknown critical chunk (5th bit of first byte of chunk type is 0)
					pos += (chunkLength + 4); //skip 4 letters and uninterpreted data of unimplemented chunk
					known_type = false;
				}
				pos += 4; //step over CRC (which is ignored)
			}
			uint32_t bpp = getBpp(info);
			std::vector<uint8_t> scanlines(((info.width * (info.height * bpp + 7)) / 8) + info.height); //now the out buffer will be filled
			Zlib zlib; //decompress with the Zlib decompressor
			error = zlib.decompress(scanlines, idat); if(error) return; //stop if the zlib decompressor returned an error
			size_t bytewidth = (bpp + 7) / 8, outlength = (info.height * info.width * bpp + 7) / 8;
			out.resize(outlength); //time to fill the out buffer
			uint8_t* out_ = outlength ? &out[0] : 0; //use a regular pointer to the std::vector for faster code if compiled without optimization
			if(info.interlaceMethod == 0) //no interlace, just filter
			{
				size_t linestart = 0, linelength = (info.width * bpp + 7) / 8; //length in bytes of a scanline, excluding the filtertype byte
				if(bpp >= 8) //byte per byte
				for(uint32_t y = 0; y < info.height; y++)
				{
					uint32_t filterType = scanlines[linestart];
					const uint8_t* prevline = (y == 0) ? 0 : &out_[(y - 1) * info.width * bytewidth];
					unFilterScanline(&out_[linestart - y], &scanlines[linestart + 1], prevline, bytewidth, filterType,	linelength); if(error) return;
					linestart += (1 + linelength); //go to start of next scanline
				}
				else //less than 8 bits per pixel, so fill it up bit per bit
				{
					std::vector<uint8_t> templine((info.width * bpp + 7) >> 3); //only used if bpp < 8
					for(size_t y = 0, obp = 0; y < info.height; y++)
					{
						uint32_t filterType = scanlines[linestart];
						const uint8_t* prevline = (y == 0) ? 0 : &out_[(y - 1) * info.width * bytewidth];
						unFilterScanline(&templine[0], &scanlines[linestart + 1], prevline, bytewidth, filterType, linelength); if(error) return;
						for(size_t bp = 0; bp < info.width * bpp;) setBitOfReversedStream(obp, out_, readBitFromReversedStream(bp, &templine[0]));
						linestart += (1 + linelength); //go to start of next scanline
					}
				}
			}
			else //interlaceMethod is 1 (Adam7)
			{
				size_t passw[7] = { (info.width + 7) / 8, (info.width + 3) / 8, (info.width + 3) / 4, (info.width + 1) / 4, (info.width + 1) / 2, (info.width + 0) / 2, (info.width + 0) / 1 };
				size_t passh[7] = { (info.height + 7) / 8, (info.height + 7) / 8, (info.height + 3) / 8, (info.height + 3) / 4, (info.height + 1) / 4, (info.height + 1) / 2, (info.height + 0) / 2 };
				size_t passstart[7] = {0};
				size_t pattern[28] = {0,4,0,2,0,1,0,0,0,4,0,2,0,1,8,8,4,4,2,2,1,8,8,8,4,4,2,2}; //values for the adam7 passes
				for(int i = 0; i < 6; i++) passstart[i + 1] = passstart[i] + passh[i] * ((passw[i] ? 1 : 0) + (passw[i] * bpp + 7) / 8);
				std::vector<uint8_t> scanlineo((info.width * bpp + 7) / 8), scanlinen((info.width * bpp + 7) / 8); //"old" and "new" scanline
				for(int i = 0; i < 7; i++)
					adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[i]], info.width, pattern[i], pattern[i + 7], pattern[i + 14], pattern[i + 21], passw[i], passh[i], bpp);
			}
			if(info.colorType != 6 || info.bitDepth != 8) //conversion needed
			{
				std::vector<uint8_t> data = out;
				error = convert(out, &data[0], info, info.width, info.height);
			}
		}
		void readPngHeader(const uint8_t* in, size_t inlength) //read the information from the header and store it in the Info
		{
			if(inlength < 29) { error = 27; return; } //error: the data length is smaller than the length of the header
			if(in[0] != 137 || in[1] != 80 || in[2] != 78 || in[3] != 71 || in[4] != 13 || in[5] != 10 || in[6] != 26 || in[7] != 10) { error = 28; return; } //no PNG signature
			if(in[12] != 'I' || in[13] != 'H' || in[14] != 'D' || in[15] != 'R') { error = 29; return; } //error: it doesn't start with a IHDR chunk!
			info.width = read32bitInt(&in[16]); info.height = read32bitInt(&in[20]);
			info.bitDepth = in[24]; info.colorType = in[25];
			info.compressionMethod = in[26]; if(in[26] != 0) { error = 32; return; } //error: only compression method 0 is allowed in the specification
			info.filterMethod = in[27]; if(in[27] != 0) { error = 33; return; } //error: only filter method 0 is allowed in the specification
			info.interlaceMethod = in[28]; if(in[28] > 1) { error = 34; return; } //error: only interlace methods 0 and 1 exist in the specification
			error = checkColorValidity(info.colorType, info.bitDepth);
		}
		void unFilterScanline(uint8_t* recon, const uint8_t* scanline, const uint8_t* precon, size_t bytewidth, uint32_t filterType, size_t length)
		{
			switch(filterType)
			{
				case 0: for(size_t i = 0; i < length; i++) recon[i] = scanline[i]; break;
				case 1:
					for(size_t i =				 0; i < bytewidth; i++) recon[i] = scanline[i];
					for(size_t i = bytewidth; i <		length; i++) recon[i] = scanline[i] + recon[i - bytewidth];
					break;
				case 2:
					if(precon) for(size_t i = 0; i < length; i++) recon[i] = scanline[i] + precon[i];
					else			 for(size_t i = 0; i < length; i++) recon[i] = scanline[i];
					break;
				case 3:
					if(precon)
					{
						for(size_t i =				 0; i < bytewidth; i++) recon[i] = scanline[i] + precon[i] / 2;
						for(size_t i = bytewidth; i <		length; i++) recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) / 2);
					}
					else
					{
						for(size_t i =				 0; i < bytewidth; i++) recon[i] = scanline[i];
						for(size_t i = bytewidth; i <		length; i++) recon[i] = scanline[i] + recon[i - bytewidth] / 2;
					}
					break;
				case 4:
					if(precon)
					{
						for(size_t i =				 0; i < bytewidth; i++) recon[i] = (uint8_t)(scanline[i] + paethPredictor(0, precon[i], 0));
						for(size_t i = bytewidth; i <		length; i++) recon[i] = (uint8_t)(scanline[i] + paethPredictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
					}
					else
					{
						for(size_t i =				 0; i < bytewidth; i++) recon[i] = scanline[i];
						for(size_t i = bytewidth; i <		length; i++) recon[i] = (uint8_t)(scanline[i] + paethPredictor(recon[i - bytewidth], 0, 0));
					}
					break;
				default: error = 36; return; //error: unexisting filter type given
			}
		}
		void adam7Pass(uint8_t* out, uint8_t* linen, uint8_t* lineo, const uint8_t* in, uint32_t w, size_t passleft, size_t passtop, size_t spacex, size_t spacey, size_t passw, size_t passh, uint32_t bpp)
		{ //filter and reposition the pixels into the output when the image is Adam7 interlaced. This function can only do it after the full image is already decoded. The out buffer must have the correct allocated memory size already.
			if(passw == 0) return;
			size_t bytewidth = (bpp + 7) / 8, linelength = 1 + ((bpp * passw + 7) / 8);
			for(uint32_t y = 0; y < passh; y++)
			{
				uint8_t filterType = in[y * linelength], *prevline = (y == 0) ? 0 : lineo;
				unFilterScanline(linen, &in[y * linelength + 1], prevline, bytewidth, filterType, (w * bpp + 7) / 8); if(error) return;
				if(bpp >= 8) for(size_t i = 0; i < passw; i++) for(size_t b = 0; b < bytewidth; b++) //b = current byte of this pixel
					out[bytewidth * w * (passtop + spacey * y) + bytewidth * (passleft + spacex * i) + b] = linen[bytewidth * i + b];
				else for(size_t i = 0; i < passw; i++)
				{
					size_t obp = bpp * w * (passtop + spacey * y) + bpp * (passleft + spacex * i), bp = i * bpp;
					for(size_t b = 0; b < bpp; b++) setBitOfReversedStream(obp, out, readBitFromReversedStream(bp, &linen[0]));
				}
				uint8_t* temp = linen; linen = lineo; lineo = temp; //swap the two buffer pointers "line old" and "line new"
			}
		}
		static uint32_t readBitFromReversedStream(size_t& bitp, const uint8_t* bits) { uint32_t result = (bits[bitp >> 3] >> (7 - (bitp & 0x7))) & 1; bitp++; return result;}
		static uint32_t readBitsFromReversedStream(size_t& bitp, const uint8_t* bits, uint32_t nbits)
		{
			uint32_t result = 0;
			for(size_t i = nbits - 1; i < nbits; i--) result += ((readBitFromReversedStream(bitp, bits)) << i);
			return result;
		}
		void setBitOfReversedStream(size_t& bitp, uint8_t* bits, uint32_t bit) { bits[bitp >> 3] |=	(bit << (7 - (bitp & 0x7))); bitp++; }
		uint32_t read32bitInt(const uint8_t* buffer) { return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]; }
		int checkColorValidity(uint32_t colorType, uint32_t bd) //return type is a LodePNG error code
		{
			if((colorType == 2 || colorType == 4 || colorType == 6)) { if(!(bd == 8 || bd == 16)) return 37; else return 0; }
			else if(colorType == 0) { if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8 || bd == 16)) return 37; else return 0; }
			else if(colorType == 3) { if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8						)) return 37; else return 0; }
			else return 31; //unexisting color type
		}
		uint32_t getBpp(const Info& info)
		{
			if(info.colorType == 2) return (3 * info.bitDepth);
			else if(info.colorType >= 4) return (info.colorType - 2) * info.bitDepth;
			else return info.bitDepth;
		}
		int convert(std::vector<uint8_t>& out, const uint8_t* in, Info& infoIn, uint32_t w, uint32_t h)
		{ //converts from any color type to 32-bit. return value = LodePNG error code
			size_t numpixels = w * h, bp = 0;
			out.resize(numpixels * 4);
			uint8_t* out_ = out.empty() ? 0 : &out[0]; //faster if compiled without optimization
			if(infoIn.bitDepth == 8 && infoIn.colorType == 0) //greyscale
			for(size_t i = 0; i < numpixels; i++)
			{
				out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[i];
				out_[4 * i + 3] = (infoIn.key_defined && in[i] == infoIn.key_r) ? 0 : 255;
			}
			else if(infoIn.bitDepth == 8 && infoIn.colorType == 2) //RGB color
			for(size_t i = 0; i < numpixels; i++)
			{
				for(size_t c = 0; c < 3; c++) out_[4 * i + c] = in[3 * i + c];
				out_[4 * i + 3] = (infoIn.key_defined == 1 && in[3 * i + 0] == infoIn.key_r && in[3 * i + 1] == infoIn.key_g && in[3 * i + 2] == infoIn.key_b) ? 0 : 255;
			}
			else if(infoIn.bitDepth == 8 && infoIn.colorType == 3) //indexed color (palette)
			for(size_t i = 0; i < numpixels; i++)
			{
				if(4U * in[i] >= infoIn.palette.size()) return 46;
				for(size_t c = 0; c < 4; c++) out_[4 * i + c] = infoIn.palette[4 * in[i] + c]; //get rgb colors from the palette
			}
			else if(infoIn.bitDepth == 8 && infoIn.colorType == 4) //greyscale with alpha
			for(size_t i = 0; i < numpixels; i++)
			{
				out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[2 * i + 0];
				out_[4 * i + 3] = in[2 * i + 1];
			}
			else if(infoIn.bitDepth == 8 && infoIn.colorType == 6) for(size_t i = 0; i < numpixels; i++) for(size_t c = 0; c < 4; c++) out_[4 * i + c] = in[4 * i + c]; //RGB with alpha
			else if(infoIn.bitDepth == 16 && infoIn.colorType == 0) //greyscale
			for(size_t i = 0; i < numpixels; i++)
			{
				out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[2 * i];
				out_[4 * i + 3] = (infoIn.key_defined && 256U * in[i] + in[i + 1] == infoIn.key_r) ? 0 : 255;
			}
			else if(infoIn.bitDepth == 16 && infoIn.colorType == 2) //RGB color
			for(size_t i = 0; i < numpixels; i++)
			{
				for(size_t c = 0; c < 3; c++) out_[4 * i + c] = in[6 * i + 2 * c];
				out_[4 * i + 3] = (infoIn.key_defined && 256U*in[6*i+0]+in[6*i+1] == infoIn.key_r && 256U*in[6*i+2]+in[6*i+3] == infoIn.key_g && 256U*in[6*i+4]+in[6*i+5] == infoIn.key_b) ? 0 : 255;
			}
			else if(infoIn.bitDepth == 16 && infoIn.colorType == 4) //greyscale with alpha
			for(size_t i = 0; i < numpixels; i++)
			{
				out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[4 * i]; //most significant byte
				out_[4 * i + 3] = in[4 * i + 2];
			}
			else if(infoIn.bitDepth == 16 && infoIn.colorType == 6) for(size_t i = 0; i < numpixels; i++) for(size_t c = 0; c < 4; c++) out_[4 * i + c] = in[8 * i + 2 * c]; //RGB with alpha
			else if(infoIn.bitDepth < 8 && infoIn.colorType == 0) //greyscale
			for(size_t i = 0; i < numpixels; i++)
			{
				uint32_t value = (readBitsFromReversedStream(bp, in, infoIn.bitDepth) * 255) / ((1 << infoIn.bitDepth) - 1); //scale value from 0 to 255
				out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = (uint8_t)(value);
				out_[4 * i + 3] = (infoIn.key_defined && value && ((1U << infoIn.bitDepth) - 1U) == infoIn.key_r && ((1U << infoIn.bitDepth) - 1U)) ? 0 : 255;
			}
			else if(infoIn.bitDepth < 8 && infoIn.colorType == 3) //palette
			for(size_t i = 0; i < numpixels; i++)
			{
				uint32_t value = readBitsFromReversedStream(bp, in, infoIn.bitDepth);
				if(4 * value >= infoIn.palette.size()) return 47;
				for(size_t c = 0; c < 4; c++) out_[4 * i + c] = infoIn.palette[4 * value + c]; //get rgb colors from the palette
			}
			return 0;
		}
		long paethPredictor(long a, long b, long c) //Paeth predicter, used by PNG filter type 4
		{
			long p = a + b - c, pa = p > a ? (p - a) : (a - p), pb = p > b ? (p - b) : (b - p), pc = p > c ? (p - c) : (c - p);
			return (pa <= pb && pa <= pc) ? a : pb <= pc ? b : c;
		}
	};
	PNG decoder; decoder.decode(out_image_32bit, in_png, in_size);
	image_width = decoder.info.width; image_height = decoder.info.height;
	return decoder.error;
}

/* Global Variables */
Uint32 sdlgraphics_flags = 0;
LuxColour sdlgraphics_colour = {0,0,0,255};
SDL_Rect sdlgraphics_dimension = {0, 0, 320,240};
SDL_Rect * sdlgraphics_dirty = 0;
float sdlgraphics_ratio[2] = {1.0, 1.0};
int32_t sdlgraphics_scale = 0;

SDL_Window * sdlgraphics_window = nullptr;
std::string sdlgraphics_title;
uint32_t sdlgraphics_fps = 0, sdlgraphics_fpstime = 0;
bool sdlgraphics_customtext = false;
int32_t sdlgraphics_customtext_height = 32;


// Loaded a file from the game file.
SDL_Surface * SDL_Surface_LoadImage( std::string file )
{
	uint8_t * data = nullptr;
	uint32_t size;
	SDL_Surface * temp_surface = nullptr;
	if ( lux::game )
	{
		size = lux::game->GetFile(file, &data, false);
		if ( size )
		{
			std::vector<uint8_t> image;
			uint32_t w, h;
			int error = decodePNG(image, w, h, data, size);
			uint8_t * pixels = &image[0];
			if ( !error )
			{
				#if SDL_BYTEORDER == SDL_BIG_ENDIAN
					temp_surface = SDL_CreateRGBSurfaceFrom( &image[0], w, h, 32, 0, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
				#else
					temp_surface = SDL_CreateRGBSurfaceFrom( pixels, w, h, 32, 0, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
				#endif
				SDL_SetAlpha(temp_surface, 0, 255);
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "SDL_Surface_LoadImage:" << w << "x" << h << std::endl;
			}
			else
				lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << file << ": not a png image" << std::endl;
		}
	}

	return temp_surface;
}

SDL_Texture * SDL_Texture_LoadImage( std::string file )
{
	SDL_Surface * temp_surface = SDL_Surface_LoadImage( file );
	if (temp_surface)
		return SDL_CreateTextureFromSurface(0, temp_surface);
	return nullptr;
}

/* Display Functions */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Init( uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen )
{
	if ( SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO )
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	SDL_InitSubSystem(SDL_INIT_VIDEO);


	/* Set Init Flags */
	if ( lux::config->GetString("display.surface") == "hardware" )
		sdlgraphics_flags |= SDL_HWSURFACE;
	else
		sdlgraphics_flags |= SDL_SWSURFACE;

	if ( lux::config->GetBoolean("display.dbuff") )
		sdlgraphics_flags |= SDL_DOUBLEBUF;

	if ( lux::config->GetBoolean("display.fullscreen") )
		sdlgraphics_flags |= SDL_FULLSCREEN;
	sdlgraphics_title = lux::config->GetString("project.title") + " (SDL2 Rendering)";
	sdlgraphics_scale = lux::config->GetNumber("display.autoscale");
	sdlgraphics_dimension.w = width;
	sdlgraphics_dimension.h = height;


	sdlgraphics_window = SDL_CreateWindow(sdlgraphics_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

	SDL_RendererInfo info;
	int driver_count = SDL_GetNumRenderDrivers();
	for (int i=0; i<driver_count; ++i)
	{
		SDL_GetRenderDriverInfo(i, &info);
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Available Render: " << info.name << std::endl;
	}

	if ( SDL_CreateRenderer(sdlgraphics_window, -1, SDL_RENDERER_ACCELERATED ) )
	{
		return false;
	}
	SDL_SelectRenderer(sdlgraphics_window);

	/* Set Window Icon */
	SDL_Surface * icon = SDL_Surface_LoadImage("./sprites/__icon.png");
	if ( icon != nullptr )
	{
		SDL_SetWindowIcon(sdlgraphics_window,icon);
		SDL_FreeSurface(icon);
	}

	const char *name = SDL_GetCurrentVideoDriver();
	SDL_GetRendererInfo( &info);
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Video System: " << name << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Render: " << info.name << std::endl;

	return true;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Destory()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Update( uint8_t screen, LuxRect rect)
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Show( uint8_t screen )
{
	if ( sdlgraphics_window )
	{
		SDL_RenderPresent();
		SDL_SetRenderDrawColor(sdlgraphics_colour.r, sdlgraphics_colour.b, sdlgraphics_colour.g, sdlgraphics_colour.a);
		SDL_RenderClear();
	}
}

// Converts display location to screen location
LUX_DISPLAY_FUNCTION void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y)
{
	if ( sdlgraphics_scale )
	{
		*x = (int32_t)((float)*x * sdlgraphics_ratio[0]);
		*y = (int32_t)((float)*y * sdlgraphics_ratio[1]);
	}
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_BackgroundObject( MapObject background )
{
	/* TODO
	- Draw background object
	*/
	sdlgraphics_colour = background.effects.colour;
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_SetFullscreen( bool able )
{
	return SDL_SetWindowFullscreen(sdlgraphics_window, able) == able;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_TextSprites( bool able )
{
	if ( lux::display->sprite_font.length() )
	{
		sdlgraphics_customtext = able;
		if ( sdlgraphics_customtext )
		{
			LuxSprite * sdata = lux::display->GetSprite( lux::display->sprite_font, "48" );
			if ( sdata )
				sdlgraphics_customtext_height = sdata->rect.h;
			else
				sdlgraphics_customtext_height = 16;
		}
	}
	else
		sdlgraphics_customtext = false;
}

/* Resource Functions */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_RefreshSpriteSheet( std::string name, std::map<std::string, LuxSprite *> * children )
{
	SDL_Surface * parent_sheet = SDL_Surface_LoadImage("./sprites/" + name);
	if ( !parent_sheet )
		return false;

	std::map<std::string, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			if ( p->second->data )
			{
				// Delete Old Data
				SDL_DestroyTexture((SDL_Texture *)p->second->data);
			}
			SDL_Rect tmp_rect;
			tmp_rect.x = p->second->rect.x;
			tmp_rect.y = p->second->rect.y;
			tmp_rect.w = p->second->rect.w;
			tmp_rect.h = p->second->rect.h;

			SDL_Surface * temp_sheet = SDL_CreateRGBSurface( sdlgraphics_flags, tmp_rect.w, tmp_rect.h, 32, RMASK, GMASK, BMASK, AMASK );
			SDL_BlitSurface(parent_sheet, &tmp_rect, temp_sheet, nullptr);
			SDL_SetAlpha(temp_sheet, 0, 0);

			if ( temp_sheet )
			{
				p->second->data = (void *)SDL_CreateTextureFromSurface(0, temp_sheet);
			}
			SDL_FreeSurface( temp_sheet );
		}
	}
	SDL_FreeSurface( parent_sheet );
	return true;
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheet( std::string name, std::map<std::string, LuxSprite *> * children )
{
	SDL_Surface * parent_sheet = SDL_Surface_LoadImage("./sprites/" + name);
	if ( !parent_sheet )
		return false;
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Lux_NATIVE_LoadSpriteSheet:" << parent_sheet->w << "x" << parent_sheet->h << std::endl;
	std::map<std::string, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			SDL_Rect tmp_rect;
			tmp_rect.x = p->second->rect.x;
			tmp_rect.y = p->second->rect.y;
			tmp_rect.w = p->second->rect.w;
			tmp_rect.h = p->second->rect.h;

			SDL_Surface * temp_sheet = SDL_CreateRGBSurface( 0, tmp_rect.w, tmp_rect.h, 32, RMASK, GMASK, BMASK, AMASK );

			if( temp_sheet == nullptr )
			{
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
			}
			else
			{
				if ( tmp_rect.x + tmp_rect.w < parent_sheet->w && tmp_rect.y + tmp_rect.h < parent_sheet->h)
				{
					SDL_BlitSurface(parent_sheet, &tmp_rect, temp_sheet, nullptr);
				SDL_SetAlpha(temp_sheet, 0, 0);
				}

			}
			if ( temp_sheet )
			{
				p->second->data = (void *)SDL_CreateTextureFromSurface(0, temp_sheet);
			}
			SDL_FreeSurface( temp_sheet );
		}
	}
	SDL_FreeSurface( parent_sheet );
	return true;
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_FreeSpriteSheet( std::map<std::string, LuxSprite *> * children )
{
	std::map<std::string, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			if ( p->second->data )
			{
				SDL_DestroyTexture((SDL_Texture *)p->second->data);
				p->second->data = nullptr;
			}
		}
	}
	return true;
}


LUX_DISPLAY_FUNCTION LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	LuxSprite * sprite = new LuxSprite;
	std::vector<uint8_t> image;
	uint32_t w, h;

	int error = decodePNG(image, w, h, data, size);

	if ( !error )
	{
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			SDL_Surface * surface = SDL_CreateRGBSurfaceFrom( &image[0], w, h, 32, 0, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		#else
			SDL_Surface * surface = SDL_CreateRGBSurfaceFrom( &image[0], w, h, 32, 0, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		#endif
		if ( surface )
		{
			sprite->data = (void*) SDL_CreateTextureFromSurface(0, surface);
		}
		SDL_FreeSurface(surface);
		image.clear();
		return sprite;
	}
	else
	{
		delete sprite;
		return nullptr;
	}

}

/* Drawing Functions */

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, Effects image_effects )
{
	if ( data == nullptr )
		return;

	int32_t width = 0, height = 0;
	SDL_Texture * surface = (SDL_Texture*)data;
	SDL_QueryTexture(surface, nullptr, nullptr, &width, &height);

	if ( width == 0 || height == 0 )
	{
		return;
	}

	SDL_Rect draw, area;
	draw.x = dest_rect.x;
	draw.y = dest_rect.y;
	draw.w = width;
	draw.h = height;

	if ( (dest_rect.w == draw.w) && (dest_rect.h == draw.h) )
	{
		dest_rect.w = dest_rect.h = 0;
		image_effects.tile = 0;
	}


	if ( image_effects.flipmode&16 )
	{
		image_effects.flipmode -= 16;
	}

	if ( (dest_rect.w > 0 || dest_rect.h > 0) && image_effects.tile )
	{
		if ( image_effects.flipmode == 1 || image_effects.flipmode == 3 )
		{
			area.w = dest_rect.h;
			area.h = dest_rect.w;
		}
		else
		{
			area.w = dest_rect.w;
			area.h = dest_rect.h;
		}

		for( area.x = 0; area.x < area.w; area.x += width )
		{
			for( area.y = 0; area.y < area.h; area.y += height )
			{
				draw.x = dest_rect.x + area.x;
				draw.y = dest_rect.y + area.y;
				SDL_SetRenderDrawColor(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
				SDL_RenderCopy(surface, nullptr, &draw);
			}
		}
	}
	else
	{
		dest_rect.w = width;
		dest_rect.h = height;
		SDL_SetRenderDrawColor(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
		SDL_RenderCopy(surface, nullptr, &draw);
	}
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawRect( LuxRect dest_rect, Effects image_effects)
{
	SDL_SetRenderDrawColor(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
	SDL_Rect dest;
			dest.x = dest_rect.x;
			dest.y = dest_rect.y;
			dest.w = dest_rect.w;
			dest.h = dest_rect.h;
	SDL_RenderFillRect(&dest);
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawPolygon ( int16_t * x_point, int16_t *y_point, uint16_t point_count, Effects image_effects, void * texture )
{
	/*
	if ( texture != nullptr )
	{
		texturedPolygon(sdlgraphics_screen, x_point, y_point, point_count, (SDL_Surface *)texture, 0,0);
	}
	else
	{
		filledPolygonRGBA(sdlgraphics_screen, x_point, y_point, point_count, image_effects.colour.r, image_effects.colour.g, image_effects.colour.b,image_effects.colour.a);
	}
	//aapolygonRGBA(SDL_Surface * dst, Sint16 * vx, Sint16 * vy,int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	*/
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawCircle( LuxRect dest_rect, Effects image_effects )
{
	//SDL_FillEllipse
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawLine ( LuxRect points, Effects image_effects)
{
	SDL_SetRenderDrawColor(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
	SDL_RenderDrawLine( points.x, points.y, points.w, points.h );
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, Effects image_effects, bool allow_custom)
{
	SDL_Rect dest;
	uint16_t x = dest_rect.x;

	std::string::iterator object;
	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;
		/*
		194-223 2 bytes
		224-239 3 bytes
		240-244 4 bytes
		*/
		if (cchar == '\n' || cchar == '\r')
		{
			dest_rect.y += ( sdlgraphics_customtext && allow_custom ? sdlgraphics_customtext_height + 2 : 10);
			x = dest_rect.x;
		}
		else if ( cchar <= 32 )
		{
			x += 7;
		}
		else if ( cchar <= 128 )
		{
			/*
			if ( sdlgraphics_customtext && allow_custom )
			{
				x += Lux_NATIVE_DrawChar((int8_t)cchar, x, dest_rect.y, image_effects);
			}
			else
			{
				characterRGBA(sdlgraphics_screen, x, dest_rect.y, (int8_t)cchar, image_effects.colour.r, image_effects.colour.g, image_effects.colour.b,255);
				x += 7;
			}
			*/
			dest.x = x;
			dest.y = dest_rect.y;
			dest.w = 4;
			dest.h = 7;
			SDL_SetRenderDrawColor(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
			SDL_RenderDrawRect(&dest);

		}
		else if ( cchar < 224 )
		{
			x += 7;
			object++;
		}
		else if ( cchar < 240 )
		{
			x += 7;
			object +=2;
		}
		else if ( cchar < 245 )
		{
			x += 7;
			object +=3;
		}
	}
}






