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

// print client-server communication
#define DEBUG_MSG true
#define DEBUG     string("[DEBUG]: ")
using namespace std;

void colorPrint(string color, string message, bool newLine = true)
{
    cout << color << message << RESET << (newLine ? "\n" : "");
}

void prettyPrintServerResponse(string message)
{
    if(!DEBUG_MSG) return;
    colorPrint(message.find("-ERR") == string::npos ? BLUE : RED,
        DEBUG + "Server response: " + message, message[message.length() - 1] != '\n');
}

void prettyPrintSentCommand(string command)
{
    if(!DEBUG_MSG) return;
    colorPrint(GREEN, DEBUG + "Command sent: " + command, command[command.length() - 1] != '\n');
}

void prettyPrintFailedCommand(string command)
{
    if(!DEBUG_MSG) return;
    colorPrint(RED, DEBUG + "Failed to send command: " + command, command[command.length() - 1] != '\n');
}

void prettyPrintNoResponse()
{
    if(!DEBUG_MSG) return;
    colorPrint(RED, DEBUG + "No response from server");
}
bool sendCommand(TCPConnection *connection, string command)
{
    if(!connection->sendData(command))
    {
        prettyPrintFailedCommand(command);
        return false;
    }
    
    prettyPrintSentCommand(command);
    return true;
}

bool printResponse(TCPConnection *connection, string &response)
{
    if(!connection->receiveData(response))
    {
        prettyPrintNoResponse(); 
        return false;
    }
    
    prettyPrintServerResponse(response); 
    return true;
}

bool authenticate(TCPConnection *POP3Server, string username, string password)
{
    colorPrint(WHITE, "Authenticating on server...");
    
    string command, serverResponse;

    if(!printResponse(POP3Server, serverResponse))
        return false;

    // authentication process
    // USER command
    command = string("USER ") + username + SUFFIX;
    if(!sendCommand(POP3Server, command))
        return false;
    
    if(!printResponse(POP3Server, serverResponse))
        return false;
    
    // PASS command
    command = string("PASS ") + password + SUFFIX;
    sendCommand(POP3Server, command);
    
    if(!printResponse(POP3Server, serverResponse))
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
    // QUIT command
    // fail on:
    // - sending failed
    // - getting response failed
    // - error in response
    string serverResponse;
    if( (!sendCommand(POP3Server, string("QUIT") + SUFFIX)) ||
        (!printResponse(POP3Server, serverResponse)) ||
        (serverResponse.find("-ERR") != string::npos))
        {    
            colorPrint(RED, "Failed to properly close connection (serverside)");
            return false;
        }

    colorPrint(GREEN, "Connection closed successfully (serverside)");

    return true;
}

int closeConnections(TCPConnection *POP3Server, string reason)
{
    colorPrint(reason.find("error") == string::npos ? WHITE : RED, "Closing connections due to " + reason);
    
    closeConnectionServerside(POP3Server);
    if(POP3Server->closeConnection())
        colorPrint(GREEN, "Successfully closed connection (clientside)");
    else
        colorPrint(RED, "Failed to properly close connection (clientside)");
    
    return 1;
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
   
    colorPrint(WHITE, "Initalizing connection...");

    // initialize connection
    if(!POP3Server.initializeConnection())
        return closeConnections(&POP3Server, "error while initalizing connection");

    colorPrint(GREEN, "Connection initialized successfully");
    
    // authenticate on server
    if(!authenticate(&POP3Server, settings["username"], settings["password"]))
        return closeConnections(&POP3Server, "authentication error");
    
    string serverResponse;
    
    // get message list
    colorPrint(WHITE, "Obtaining message list...");
    if(!sendCommand(&POP3Server, string("LIST") + SUFFIX) ||
       !printResponse(&POP3Server, serverResponse))
        return closeConnections(&POP3Server, "error getting message list");

    closeConnections(&POP3Server, "application exit.");
    
    return 0;
}

