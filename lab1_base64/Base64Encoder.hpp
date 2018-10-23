#pragma once

class Base64Encoder
{
public:
    static void encode(const char (&input)[3], char (&output)[4])
    {
        output[0] = lookupTable[(input[0] >> 2)                  ]; // 6 first bits
        output[1] = lookupTable[(input[0] << 6) | (input[1] >> 4)]; // last 2 + first 4 bits
        output[2] = lookupTable[(input[1] << 4) | (input[2] >> 6)]; // last 4 + first 2 bits
        output[3] = lookupTable[(input[2] << 2)]; // last 6 bits
    }












private:
    static constexpr char lookupTable[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
};
/*

*/
