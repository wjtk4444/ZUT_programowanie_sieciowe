#pragma once

class Base64Decoder
{
public:
    static void decode(const char (&input)[4], char (&output)[3])
    {
        // get last 6 from input[0] (bit 0 and bit 1 are always zeros)
        // append bits 2 and 3 from input[1] to the result
        output[0] = reverseLookupTable[(input[0] & 0x3F) | ((input[1] << 2) >> 6)];
        if(output[0] == 0) output[0] = '*';
        // get last 4 bits from input[1], and bits 2, 3, 4, 5 from input[2]
        output[1] = reverseLookupTable[(input[1] & 0xF) | ((input[2] << 2) >> 4)];
        if(output[2] == 0) output[1] = '*';
        
        // get last 2 bits from input[2] and last 6 bits from input[3]
        output[2] = reverseLookupTable[(input[2] & 0x3) | (input[2] & 0x3F)];
        if(output[2] == 0) output[2] = '*';
    }

private:
    static constexpr char reverseLookupTable[128] = {
             0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , 
             0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , 
             0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , 
             0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
             0 ,  0 ,  0 ,                                    //  0 - 47 [non-base64]
             
                            '+' ,                             //  43 [ + ]
    
                                 0 ,  0 ,  0 ,                // 44 - 46 [non-base64]
    
                                                '/' ,         // 47 [ / ]
    
                                                     0 ,  1 ,
             2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 ,           // 48 - 57 [0-9]
            
                                                     0 ,  0 ,                         
             0 ,  0 ,  0 ,  0 ,  0 ,                          // 58 - 64 [non-base64]
            
                                     'A', 'B', 'C', 'D', 'E', 
            'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
            'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
            'Z',                                              // 65 - 90 [A-Z] 
    
                  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,                // 91 - 96 [non-base64]
    
                                               'a', 'b', 'c',
            'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
            'x', 'y', 'z',                                    // 97 - 122 [a-z]
    
                            0 ,  0 ,  0 ,  0 ,  0 ,           // 123-127 [non-base-64]
        };
};
