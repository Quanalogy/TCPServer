#include <iostream>
#include "TCPServer.h"

using namespace std;
int main() {
    std::cout << "Hello, World!" << std::endl;

    TCPServer server;
    if(server.initServer() != 0){
        cout << "Exiting server failed" << endl;
        return -1;
    } else {
        while(1) {

        }
    }
}