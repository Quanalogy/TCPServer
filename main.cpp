#include <iostream>
#include <zconf.h>
#include "TCPServer.h"

using namespace std;
int main() {
    TCPServer server;

    if(server.initServer() != 0){
        cout << "Exiting server failed" << endl;
        return -1;
    } else {
        while(1) {
            server.acceptConnection();
        }
    }
}