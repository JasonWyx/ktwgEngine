#pragma once
#include <vector>
#include <array>
#include <bitset>
#include "bitstream.h"

namespace Compression
{

// Run-Length Coding
std::vector<uint8_t> EncodeRLE(const void* data, const size_t length);
std::vector<uint8_t> DecodeRLE(const void* data, const size_t length);

// Huffman Coding
static constexpr size_t NumSymbols = 1 << 8;

BitStream EncodeHuffman(const void* data, const size_t length, std::array<BitStream, 256>& table);
BitStream DecodeHuffman(const void* data, const size_t length, const std::array<BitStream, 256>& table);

// Arithmetic Coding

} // namespace Compression