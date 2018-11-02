#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include"Base64Encoder.hpp"
#include"Base64Decoder.hpp"

using namespace std;
void printUsageDetails(char *argv0)
{
    cout << "Simple base64 encoder / decoder with padding support." << endl;
    cout << "Files / strings with length % 4 will be complemented by zeros." << endl;
    cout << "USAGE (1): " << argv0 << " OPTION stringData" << endl;
    cout << "USAGE (2): " << argv0 << " OPTION inputFile.ext outputFile.ext" << endl;
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
        stringstream input(argv[2]);

        if(encode)
            Base64Encoder::encodeStream(input, cout);
        else // decode
            Base64Decoder::decodeStream(input, cout);
        
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

        ofstream outputFile(argv[3], ios::out | ios::binary);
        if(!outputFile.is_open())
        {
            cout << "Failed to open file\"" << argv[3] << "\" for writing." << endl;
            printUsageDetails(argv[0]);
            
            return 0;
        }

        if(encode)
            Base64Encoder::encodeStream(inputFile, outputFile);
        else // decode
            Base64Decoder::decodeStream(inputFile, outputFile);

        inputFile.close();
        outputFile.close();

        cout << "Done." << endl;
    }

    return 0;
}

