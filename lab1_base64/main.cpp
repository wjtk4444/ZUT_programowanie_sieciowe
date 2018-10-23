#include<iostream>
#include<fstream>

#include"Base64Encoder.hpp"

using namespace std;

void printBits(char m)
{
    for(int i = 7; i >= 0; i--)
        cout << ((m >> i) & 1);

    cout << endl;
}


int main(int argc, char **argv)
{
    if(0 && argc != 4)
    {
        cout << "USAGE: " << argv[0] << " encode/decode in.ext out.ext" << endl;
        return 0;
    }

    char test[] = "any carnal pleasur";
    char input[3], output[4];
    int i = 0, size = sizeof(test) / sizeof(char) - 1;

    for(int i = 0; i < size; i++)
        cout << test[i];

    cout << endl;
    
        int padding = 0;
    for(int i = 0; i < size; i+=3)
    {

        input[0] = test[i];
        
        if(i + 2 == size){
            input[2] = 0;
            padding = 1;
        }
        else
            input[2] = test[i + 2];


        if(i + 1 >= size){
            input[1] = 0;
            padding = 2;
        }
        else
            input[1] = test[i + 1];

        Base64Encoder::encode(input, output, padding);
        
        for(int j = 0; j < 4; j++)
            cout << output[j];
    }

    cout << endl;

    return 0;
}

