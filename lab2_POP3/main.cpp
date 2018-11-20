#include<iostream>
#include<string>
#include<map>
#include<fstream>

using namespace std;

int main(int argc, char **argv)
{
    map<string, string> settings;
    // defaults
    settings["address"  ] = "pop3.wp.pl";
    settings["port"    ] = "995"; 
    settings["username"] = "user";
    settings["password"] = "pasword";
    settings["interval"] = "5";
    
    ifstream configFile("App.config");
    if(!configFile.is_open())
    {
        cout << "App.config" << " could not be opened for reading." << endl;
        return 1;
    }

    // read pairs from config file
    string name, value;
    while(configFile >> name && configFile >> value)
        if(settings.find(name) != settings.end())
            settings[name] = value;

    // print config (debug)
    //for(auto const& pair : settings)
    //    cout << pair.first << " = " << pair.second << endl;

    return 0;
}

