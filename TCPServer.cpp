//
// Created by munk on 15-02-17.
//

#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include "TCPServer.h"


using namespace std;
TCPServer::TCPServer() {
    cout << "Initializing the server..." << endl;
    IPAddr = "127.0.0.1";
    PortNr = "12000";

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
    buf[recv_size] = '\0';
    cout << "This is what came through: " << buf << " in a size of: " << recv_size << " bytes"
         << endl;


    if(recv_size == 3 && strstr(buf, "dir") != NULL){
        cout << "Requested to list possible files" << endl;
        char **buffer;
        getFilesOnServer(&buffer, "./", &incoming_addr, incoming_size, incoming_fd);
    } else {
        sendFile(incoming_fd, buf);
    }

}

void TCPServer::sendFile(int fd, const char *location) {
    int file = open(location, O_RDONLY);
    if(file == -1){
        cout << "Failed to open file" << strerror(errno) << endl;
    } else {
        cout << "File exist" << endl;
    }

    struct stat file_stat;
    if(fstat(file, &file_stat) < 0){
        cout << "Failed to assign stats" << endl;
    } else {
        cout << "Filesize of file to send: " << file_stat.st_size << endl;
    }
}


size_t TCPServer::getFilesOnServer(char ***buf, const char *location,
                                   const struct sockaddr *dest_addr,
                                   socklen_t dest_len, int dest_fd) {
    DIR *dp;
    struct dirent *ep;
    size_t dirElements = 0;
    dp = opendir(location);

    if(dp == NULL){
        cout << "Failed to open file directory" << endl;
        closedir(dp);
    } else {
        *buf = NULL;
        ep = readdir(dp);
        while(ep != NULL){
            ++dirElements;
            ep = readdir(dp);
        }
        rewinddir(dp);

        *buf = (char **) calloc(dirElements, sizeof(char *));

        dirElements = 0;
        ep = readdir(dp);
        while(ep != NULL) {
            (*buf)[dirElements++] = strdup(ep->d_name);
            ep = readdir(dp);
        }
        closedir(dp);

        ssize_t bytesSend = 0;
        for (int i = 0; i < sizeof(*buf); i+=1000) {
//            while(bytesSend > 0) {
                bytesSend = sendto(dest_fd, *buf[i], 1000, 0, dest_addr, dest_len);
                if(bytesSend == -1){
                    cout << "Error sending list: " << strerror(errno) << endl;
                } else {
                    cout << "Send " << bytesSend << " bytes" << endl;
                }
//            }
        }

    }
    return dirElements;
}
