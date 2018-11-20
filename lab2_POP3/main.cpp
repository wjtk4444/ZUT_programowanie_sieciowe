#include<iostream>
#include<string>
#include<map>
#include<fstream>

#include"TCPConnection.hpp"

using namespace std;

int main(int argc, char **argv)
{
    map<string, string> settings;
    // defaults
    settings["address" ] = "pop3.wp.pl";
    settings["port"    ] = "995"; 
    settings["username"] = "user";
    settings["password"] = "pasword";
    settings["interval"] = "5";
    settings["useSSL"  ] = "1";
    
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

    
    std::string serverResponse;
    TCPConnection POP3Server(settings["address"], atoi(settings["port"].c_str()),
        atoi(settings["useSSL"].c_str()) != 0);
    
    if(POP3Server.initializeConnection() == true)
        cout << "Connection initialized successfully." << endl;
    else
        cout << "Failed to initialize connection" << endl;
    
   
    // authentication
    if(!POP3Server.sendData(string("USER ") + settings["username"] + "\r\n"))
    {
        cout << "Failed to send USER command" << endl;
        return 1;
    }

    if(!POP3Server.receiveData(serverResponse))
    {
        cout << "No response from server." << endl;
        return 1;
    }

    if(!POP3Server.sendData(string("PASS ") + settings["password"] + "\r\n"))
    {
        cout << "Failed to send USER command" << endl;
        return 1;
    }

    if(!POP3Server.receiveData(serverResponse))
    {
        cout << "No response from server." << endl;
        return 1;
    }
    
    if(serverResponse.find("-ERR") != string::npos)
    {
        cout << serverResponse;
        return 1;
    }
    else
    {
        cout << "Authentification successful" << endl;
    }

    // get list of messages
    if(!POP3Server.sendData(string("LIST\r\n")))
    {
        cout << "Failed to send STAT command" << endl;
        return 1;
    }

    if(!POP3Server.receiveData(serverResponse))
    {
        cout << "No response from server." << endl;
        return 1;
    }
    
    cout << serverResponse;

    
    if(!POP3Server.sendData(string("QUIT\r\n")))
    {
        cout << "Failed to send QUIT command" << endl;
        return 1;
    }

    if(!POP3Server.receiveData(serverResponse))
    {
        cout << "No response from server." << endl;
        return 1;
    }
    
    if(serverResponse.find("+OK") != string::npos)
    {
        cout << serverResponse;
        return 0;
    }
    else
    {
        cout << serverResponse;
        cout << "Failed to properly close connection." << endl;
        return 1;
    }


    POP3Server.closeConnection();

    return 1;
}

