#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "global_stuff.h"

#define SERVER_IP "127.0.0.1"

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return 1;
    }
    cout << "Connected to server." << endl;

    string line; 
    while (getline(cin, line)){
        line.push_back('\n'); 
        write(client_fd, line.c_str(), (int)line.size());
        memset(buffer, 0, BUFFER_SIZE);
        int num_read = read(client_fd, buffer, BUFFER_SIZE);
        if (num_read <= 0){
            cout << "Server disconnected." << endl; 
            break; 
        }
        cout << "Server: " << buffer << endl; 
    }
    close(client_fd);
    return 0;
}