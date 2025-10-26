#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <arpa/inet.h>
#include "global_stuff.h"

using namespace std; 


void debug_out() { cerr << endl; }
 
template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T) {
	cerr << " " << H ;
	debug_out(T...);
}

template <typename T>
inline T lowbit(T x) { return x & -x; }

#define debug(...)             cerr << "(" << #__VA_ARGS__ << "): ", debug_out(__VA_ARGS__)

const int MAX_SERVER_QUEUE = 5; 
const int COMMAND_WORD = 0;
const int REG_ROLE = 1; 
const int REG_USERNAME = 2; 
const int REG_PASSWORD = 3; 
const int LOGIN_USERNAME = 1; 
const int LOGIN_PASSWORD = 2;
const int ADD_FLIGHT_ID = 1;
const int ADD_FLIGHT_ORIGIN = 2;
const int ADD_FLIGHT_DEST = 3;
const int ADD_FLIGHT_TIME = 4;
const int ADD_FLIGHT_COLS = 5;
const int ADD_FLIGHT_ROWS = 6;

vector <string> tokenize(string command){
    vector <string> tokens;
    string token = "";
    for (auto c : command){
        if (c == ' '){
            if (!token.empty()){
                tokens.push_back(token);
                token.clear();
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

void send_broadcast(int udp_socket, string message, int port){
    struct sockaddr_in broadcast_address; 
    memset(&broadcast_address, 0, sizeof(broadcast_address));
    broadcast_address.sin_family = AF_INET; 
    broadcast_address.sin_port = htons(port);
    inet_pton(AF_INET, "255.255.255.255", &broadcast_address.sin_addr);
    sendto(udp_socket, message.c_str(), (int)message.size(), 0, (struct sockaddr *)&broadcast_address, sizeof(broadcast_address));
}

void handle_register_command(vector <User*> &users, int client_socket, vector <string> &command, int udp_socket){
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
        string message = "BROADCAST NEW_USER " + command[REG_USERNAME] + " " + command[REG_ROLE];
        send_broadcast(udp_socket, message, UDP_PORT_AIRLINE);
    }
}

User* find_user_by_username(vector <User*> &users, string username){
    for (auto user : users){
        if (user->username == username)
            return user; 
    }
    return NULL;
}

Flight* find_flight_by_id(vector <Flight*> &flights, string id){
    for (auto flight : flights){
        if (flight->flight_id == id)
            return flight; 
    }
    return NULL; 
}

void handle_login_command(vector <User*> &users, int client_socket, vector <string> &command){
    if (command.size() <= LOGIN_PASSWORD){
        write(client_socket, "ERROR Invalid command format", 28);
        return; 
    }
    User* user = find_user_by_username(users, command[LOGIN_USERNAME]);
    if (user == NULL)
        write(client_socket, "ERROR UserNotFound", 18);
    else if (user->password != command[LOGIN_PASSWORD])
        write(client_socket, "ERROR InvalidPassword", 21);
    else {
        cout << command[LOGIN_USERNAME] << " logged in successfully" << endl; 
        write(client_socket, "LOGIN OK", 8);
    }
}

void handle_add_flight_command(vector<Flight*> &flights, vector <User*> &users, int client_socket, vector <string> &command, int udp_socket){
    if (command.size() <= ADD_FLIGHT_ROWS){
        write(client_socket, "ERROR Invalid command format", 28);
        return; 
    }
    Flight* flight = find_flight_by_id(flights, command[ADD_FLIGHT_ID]);
    if (flight != NULL){
        write(client_socket, "ERROR DuplicateFlightID", 23);
        return; 
    }
    Flight* new_flight = new Flight(command[ADD_FLIGHT_ID], command[ADD_FLIGHT_ORIGIN], command[ADD_FLIGHT_DEST], command[ADD_FLIGHT_TIME]);
    int num_col = stoi(command[ADD_FLIGHT_COLS]);
    int num_row = stoi(command[ADD_FLIGHT_ROWS]);
    for (int col = 0; col < num_col; col++){
        char col_char = 'A' + col; 
        for (int row = 1; row <= num_row; row++){
            Seat* new_seat = new Seat(col_char + to_string(row));
            new_flight->add_seat(new_seat);
        }
    }
    flights.push_back(new_flight);
    cout << "New flight added: " << command[ADD_FLIGHT_ID] << endl;
    write(client_socket, "FLIGHT ADDED OK", 15);
    string broadcast_message = "BROADCAST NEW FLIGHT " + new_flight->flight_id + " " + new_flight->origin + " " + new_flight->destination + " " + new_flight->time;
    send_broadcast(udp_socket, broadcast_message, UDP_PORT_CUSTOMER);
}

void handle_command(vector <Flight*> flights, vector <User*> &users, int client_socket, string buffer, int udp_socket_customer, int udp_socket_airline){
    vector <string> command = tokenize(buffer);
    if (command[COMMAND_WORD] == "REGISTER")
        handle_register_command(users, client_socket, command, udp_socket_airline);
    if (command[COMMAND_WORD] == "LOGIN")
        handle_login_command(users, client_socket, command);
    if (command[COMMAND_WORD] == "ADD_FLIGHT")
        handle_add_flight_command(flights, users, client_socket, command, udp_socket_customer);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    vector <User*> users;
    vector <Flight*> flights; 

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int tcp_opt = 1; 
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &tcp_opt, sizeof(tcp_opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, MAX_SERVER_QUEUE);
    cout << "Server is listening on port " << TCP_PORT << endl; 

    int udp_socket_airline; 
    udp_socket_airline = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_airline < 0){
        cerr << "UDP socket airline creation faild" << endl; 
        return 1; 
    }
    int broadcast_opt_airline = 1; 
    setsockopt(udp_socket_airline, SOL_SOCKET, SO_BROADCAST, &broadcast_opt_airline, sizeof(broadcast_opt_airline));

    int udp_socket_customer; 
    udp_socket_customer = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_customer < 0){
        cerr << "UDP socket customer creation faild" << endl; 
        return 1; 
    }
    int broadcast_opt_customer = 1; 
    setsockopt(udp_socket_customer, SOL_SOCKET, SO_BROADCAST, &broadcast_opt_customer, sizeof(broadcast_opt_customer));

    fd_set master_read_fds, tmp_read_fds;
    int max_fd;
    FD_ZERO(&master_read_fds);
    FD_SET(server_fd, &master_read_fds);
    max_fd = server_fd;

    while (true){
        tmp_read_fds = master_read_fds; 
        if (select(max_fd + 1, &tmp_read_fds, NULL, NULL, NULL) == -1){
            cerr << "select() error" << endl; 
            return 1; 
        }
        for (int i = 0; i <= max_fd; i++){
            if (FD_ISSET(i, &tmp_read_fds)){
                if (i == server_fd){
                    int new_client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    if (new_client_socket < 0){
                        cerr << "accept() error" << endl; 
                    }
                    else {
                        FD_SET(new_client_socket, &master_read_fds);
                        max_fd = max(max_fd, new_client_socket);
                        cout << "New client connected, Socket FD = " << new_client_socket << endl; 
                    }
                }
                else {
                    memset(buffer, 0, BUFFER_SIZE);
                    int num_read = read(i, buffer, BUFFER_SIZE);
                    if (num_read <= 0){
                        cout << "Client disconnected, Socket FD: " << i << endl; 
                        close(i);
                        FD_CLR(i, &master_read_fds);
                    }
                    else {
                        cout << "Received from client " << i << ": " << buffer; 
                        for (int j = 0; j < BUFFER_SIZE; j++){
                            if (buffer[j] == '\n'){
                                buffer[j] = '\0';
                                break; 
                            }
                        }
                        handle_command(flights, users, i, buffer, udp_socket_customer, udp_socket_airline);
                    }
                }
            }
        }
    }
    close(server_fd);
    close(udp_socket_airline);
    close(udp_socket_customer);

    return 0;
}