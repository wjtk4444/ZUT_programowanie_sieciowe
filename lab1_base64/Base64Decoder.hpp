#pragma once

#include<ostream>

class Base64Decoder
{
public:
    static void decodeStream(std::istream &input, std::ostream &output)
    {
        unsigned char inputBytes[4], outputBytes[3];

        while(true)
        {
            input.read((char*)&inputBytes, 4);
        
            int read = input.gcount();
            if(!read) // EOF reached
                break;
            
            if(read == 4)
            {
                decodeBytes(inputBytes, outputBytes);
            }
            else // <4
            {
                inputBytes[3] = 0;
                if(read < 3)
                    inputBytes[2] = 0;
                if(read < 2)
                    inputBytes[1] = 0;

                decodeBytes(inputBytes, outputBytes);
            }

            output.write((char*)&outputBytes, 3);
        }
    }

    static void decodeBytes(const unsigned char (&input)[4], unsigned char (&output)[3])
    {
        unsigned int tmp = (reverseLookupTable[input[0]] << 18) +
                  (reverseLookupTable[input[1]] << 12) +
                  (reverseLookupTable[input[2]] << 6) +
                  (reverseLookupTable[input[3]]);

        output[0] = ((tmp >> 16) & 0xFF);
        output[1] = ((tmp >> 8)  & 0xFF);
        output[2] = ( tmp        & 0xFF);
    }

private:
    static constexpr char reverseLookupTable[128] = {
             0,   0,   0,   0,   0,   0,   0,   0, 
             0,   0,   0,   0,   0,   0,   0,   0, 
             0,   0,   0,   0,   0,   0,   0,   0, 
             0,   0,   0,   0,   0,   0,   0,   0, 
             0,   0,   0,   0,   0,   0,   0,   0, 
             0,   0,   0,  62,   0,   0,   0,  63, 
            52,  53,  54,  55,  56,  57,  58,  59, 
            60,  61,   0,   0,   0,   0,   0,   0, 
             0,   0,   1,   2,   3,   4,   5,   6, 
             7,   8,   9,  10,  11,  12,  13,  14, 
            15,  16,  17,  18,  19,  20,  21,  22, 
            23,  24,  25,   0,   0,   0,   0,   0, 
             0,  26,  27,  28,  29,  30,  31,  32, 
            33,  34,  35,  36,  37,  38,  39,  40, 
            41,  42,  43,  44,  45,  46,  47,  48, 
            49,  50,  51,   0,   0,   0,   0,   0,
        };
};
