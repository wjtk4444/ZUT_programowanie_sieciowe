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

void printUsageDetails(char *argv0)
{
    cout << "USAGE (1): " << argv0 << " option data" << endl;
    cout << "USAGE (2): " << argv0 << " option inputFile.ext outputFile.ext" << endl;
    cout << "AVAILABLE OPTIONS: e - encode, d - decode" << endl;
}

int main(int argc, char **argv)
{
    if((argc != 3 && argc != 4) || (argv[1][0] != 'e' && argv[1][0] != 'd'))
    {
        printUsageDetails(argv[0]);
        return 1;
    }

    bool encode = argv[1][0] == 'e';

    if(argc == 3) 
    {
        string data(argv[2]);
        
        if(encode)
            cout << base64EncodeString(data);
        else // decode
            cout << base64DecodeString(data);

        cout << endl;

        return 0;
    }
    else if(argc == 4)
    {
        ifstream inputFile(argv[2], ios::in | ios::binary);
        if(!inputFile.is_open())
        {
            cout << "Failed to open file\"" << argv[2] << "\" for reading." << endl;
            printUsageDetails(argv[0]);
            
            return 0;
        }

        ofstream outputFile(argv[2], ios::out | ios::binary);
        if(!outputFile.is_open())
        {
            cout << "Failed to open file\"" << argv[3] << "\" for writing." << endl;
            printUsageDetails(argv[0]);
            
            return 0;
        }

        inputFile.seekg(0, inputFile.end);
        int inputLength = inputFile.tellg();
        inputFile.seekg(0, inputFile.beg);
        
        if(encode)
        {
            char in[3], out[4];
            for(int i = 0; i < inputLength / 3; i++)
            {
                inputFile.read(in, 3);
                Base64Encoder::encode(in, out);
                outputFile.write(out, 4);
            }

            int padding = inputLength % 3;
            if(padding)
            {
                if(padding == 1)
                {
                    inputFile.read(in, 2);
                }
                else // padding == 2
                {
                    inputFile.read(in, 1);
                    in[1] = '=';
                }

                in[2] = '=';
            }

            Base64Encoder::encode(in, out);
            outputFile.write(out, 4);
        }
        else // decode
        {

        }

        inputFile.close();
        outputFile.close();

        cout << "Done." << endl;

        return 0;
    }
    
    return 0;
}

