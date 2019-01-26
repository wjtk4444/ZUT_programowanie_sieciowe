#pragma once

#include<string>

class Message
{
public:
    Message(std::string uid, std::string title, std::string sender) : uid(uid), title(title), sender(sender) { }

    bool operator < (const Message &message) const
    {
        return uid.compare(message.uid) < 0;
    }

    std::string getTitle()
    {
        return title;
    }

    std::string getSender()
    {   
        return sender;
    }

    std::string getUID()
    {
        return uid;
    }

private:
    std::string title;
    std::string uid;
    std::string sender;
};
