//
// Created by munk on 15-02-17.
//

#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
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
        cout << "An error has occurred while binding the socket on server side: "
             << endl << strerror(errno) << endl;
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
        sendFile(incoming_fd, &incoming_addr, incoming_size, buf);
    }

}

size_t TCPServer::sendFile(int dest_fd, const struct sockaddr *dest_addr,
                           socklen_t dest_len, const char location[]) {
    int file = open(location, O_RDONLY);

    if(file == -1){
        cout << "Failed to open file" << strerror(errno) << endl;
        char error[] = {'4','0','4'};
        return (size_t) sendto(dest_fd, error, strlen(error), 0, dest_addr, dest_len);
    } else {
        cout << "File exist" << endl;
    }

    struct stat file_stat;
    if(fstat(file, &file_stat) < 0){
        cout << "Failed to assign stats" << endl;
    } else {
        cout << "Filesize of file to send: " << file_stat.st_size << endl;
    }

    if((file_stat.st_mode & S_IFMT) == S_IFDIR){    // Requested a directory, this is not allowed
        cout << "Requested file is a directory!" << endl;
        char error[] = {'4', '0', '0'};     // Bad request
        return (size_t) sendto(dest_fd, error, strlen(error), 0, dest_addr, dest_len);
    }

    // Send the size of file to expect
    char filesize[256];
    sprintf(filesize, "%d", file_stat.st_size);
    ssize_t bytesSend = sendto(dest_fd, filesize, sizeof(filesize), 0, dest_addr, dest_len);
    if(bytesSend == -1){
        cout << "Error sending number of bytes, with error: " << strerror(errno) << endl;
    }

    // Send the file
    size_t bytesToSend = file_stat.st_size;
    off_t offset = 0;
    size_t sendBuf = 1000;
    while(bytesToSend > 0){
        if(bytesToSend < 1000){
            sendBuf =(size_t) bytesToSend;
        }
        bytesSend = sendfile(dest_fd, file, &offset, sendBuf);
        cout << "offset for file to send is: " << offset << endl;
        cout << "Bytes send: " << bytesSend << endl;
        if(bytesSend == -1){
            cout << "Error while sending the file, with error: " << strerror(errno) << endl;
            return (size_t) shutdown(dest_fd, 2); // force shutdown the connection
        }

        bytesToSend -= bytesSend;

    }
    return bytesToSend;
}


size_t TCPServer::getFilesOnServer(char ***buf, const char *location,
                                   const struct sockaddr *dest_addr,
                                   socklen_t dest_len, int dest_fd) {
    DIR *dp;
    struct dirent *ep;          // handler of dir
    size_t dirElements = 0;     // Elements in dir
    dp = opendir(location);     // Handler of current element in dir


    if(dp == NULL){             // Is the dir open?
        cout << "Failed to open file directory" << endl;
        closedir(dp);
        return 0;
    } else {                    // It is
        *buf = NULL;
        ep = readdir(dp);
        while(ep != NULL){      // Count elements in dir
            ++dirElements;
            ep = readdir(dp);
        }
        rewinddir(dp);          // Start from the beginning of the dir

        *buf = (char **) calloc(dirElements, sizeof(char *));   // Set size to fit the elements

        dirElements = 0;
        ep = readdir(dp);
        while(ep != NULL) {     // Set the name of each element in the dir to the buffer
            (*buf)[dirElements++] = strdup(ep->d_name);
            ep = readdir(dp);
        }
        closedir(dp);           // Cleanup

        // Send to the client how many elements to expect
        char dirNum[sizeof(size_t)];
        snprintf(dirNum, sizeof(dirNum), "%zu", dirElements);   // Convert to format that can be send

        ssize_t bytesSend = 0;
        bytesSend = sendto(dest_fd, &dirNum[0], dirElements, 0, dest_addr, dest_len);   // Send how many elements to expect

        cout << "Dir elements to send: " << dirNum << endl;

        if(bytesSend == -1) {
            cout << "Error sending number of dir elements, quitting with error: "
                 << strerror(errno) << endl;
            return 0;
        }

        cout << "Elements to send:" << endl;
        for (int j = 0; j < dirElements; ++j){
            cout << (*buf)[j] << endl;
        }

        for (int i = 0; i < dirElements; i++) {         // Send all the elements, one by one
            size_t sendLen = strlen((*buf)[i]);         // Length of element to send

            bytesSend = sendto(dest_fd, &sendLen, sizeof(sendLen), 0, dest_addr, dest_len); // Send size of element to expect
            if(bytesSend == -1){
                cout << "Error sending list, quitting with error: " << strerror(errno) << endl;
                return 0;
            } else {
                cout << "Send " << bytesSend << " bytes" << endl;
            }

            bytesSend = sendto(dest_fd, (*buf)[i], sendLen, 0, dest_addr, dest_len);        // Send the element
            if(bytesSend == -1){
                cout << "Error sending list, quitting with error: " << strerror(errno) << endl;
                return 0;
            } else {
                cout << "Send " << bytesSend << " bytes" << endl;
            }
        }
        // Force shutdown, hence sending the 0 byte to terminate the while on client
        if(shutdown(dest_fd, 2) == -1) {
            cout << "Failed closing socket, with error" << strerror(errno) << endl;
        }

    }

    return dirElements;
}
