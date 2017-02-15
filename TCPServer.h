//
// Created by munk on 15-02-17.
//

#ifndef CLIENT_SERVER_SOCKET_TCPSERVER_H
#define CLIENT_SERVER_SOCKET_TCPSERVER_H

#define IPSIZE 14
#define QUEUESIZE 4

class TCPServer {

public:
    TCPServer();
    int initServer();

private:
    int serverSocket;
    char ip[IPSIZE] = {'1','0','.','0','.','0','.','1'};
};


#endif //CLIENT_SERVER_SOCKET_TCPSERVER_H