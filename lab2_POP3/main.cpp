#include<iostream>
#include<string>
#include<map>
#include<fstream>

#include"POP3Client.hpp"
#include"Message.hpp"

#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN   "\x1B[36m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"

using namespace std;

void colorPrint(string color, string message, bool newLine = true)
{
    cout << color << message << RESET << (newLine ? "\n" : "");
}

int printErrorAndExit(std::string error)
{
    colorPrint(RED, error);
    
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
    

    POP3Client mailClient(settings["address"], stoi(settings["port"]), stoi(settings["interval"]));
    if(!mailClient.authenticate(settings["username"], settings["password"]))
        return printErrorAndExit("Authentication failure.");

    colorPrint(GREEN, "Authentication successfull.");
    colorPrint(GREEN, "Fetching messages from the server...");
    for(Message message : mailClient.getMessages())
    {
        cout << "Message: " << message.getUID() << endl; 
        cout << "Title: " <<  message.getTitle() << endl; 
        cout << "Sender: " <<  message.getSender() << endl << endl; 
    }

    colorPrint(GREEN, string("Started listening for new messages. Checking every ") 
        + settings["interval"] + " seconds");
    colorPrint(CYAN, "Press q to exit the program.");

    mailClient.registerOnNewMessage([](Message message){ 
            cout << "New message: " << message.getUID() << endl; 
            cout << "Title: " <<  message.getTitle() << endl; 
            cout << "Sender: " <<  message.getSender() << endl << endl; 
        });  
   
    getchar();

    return 0;
}

