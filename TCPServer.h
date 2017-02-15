//
// Created by munk on 15-02-17.
//

#ifndef CLIENT_SERVER_SOCKET_TCPSERVER_H
#define CLIENT_SERVER_SOCKET_TCPSERVER_H

#define IPSIZE 14
#define QUEUESIZE 4

#include <iostream>
#include <netdb.h>
#include <sys/socket.h>


class TCPServer {

public:
    TCPServer();
    int initServer();

private:
    int serverSocket;
    const char *IPAddr;
    const char *PortNr;
    struct addrinfo hints;
    struct addrinfo *serverinfo;

};


#endif //CLIENT_SERVER_SOCKET_TCPSERVER_H