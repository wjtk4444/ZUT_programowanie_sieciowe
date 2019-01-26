#pragma once

#include<iostream>

#include<string>
#include<sstream>
#include<set>
#include<chrono>
#include<thread>
#include<mutex>
#include<functional>
#include<regex>

#include"TCPConnection.hpp"
#include"Message.hpp"

#define SUFFIX  "\r\n"
#define MSG_END  "\r\n"

enum POP3_ERROR {
    NONE = 0, INIT_CONNECTION_ERROR = 1, CLOSE_CONNECTION_ERROR = 2, SERVER_ERROR = 3, AUTHENTICATION_FAILED = 4
};

class POP3Client 
{
public:
    POP3Client(std::string address, int port, int refreshTime = 10) : address(address), port(port),
        refreshTime(refreshTime), backgroundService(&POP3Client::backgroundLoop, this)
    {
        isAuthenticated = false;
        onNewMessage = nullptr;
        keepRunning = true;
    }

    bool authenticate(std::string username, std::string password)
    {
        this->username = username;
        this->password = password;
        
        std::string response;
        POP3_ERROR err = sendCommand("", response, true);
        if(err == NONE || err == CLOSE_CONNECTION_ERROR)
        {   
            isAuthenticated = true;
            updateMessages();
            return true;
        }

        return false;
    }

    std::set<Message> getMessages()
    {
        return messages;
    }
    
    void registerOnNewMessage(std::function<void(Message)> callback)
    {
        onNewMessage = callback;
    }

    void unregisterOnNewMessage()
    {
        onNewMessage = nullptr;
    }
    
    ~POP3Client()
    {
        keepRunning = false;
        backgroundService.join();
    }

private:
    std::string getMessageList()
    {       
        if(!isAuthenticated)
            throw std::runtime_error("User not authenticated");

        std::string command, response;
        command = std::string("LIST") + SUFFIX;
        POP3_ERROR err = sendCommand(command, response);
        if(err != NONE && err != CLOSE_CONNECTION_ERROR)
            throw std::runtime_error("Something happened (getMessageList)");

        return response;
    }

    Message getMessage(std::string messageNo, std::string uid)
    {
        std::string response;
        POP3_ERROR err = sendCommand(std::string("RETR ") + messageNo + SUFFIX, response);
        if(err != NONE && err != CLOSE_CONNECTION_ERROR)
        {
            std::cout << "Error: " << err << std::endl;
            throw std::runtime_error("Something happened (getMesage)");
        }
        std::regex titleRegex("Subject: (.*)"); 
        std::regex senderRegex("From: (.*)"); 

        std::string line, title = "", sender = "";
        std::stringstream ss(response);
        while(std::getline(ss, line))
        {
            if(title == "")
            {
                std::smatch titleMatch;
                std::regex_search(line, titleMatch, titleRegex);
                if(titleMatch.size() == 2)
                    title = titleMatch[1];
            }
            if(sender == "")
            {
                std::smatch senderMatch;
                std::regex_search(line, senderMatch, senderRegex);
                if(senderMatch.size() == 2)
                    sender = senderMatch[1];
            }

            if(title != "" && sender != "")
                break;
        }

        return Message(uid, title, sender);
    }

    std::set<Message> updateMessages()
    {
        std::regex messageRegex("([0-9]+) ([0-9]+)");
        std::stringstream ss(getMessageList());
        std::string line;
        std::set<Message> newMessages;

        messagesMutex.lock();
        while(std::getline(ss, line))
        {
            std::smatch match;
            std::regex_search(line, match, messageRegex);
            if(match.size() == 3)
            {
                std::string messageNo = match[1];
                std::string uid = match[2];
                if(uids.find(uid) == uids.end())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    uids.insert(uid);
                    Message message = getMessage(messageNo, uid);
                    messages.insert(message);
                    newMessages.insert(message);
                }
            }
        }
        messagesMutex.unlock();

        return newMessages;
    }

    void backgroundLoop()
    {
        while(keepRunning)
        {
            for(int i = 0; i < refreshTime * 10; i++)
            {
                if(!keepRunning)
                    break;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
    
            if(!keepRunning)
                break;

            std::set<Message> newMessages = updateMessages();

            if(!keepRunning)
                break;
            
            if(onNewMessage && !newMessages.empty())
            {
                for(auto message : newMessages)
                    onNewMessage(message);
            }
        }
    }

    POP3_ERROR sendCommand(std::string command, std::string &response, bool authOnly = false)
    {
        // open new tcp connection
        TCPConnection mailServer(address, port, port == 995);

        // try connecting
        if (!mailServer.initializeConnection())
            return POP3_ERROR::INIT_CONNECTION_ERROR;
        
        std::string serverCommand, serverResponse;
        // read the "hello" message from server
        if(!mailServer.receiveData(serverResponse, MSG_END))
            return POP3_ERROR::SERVER_ERROR;

        // authentication process
        // USER serverCommand
        serverCommand = std::string("USER ") + username + SUFFIX;
        if(!mailServer.sendData(serverCommand))
            return POP3_ERROR::SERVER_ERROR;
       
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(!mailServer.receiveData(serverResponse, MSG_END))
            return POP3_ERROR::SERVER_ERROR;
        
        // PASS serverCommand
        serverCommand = std::string("PASS ") + password + SUFFIX;
        if(!mailServer.sendData(serverCommand))
            return POP3_ERROR::SERVER_ERROR;
       
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(!mailServer.receiveData(serverResponse, MSG_END))
            return POP3_ERROR::SERVER_ERROR;
        
        // "parsing" server serverResponse
        if(serverResponse.find("-ERR") != std::string::npos)
            return POP3_ERROR::AUTHENTICATION_FAILED;

        if(!authOnly)
        {
            if(!mailServer.sendData(command))
                return POP3_ERROR::SERVER_ERROR;

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if(!mailServer.receiveData(serverResponse, MSG_END))
                return POP3_ERROR::SERVER_ERROR;

            response = serverResponse;
        }

        serverCommand = std::string("QUIT") + SUFFIX;
        if(!mailServer.sendData(serverCommand))
            return POP3_ERROR::SERVER_ERROR;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(!mailServer.receiveData(serverResponse, MSG_END))
            return POP3_ERROR::SERVER_ERROR;
        
        if(serverResponse.find("-ERR") != std::string::npos)
            return POP3_ERROR::CLOSE_CONNECTION_ERROR;
        
        if(mailServer.closeConnection())
            return POP3_ERROR::CLOSE_CONNECTION_ERROR;

        return POP3_ERROR::NONE;
    }

    bool isAuthenticated;
    bool keepRunning;
    
    int port, refreshTime;
    std::string address, username, password;
    
    std::set<Message> messages;
    std::set<std::string> uids;
    std::mutex messagesMutex;
    
    std::function<void(Message)> onNewMessage;
    std::thread backgroundService;
};
