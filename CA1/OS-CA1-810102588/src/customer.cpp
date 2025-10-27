#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "global_stuff.h"

using namespace std; 

#define SERVER_IP "127.0.0.1"


int main() {
    int client_fd;
    int udp_fd; 
    struct sockaddr_in server_address;
    struct sockaddr_in udp_address; 
    char buffer[BUFFER_SIZE] = {0};

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        cerr << "Connection Failed" << endl;
        return 1;
    }
    cout << "Connected to server." << endl;

    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    int opt = 1;
    setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    udp_address.sin_family = AF_INET; 
    udp_address.sin_port = htons(UDP_PORT_CUSTOMER);
    udp_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(udp_fd, (struct sockaddr *)&udp_address, sizeof(udp_address)) < 0){
        cerr << "UDP bind failed" << endl; 
        return 1;
    }

    fd_set read_fds; 
    while (true){
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds); 
        FD_SET(client_fd, &read_fds);
        FD_SET(udp_fd, &read_fds);

        int max_fd = max(client_fd, max(udp_fd, STDIN_FILENO));
        select(max_fd + 1, &read_fds, NULL, NULL, NULL); 
        if (FD_ISSET(STDIN_FILENO, &read_fds)){
            string line; 
            if (!getline(cin, line))
                break; 
            line.push_back('\n');
            write(client_fd, line.c_str(), (int)line.size());
        }
        if (FD_ISSET(client_fd, &read_fds)){
            memset(buffer, 0, BUFFER_SIZE);
            int num_read = read(client_fd, buffer, BUFFER_SIZE);
            if (num_read <= 0){
                cout << "Server disconnected." << endl; 
                break;
            }
            cout << buffer << endl; 
        }
        if (FD_ISSET(udp_fd, &read_fds)){
            memset(buffer, 0, BUFFER_SIZE);
            read(udp_fd, buffer, BUFFER_SIZE);
            cout << "Broadcast: " << buffer << endl; 
        }
    }
    close(client_fd);
    close(udp_fd);
    return 0;
}