#include<iostream>
#include<string>
#include<map>
#include<fstream>

#include"TCPConnection.hpp"

#define SUFFIX  "\r\n"

#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYANN   "\x1B[36m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"

using namespace std;

void colorPrint(string color, string message, bool newLine = true)
{
    cout << color << message << (newLine ? "\n" : "") << RESET;
}

void prettyPrintServerResponse(string message)
{
    colorPrint(message.find("-ERR") == string::npos ? BLUE : RED,
        "Server response: " + message, message[message.length() - 1] != '\n');
}

void prettyPrintSentCommand(string command)
{
    colorPrint(GREEN, "Command sent: " + command, command[command.length() - 1] != '\n');
}

void prettyPrintFailedCommand(string command)
{
    colorPrint(RED, "Failed to send command: " + command, command[command.length() - 1] != '\n');
}

bool sendCommandAndPrintResponse(TCPConnection *connection, string command, string &response)
{
    if(!connection->sendData(command))
    {
        prettyPrintFailedCommand(command);
        return false;
    }
    prettyPrintSentCommand(command);

    if(!connection->receiveData(response))
    {
        colorPrint(RED, "No response from server");
        return false;
    }
    prettyPrintServerResponse(response); 
    
    return true;
}

bool authenticate(TCPConnection *POP3Server, string username, string password)
{
    string serverResponse;
    
    // wp.pl hack
    if(!POP3Server->receiveData(serverResponse))
    {
        colorPrint(RED, "No response from server");
        return false;
    }
    prettyPrintServerResponse(serverResponse); 
   
    // authentication process
    // USER command
    if(!sendCommandAndPrintResponse(POP3Server, string("USER ") + username + SUFFIX, serverResponse))
        return false;
    // PASS command
    if(!sendCommandAndPrintResponse(POP3Server, string("PASS ") + password + SUFFIX, serverResponse))
        return false;
    
    // "parsing" server response
    if(serverResponse.find("-ERR") != string::npos)
    {
        colorPrint(RED, "Authentifiction failed");
        return false;
    }

    colorPrint(GREEN, "Authentification successful");
    
    return true;
}

bool closeConnectionServerside(TCPConnection *POP3Server)
{
    string serverResponse;
    
    // QUIT command
    if(!sendCommandAndPrintResponse(POP3Server, string("PASS") + SUFFIX, serverResponse))
  
    // "parsing" server response
    if(serverResponse.find("-ERR") != string::npos)
    {
        colorPrint(RED, "Failed to properly close connection");
        return false;
    }

    colorPrint(GREEN, "Connection closed: " + serverResponse);

    return true;
}

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
        colorPrint(RED, "App.config could not be opened for reading");
        return 1;
    }

    // read pairs from config file
    string name, value;
    while(configFile >> name && configFile >> value)
        if(settings.find(name) != settings.end())
            settings[name] = value;

    
    // create new connection
    TCPConnection POP3Server(settings["address"], atoi(settings["port"].c_str()),
        atoi(settings["useSSL"].c_str()) != 0);
   
    // initialize connection
    if(POP3Server.initializeConnection() == true)
        colorPrint(GREEN, "Connection initialized successfully");
    else
        colorPrint(RED, "failed to initialize connection");
    
    // authenticate on server
    if(!authenticate(&POP3Server, settings["username"], settings["password"]))
    {
        closeConnectionServerside(&POP3Server);
        POP3Server.closeConnection();
        return 1;
    }

    string serverResponse;
    // get list of messages
    if(!POP3Server.sendData(string("LIST") + SUFFIX))
        colorPrint(RED, "Failed to send STAT command");

    if(!POP3Server.receiveData(serverResponse))
        colorPrint(RED, "No response from server");

    prettyPrintServerResponse(serverResponse); 

    closeConnectionServerside(&POP3Server);
    POP3Server.closeConnection();

    return 0;
}

