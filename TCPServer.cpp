//
// Created by munk on 15-02-17.
//

#include <sys/socket.h>
#include "TCPServer.h"
#include <iostream>

using namespace std;
TCPServer::TCPServer() {
    cout << "Initializing the server..." << endl;

}

int TCPServer::initServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket <= 0){
        cout << "Failed to initialize the server!" << endl;
        return serverSocket;
    } else {
        cout << "The server has been initialized" << endl;
    }

    const sockaddr tcpAddress = {AF_INET, {*ip}};

    int error = bind(serverSocket, &tcpAddress, sizeof(tcpAddress));

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