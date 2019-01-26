#pragma once

#include<string>
#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<openssl/ssl.h>
#include<openssl/err.h>
#include<unistd.h>

class TCPConnection
{
public:
    TCPConnection(std::string address, int port, bool useSSL) : address(address), port(port), useSSL(useSSL)
    {
        if(useSSL)
        {
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();
            const SSL_METHOD *sslMethod = TLS_client_method();
            ctx = SSL_CTX_new(sslMethod);
            if(ctx == NULL)
                throw std::runtime_error("Failed to initialize SSL context.");
        }
    
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

        if(useSSL)
            ssl = SSL_new(ctx);

        connectionActive = false;
    }

    ~TCPConnection()
    {
        if(connectionActive)
            closeConnection();
    }

    bool initializeConnection()
    {
        if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
            return false;
        
        if(useSSL)
        {
            SSL_set_fd(ssl, sock);
            if(SSL_connect(ssl) < 0)
                return false;
        }

        connectionActive = true;
        return true;
    }

    bool closeConnection()
    {
        if(useSSL)
            SSL_CTX_free(ctx);

        if(close(sock) < 0)
            return false;

        connectionActive = false;
        return true;
    }
        
    bool receiveData(std::string &result, std::string end, int bufferSize = 65534)
    {
        char *buffer = new char[bufferSize + 1];
        memset(buffer, 0, bufferSize);

        result = "";
        while(result.find(end) == std::string::npos)
        {
            if((useSSL ? SSL_read(ssl, buffer, bufferSize) : read(sock, buffer, bufferSize)) < 0)
            {
                delete[] buffer;
                return false;
            }
            
            result += buffer;
        }

        delete[] buffer;
    
        //std::cout << "RECV: " << result << std::endl;
        return true;
    }

    bool sendData(std::string data)
    {  
        //std::cout << "SEND: " << data << std::endl;

        if((useSSL ? SSL_write(ssl, data.c_str(), data.length()) :
            write(sock, data.c_str(), data.length())) < 0)
            return false;
            
        return true;
    }

    bool isActive()
    {
        return connectionActive;
    }

private:
    SSL_CTX *ctx;
    SSL *ssl;

    bool connectionActive;
    bool useSSL;
    int sock;
    int port;
    std::string address;
    struct sockaddr_in server;
};
