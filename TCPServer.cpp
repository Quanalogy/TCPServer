//
// Created by munk on 15-02-17.
//

#include <cstring>
#include "TCPServer.h"


using namespace std;
TCPServer::TCPServer() {
    cout << "Initializing the server..." << endl;
    IPAddr = "127.0.0.1";
    PortNr = "12000";
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP
//    hints.ai_flags = AI_PASSIVE;
}

int TCPServer::initServer() {
    int status;

    if((status = getaddrinfo(IPAddr, PortNr, &hints, &serverinfo)) != 0) {
        cout << "Problems assigning the server to the ip" << endl << gai_strerror(status) << endl;
        return status;
    }

    serverSocket = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);

    if(serverSocket <= 0){
        cout << "Failed to initialize the server!" << endl;
        return serverSocket;
    } else {
        cout << "The server has been initialized" << endl;
    }

    //const sockaddr tcpAddress = {AF_INET, IPAddr};

    int error = bind(serverSocket, serverinfo->ai_addr, serverinfo->ai_addrlen);

    if(error == -1){
        cout << "An error occured when binding the socket on server side" << endl << error << endl;
        return error;
    } else {
        cout << "Successfully binded the server to the socket" << endl;
    }

    // start listening
    error = listen(serverSocket, QUEUESIZE);

    if(error == -1) {
        cout << "Failed to listen on server..." << endl;
    } else {
        cout << "Successfully setup server..." << "Beam me up Scotty!" << endl;
    }

    return error;
}