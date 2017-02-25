//
// Created by munk on 15-02-17.
//

#ifndef CLIENT_SERVER_SOCKET_TCPSERVER_H
#define CLIENT_SERVER_SOCKET_TCPSERVER_H

#define QUEUESIZE 4

#include <iostream>
#include <netdb.h>
#include <sys/socket.h>


class TCPServer {

public:
    /*!
     * Setup constants
     */
    TCPServer();

    /*!
     * Setup the server
     * @return 0 on success or the standard error (-1)
     */
    int initServer();

    /*!
     * Run in while loop, accept connections and handle the request
     */
    void acceptConnection();


private:
    int serverSocket;
    const char *IPAddr;
    const char *PortNr;
    struct addrinfo hints;
    struct addrinfo *serverinfo;

};


#endif //CLIENT_SERVER_SOCKET_TCPSERVER_H