#pragma once

#include<ostream>

class Base64Encoder
{
public:
    static void encodeStream(std::istream &input, std::ostream &output)
    {
        unsigned char inputBytes[3], outputBytes[4];

        while(true)
        {
            input.read((char*)&inputBytes, 3);
        
            int read = input.gcount();
            if(!read) // EOF reached
                break;
           
            if(read == 3)
            {
                encodeBytes(inputBytes, outputBytes);
            }
            else // 2 or 1
            {
                inputBytes[2] = 0;
                if(read == 1)
                    inputBytes[1] = 0;
                
                encodeBytes(inputBytes, outputBytes, 3 - read);
            }

            output.write((char*)&outputBytes, 4);
        }
    }

    static void encodeBytes(const unsigned char (&input)[3], unsigned char (&output)[4], int padding = 0)
    {
        unsigned int tmp = (input[0] << 16) +
                  (input[1] << 8) +
                  (input[2]);

        output[0] = lookupTable[(tmp >> 18) & 0x3F];
        output[1] = lookupTable[(tmp >> 12) & 0x3F];
        output[2] = padding > 1 ? '=' : lookupTable[(tmp >> 6)  & 0x3F];
        output[3] = padding > 0 ? '=' : lookupTable[ tmp        & 0x3F];
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
