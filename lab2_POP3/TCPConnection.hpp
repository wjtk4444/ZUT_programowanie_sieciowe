#include<string>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>

class TCPConnection
{
public:
    TCPConnection(std::string address, int port) : address(address), port(port)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1)
            throw std::runtime_error("Could not create socket.");

        in_addr_t ipAddress = inet_addr(address.c_str());
        if(ipAddress == INADDR_NONE) // requires name resolution
        {
            struct hostent *hostEntity;  
            struct in_addr **addr_list;

            if((hostEntity = gethostbyname(address.c_str())) == NULL)
                throw std::runtime_error("Could not resolve hostname.");
            
            addr_list = (struct in_addr**) hostEntity->h_addr_list;
            server.sin_addr = *addr_list[0]; //always 0?
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(port);
    }

    bool initializeConnection()
    {
        if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
            return false;

        return true;
    }
        
    bool receiveData(std::string &result, int bufferSize = 1024)
    {
        char *buffer = new char[bufferSize];
        
        if(recv(sock, buffer, sizeof(buffer), 0) < 0)
        {
            delete[] buffer;
            return false;
        }

        result = buffer;
        delete[] buffer;
    
        return true;
    }

    bool sendData(std::string data)
    {
        if(send(sock, data.c_str(), data.length() + 1, 0) < 0) // +1 because of the \0 character at the end
            return false;
        
        return true;
    }













private:
    int sock;
    int port;
    std::string address;
    struct sockaddr_in server;
};
