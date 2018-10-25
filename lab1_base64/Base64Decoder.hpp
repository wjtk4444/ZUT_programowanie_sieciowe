#pragma once

class Base64Decoder
{
public:
    static void decode(const char (&input)[4], char (&output)[3])
    {
        int tmp = (reverseLookupTable[(int)input[0]] << 18) +
                  (reverseLookupTable[(int)input[1]] << 12) +
                  (reverseLookupTable[(int)input[2]] << 6) +
                  (reverseLookupTable[(int)input[3]]);

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
