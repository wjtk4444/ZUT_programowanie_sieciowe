#include<iostream>
#include<fstream>
#include<string>

#include"Base64Encoder.hpp"
#include"Base64Decoder.hpp"

using namespace std;

void printBits(char m)
{
    for(int i = 7; i >= 0; i--)
        cout << ((m >> i) & 1);

    cout << endl;
}

string base64EncodeString(string input)
{
    char in[3], out[4];
    string base64;
    base64.reserve((input.length() * 4 / 3) & ~3);
   
    int i;
    for(i = 0; i + 2 < input.length(); i+=3)
    {
        in[0] = input[i];
        in[1] = input[i + 1];
        in[2] = input[i + 2];

        Base64Encoder::encode(in, out); 
        base64 += out;
    }


    if(i == input.length())
        return base64;

    int padding = 1;
    
    if (i + 1 == input.length())
    {
        in[1] = 0;
        padding = 2;
    }
    else if(i + 2 == input.length())
    {
        in[1] = input[i+1];
    }

    in[0] = input[i];
    in[2] = 0;

    Base64Encoder::encode(in, out, padding); 
    base64 += out;

    return base64;
}

int main(int argc, char **argv)
{
    if(0 && argc != 4)
    {
        cout << "USAGE: " << argv[0] << " encode/decode in.ext out.ext" << endl;
        return 0;
    }

    string s = string("any carnal pleasure.");
    string b64 = base64EncodeString(s);

    cout << "s  : " << s << endl;
    cout << "b64: " << b64 << endl;


    return 0;
}

