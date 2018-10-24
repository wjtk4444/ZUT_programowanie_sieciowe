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

string base64EncodeString(string data)
{
    char in[3], out[4];
    string base64;
    base64.reserve((data.length() * 4 / 3) & ~3);
   
    unsigned int i;
    for(i = 0; i + 2 < data.length(); i+=3)
    {
        in[0] = data[i];
        in[1] = data[i + 1];
        in[2] = data[i + 2];

        Base64Encoder::encode(in, out); 
        base64 += out;
    }


    if(i == data.length())
        return base64;

    int padding = 1;
    
    if (i + 1 == data.length())
    {
        in[1] = 0;
        padding = 2;
    }
    else if(i + 2 == data.length())
    {
        in[1] = data[i+1];
    }

    in[0] = data[i];
    in[2] = 0;

    Base64Encoder::encode(in, out, padding); 
    base64 += out;

    return base64;
}

string base64DecodeString(string base64)
{
    if(base64.length() % 4)
        return string("");

    char in[4], out[3];
    string data;
    data.reserve(base64.length() * 3 / 4);

    unsigned int i;
    for( i = 0; i + 3 + 4 < base64.length(); i+=4)
    {
        in[0] = base64[i];
        in[1] = base64[i + 1];
        in[2] = base64[i + 2];
        in[3] = base64[i + 3];

        Base64Decoder::decode(in, out);
        data += out;
    }

    in[0] = base64[i];
    in[1] = base64[i + 1];

    int padding = 0 + (in[2] == '=' ? 1 : 0) + (in[3] == '=' ? 1 : 0); 

    in[2] = padding > 1 ? 0 : base64[i + 2];
    in[3] = padding > 0 ? 0 : base64[i + 3];

    Base64Decoder::decode(in, out);
    
    data += out[0];
    if(padding > 1)
        data += out[1];
    if(padding > 0)
        data += out[2];
    
    return data;
}

int main(int argc, char **argv)
{
    if(0 && argc != 4)
    {
        cout << "USAGE: " << argv[0] << " encode/decode in.ext out.ext" << endl;
        return 0;
    }

    string s = string("any carnal pleasure");
    string b64 = base64EncodeString(s);
    string d = base64DecodeString(b64);

    cout << "s  : " << s << endl;
    cout << "b64: " << b64 << endl;
    cout << "d  : " << s << endl;

    return 0;
}

