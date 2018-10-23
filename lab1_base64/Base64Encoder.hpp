#pragma once

class Base64Encoder
{
public:
    static void encode(const char (&input)[3], char (&output)[4], int padding = 0)
    {
        output[0] = lookupTable[(input[0] >> 2)]; // get first 6 bits from input[0]
        
        // get last 2 bits from input[0], move them 4 positions to the left
        // and append first 4 bits from input[1] to the result
        output[1] = lookupTable[((input[0] & 0x3 ) << 4) | (input[1] >> 4)];
        
        // get last 4 bits from input[1], move them 2 positions to the left
        // and append first 2 bits from input[2] to the result
        output[2] = padding > 1 ? '=' : lookupTable[((input[1] & 0xF ) << 2) | (input[2] >> 6)];
        
        output[3] = padding ? '=' : lookupTable[(input[2] &  0x3F)]; // get last 6 bits from input[2]
    }

private:
    static constexpr char lookupTable[64] = {   
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/',
        };
};
