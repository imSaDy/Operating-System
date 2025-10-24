#include <iostream>
#include <unistd.h>     // For read(), write(), close()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <netinet/in.h> // For sockaddr_in
#include <string.h>     // For memset()
#include <vector>
#include "global_stuff.h"     // For TCP_PORT

using namespace std; 

const int MAX_SERVER_QUEUE = 5; 
const int COMMAND_WORD = 0;
const int REG_ROLE = 1; 
const int REG_USERNAME = 2; 
const int REG_PASSWORD = 3; 

vector <string> tokenize(string command){
    vector <string> tokens;
    string token = "";
    for (auto c : command){
        if (c == ' '){
            if (!token.empty()){
                tokens.push_back(token);
            }
        }
        else {
            token.push_back(c);
        }
    }
    if (!token.empty()){
        tokens.push_back(token);
    }
    return tokens; 
}

bool check_duplicate_username(vector <User*> &users, string username){
    for (auto user : users){
        if (user->username == username)
            return true; 
    }
    return false; 
}

void handle_command(vector <User*> &users, int client_socket, string buffer){
    vector <string> command = tokenize(buffer);
    if (command[COMMAND_WORD] == "REGISTER"){
        if (command.size() <= REG_PASSWORD){
            write(client_socket, "ERROR Invalid command format", 28);
            return; 
        }
        if (check_duplicate_username(users, command[REG_USERNAME])){
            write(client_socket, "ERROR UsernameAlreadyExists", 27);
            return; 
        }
        else {
            User* new_user = new User(command[REG_USERNAME], command[REG_PASSWORD], command[REG_ROLE]);
            users.push_back(new_user);
            cout << "New user registered: " << command[REG_USERNAME] << endl;
            write(client_socket, "REGISTERED OK", 13);
        }
    }
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    vector <User*> users;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1; 
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_PORT);
    
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, MAX_SERVER_QUEUE);
    cout << "Server is listening on port " << TCP_PORT << endl; 

    while (true){
        cout << "Waiting for a new client connection..." << endl; 
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        cout << "Client connected, Socket FD = " << client_socket << endl; 

        while (true){
            memset(buffer, 0, BUFFER_SIZE);
            int num_read = read(client_socket, buffer, BUFFER_SIZE);
            if (num_read <= 0){
                cout << "Client disconnected, Socket FD: " << client_socket << endl; 
                close(client_socket);
                break;
            }
            cout << "Recieved from client: " << buffer; 
            for (int i = 0; i < BUFFER_SIZE; i++){
                if (buffer[i] == '\n'){
                    buffer[i] = '\0';
                    break; 
                }
            }
            handle_command(users, client_socket, buffer);
        }
    }
    close(server_fd);
    return 0;
}