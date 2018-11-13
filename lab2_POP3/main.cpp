#include<iostream>
#include<string>
#include<map>
#include<fstream>

int main(int argc, char **argv)
{
    std::map<std::string, std::string> settings;
    // defaults
    settings["adress"  ] = "pop3.wp.pl";
    settings["port"    ] = "995"; 
    settings["username"] = "user";
    settings["password"] = "pasword";
    settings["interval"] = "5";
    
    std::ifstream configFile("App.config");
    if(!configFile.is_open())
    {
        std::cout << "App.config" << " could not be opened for reading." << std::endl;
        return 1;
    }

    // read pairs from config file
    std::string name, value;
    while(configFile >> name && configFile >> value)
        if(settings.find(name) != settings.end())
            settings[name] = value;

    // print config (debug)
    //for(auto const& pair : settings)
    //    std::cout << pair.first << " = " << pair.second << std::endl;

    return 0;
}

