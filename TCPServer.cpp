//
// Created by munk on 15-02-17.
//

#include <cstring>
//#include <zconf.h>
#include "TCPServer.h"


using namespace std;
TCPServer::TCPServer() {
    cout << "Initializing the server..." << endl;
    //IPAddr = "127.0.0.1";
    //PortNr = "12000";
    IPAddr = "10.0.0.1";
    PortNr = "9000";

    // Allocate space for hints, which holds the connection information
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP
}

int TCPServer::initServer() {
    int status;

    // setup the ipadress and port number in a readable way for the socket
    if((status = getaddrinfo(IPAddr, PortNr, &hints, &serverinfo)) != 0) {
        cout << "Problems assigning the server to the ip" << endl << gai_strerror(status) << endl;
        return status;
    }

    // Setup the socket that handles incoming requests
    serverSocket = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);

    if(serverSocket <= 0){
        cout << "Failed to initialize the server!" << endl;
        return serverSocket;
    } else {
        cout << "The server has been initialized" << endl;
    }


    //SO_REUSEADDR;
    // Bind the socket, hence telling it to which address it belongs
    int error = bind(serverSocket, serverinfo->ai_addr, serverinfo->ai_addrlen);

    if(error == -1){
        cout << "An error has occurred while binding the socket on server side" << endl << error << endl;
        return error;
    } else {
        cout << "Successfully bound the server to the socket" << endl;
    }

    // start listening with QUEUESIZE of possible connections
    error = listen(serverSocket, QUEUESIZE);

    if(error == -1) {
        cout << "Failed to listen on server..." << endl;
        return error;
    } else {
        cout << "Successfully setup server..." << "Scotty, Beam me up!" << endl;
    }

    return error;
}

void TCPServer::acceptConnection() {

    // Accept incoming connection

    // The socket for incoming connection
    struct sockaddr incoming_addr;
    // Size of it
    socklen_t incoming_size = sizeof(incoming_addr);
    // Get a filedescriptor for the incoming connection
    int incoming_fd = accept(serverSocket, &incoming_addr, &incoming_size);

    if(incoming_fd <= 0){
        cout << "Rejected an incoming connection" << endl;
    } else {
        cout << "Accepted an incoming connection" << endl;
    }

    // Make a buffer that handles chunks of 1000
    size_t buffersize = 1000;
    char buf[buffersize] = {0};

    // Receive the message from the client
    ssize_t recv_size;
    recv_size = recvfrom(incoming_fd, buf, buffersize, 0,
                         &incoming_addr, &incoming_size);
    cout << "This is what came through: " << buf << " in a size of: " << recv_size << " bytes"
         << endl;

}
