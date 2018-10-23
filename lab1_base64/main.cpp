#include<iostream>
#include<fstream>

#include"Base64Encoder.cpp"

using namespace std;

int main(int argc, char **argv)
{
    if(0 && argc != 4)
    {
        cout << "USAGE: " << argv[0] << " encode/decode in.ext out.ext" << endl;
        return 0;
    }

    char in[] = {'M', 'a','n'}, out[4];
    Base64Encoder::encode(in, out);

    for(int i = 0; i < 4; i++)
        cout << out[i];

    cout << endl;

    return 0;
}

