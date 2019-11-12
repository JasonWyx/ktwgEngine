#include "codingcompressions.h"
#include <unordered_map>
#include <queue>

namespace Compression
{

std::vector<uint8_t> EncodeRLE(const void* data, size_t length)
{
    std::vector<uint8_t> result;

    if(length > 0)
    {
        result.reserve(length * 2);
        const uint8_t* castedData = static_cast<const uint8_t*>(data);

        uint8_t currentCount = 1;
        uint8_t currentData = castedData[0];

        for(size_t i = 1; i < length; i++)
        {
            // Check for new symbol or count overflow
            if(castedData[i] != currentData || currentCount + 1 == 0)
            {
                result.push_back(currentCount);
                result.push_back(currentData);
            }
            else
            {
                currentCount = 0;
                currentData = castedData[i];
            }
            currentCount++;
        }

        // Append left over
        result.push_back(currentCount);
        result.push_back(currentData);
    }

    return result;
}

std::vector<uint8_t> DecodeRLE(const void* data, size_t length)
{
    std::vector<uint8_t> result;

    if(length > 0)
    {
        result.reserve(length);
        const uint8_t* castedData = static_cast<const uint8_t*>(data);

        for(size_t i = 0; i < length; i += 2)
        {
            uint8_t currentCount = castedData[i];
            uint8_t currentData = castedData[i+1];

            result.insert(result.end(), currentCount, currentData);
        }
    }

    return result;
}

struct HuffmanNode
{
    uint16_t m_Symbol;
    size_t m_Frequency;
    HuffmanNode* m_Left;
    HuffmanNode* m_Right;
};

struct SymbolMask
{
    // Assume 256 number of mask (8 bits)
    std::bitset<64> mask[4];

    std::bitset<64>::reference operator[](size_t index)
    {
        return mask[index / 64][index % 64];
    }

    bool operator[](size_t index) const
    {
        return mask[index / 64][index % 64];
    }

    BitStream GetBitStream() const
    {
        return BitStream() << mask[0] << mask[1] << mask[2] << mask[3];
    }
     
};

void EncodeHuffmanTable(HuffmanNode* node, BitStream code, std::array<BitStream, 256>& table)
{
    // This is the leaf node
    if (node->m_Left == nullptr && node->m_Right == nullptr)
    {
        table[node->m_Symbol] = code;
    }
    else
    {
        EncodeHuffmanTable(node->m_Left, code + BitStream() << false, table);
        EncodeHuffmanTable(node->m_Left, code + BitStream() << true,  table);
    }
}

void DecodeHuffmanTable(const SymbolMask& mask, std::vector<BitStream>& table)
{

}

BitStream EncodeHuffman(const void* data, const size_t length, std::array<BitStream, 256>& table)
{
    size_t frequencies[NumSymbols] = { };

    const uint8_t* castedData = static_cast<const uint8_t*>(data);

    for (size_t i = 0; i < length; i++)
    {
        ++frequencies[castedData[i]];
    }

    auto comparator = [](HuffmanNode* a, HuffmanNode* b) { return a->m_Frequency > b->m_Frequency; };

    std::priority_queue<HuffmanNode*, decltype(comparator) > pq;
    std::unordered_map<uint8_t, BitStream> huffmanCodes;

    for (size_t i = 0; i < sizeof(frequencies); i++)
    {
        if (frequencies[i] > 0)
        {
            pq.push(new HuffmanNode{ i, frequencies[i], nullptr, nullptr });
        }
    }

    while (pq.size() > 1)
    {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        size_t sum = left->m_Frequency + right->m_Frequency;

        pq.push(new HuffmanNode{ 0xFFFF, sum, left, right });
    }

    HuffmanNode* rootNode = pq.top();
    pq.pop();


    EncodeHuffmanTable(rootNode, BitStream(), table);

    return BitStream();
}

BitStream DecodeHuffman(const void* data, const size_t length, const std::array<BitStream, 256>& table)
{
    return BitStream();
}



} // namespace Compression

