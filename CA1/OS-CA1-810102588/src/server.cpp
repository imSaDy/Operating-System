#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <arpa/inet.h>
#include <map>
#include <signal.h>
#include <errno.h>
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
const int RESERVE_FLIGHT_ID = 1;
const int RESERVE_SEAT_START = 2;
const int CONFIRM_RESERVATION_ID = 1; 
const int CANCEL_RESERVATION_ID = 1;

map <int, Reservation*> reservations; 
int new_reservation_id = 1; 

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

void handle_login_command(vector <User*> &users, int client_socket, vector <string> &command, map <int, User*> &logged_in_users){
    User* user = find_user_by_username(users, command[LOGIN_USERNAME]);
    if (user == NULL)
        write(client_socket, "ERROR UserNotFound", 18);
    else if (user->password != command[LOGIN_PASSWORD])
        write(client_socket, "ERROR InvalidPassword", 21);
    else {
        cout << command[LOGIN_USERNAME] << " logged in successfully" << endl; 
        write(client_socket, "LOGIN OK", 8);
        logged_in_users[client_socket] = user; 
    }
}

void handle_add_flight_command(vector<Flight*> &flights, vector <User*> &users, int client_socket, vector <string> &command, int udp_socket, map <int, User*> &logged_in_users){
    if (logged_in_users.find(client_socket) == logged_in_users.end()){
        write(client_socket, "ERROR Not logged in", 19);
        return;
    }
    User* user = logged_in_users[client_socket]; 
    if (user->role != "AIRLINE"){
        write(client_socket, "ERROR Permission denied", 23);
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
    write(client_socket, "FLIGHT_ADDED OK", 15);
    string broadcast_message = "BROADCAST NEW_FLIGHT " + new_flight->flight_id + " " + new_flight->origin + " " + new_flight->destination + " " + new_flight->time;
    send_broadcast(udp_socket, broadcast_message, UDP_PORT_CUSTOMER);
}

void handle_list_flights_command(vector<Flight*> &flights, int client_socket){
    string output = ""; 
    for (auto flight : flights){
        output += flight->get_as_flight_list_item(); 
    }
    write(client_socket, output.c_str(), (int)output.size());
}

void handle_reserve_command(vector<Flight*> &flights, int client_socket, vector<string> &command, map<int, User*> &logged_in_users){
    if (logged_in_users.find(client_socket) == logged_in_users.end()){
        write(client_socket, "ERROR Not logged in", 19);
        return;
    }
    User* user = logged_in_users[client_socket];
    if (user->role != "CUSTOMER"){
        write(client_socket, "ERROR Permission denied", 23);
        return;
    }
    Flight* flight = find_flight_by_id(flights, command[RESERVE_FLIGHT_ID]);
    if (flight == NULL){
        write(client_socket, "ERROR FlightNotFound", 20);
        return;
    }
    vector <Seat*> requested_seats; 
    for (int i = RESERVE_SEAT_START; i < (int)command.size(); i++){
        string seat_id = command[i];
        Seat* seat = flight->find_seat_by_id(seat_id);
        if (seat == NULL){
            string error = "ERROR SeatNotFound " + seat_id;
            write(client_socket, error.c_str(), error.length());
            return;
        }
        if (seat->is_reserved){
            string error_message = "ERROR SeatAlreadyReserved " + seat_id;
            write(client_socket, error_message.c_str(), error_message.length());
            return;
        }
        requested_seats.push_back(seat);
    }
    int new_id = new_reservation_id++;
    Reservation* new_reservation = new Reservation(new_id, command[RESERVE_FLIGHT_ID], user->username);
    for (auto seat : requested_seats){
        seat->is_reserved = true; 
        new_reservation->reserved_seats.push_back(seat);
    }
    reservations[new_id] = new_reservation; 
    string output = "RESERVED TEMP " + to_string(new_id) + " EXPIRES IN 30";
    write(client_socket, output.c_str(), output.length());
}

void handle_confirm_command(int client_socket, vector <string> &command, map <int, User*> &logged_in_users){
    if (logged_in_users.find(client_socket) == logged_in_users.end()){
        write(client_socket, "ERROR Not logged in", 19);
        return;
    }
    int reservation_id = stoi(command[CONFIRM_RESERVATION_ID]);
    if (reservations.find(reservation_id) == reservations.end()) {
        write(client_socket, "ERROR ReservationExpired", 24);
        return;
    }
    Reservation* reserve = reservations[reservation_id];
    if (reserve->status == CONFIRMED){
        write(client_socket, "ERROR ReservationAlreadyConfirmed", 33);
        return;
    }
    reserve->status = CONFIRMED;
    write(client_socket, "CONFIRMATION OK\n", 16);
}

void handle_cancel_command(int client_socket, vector <string> &command, map<int, User*> logged_in_users){
    if (logged_in_users.find(client_socket) == logged_in_users.end()) {
        write(client_socket, "ERROR Not logged in", 19);
        return;
    }
    int reservation_id = stoi(command[CANCEL_RESERVATION_ID]);
    Reservation* res = reservations[reservation_id];
    if (res == NULL){
        write(client_socket, "ERROR ReservationNotFound", 25);
        return;
    }
    for (auto seat : res->reserved_seats) {
        seat->is_reserved = false;
    }
    delete res;
    reservations.erase(reservation_id);
    write(client_socket, "CANCELLED OK\n", 13);
}

void handle_command(vector <Flight*> &flights, vector <User*> &users, int client_socket, string buffer, int udp_socket_customer, int udp_socket_airline, map <int, User*> &logged_in_users){
    vector <string> command = tokenize(buffer);
    if (command[COMMAND_WORD] == "REGISTER")
        handle_register_command(users, client_socket, command, udp_socket_airline);
    if (command[COMMAND_WORD] == "LOGIN")
        handle_login_command(users, client_socket, command, logged_in_users);
    if (command[COMMAND_WORD] == "ADD_FLIGHT")
        handle_add_flight_command(flights, users, client_socket, command, udp_socket_customer, logged_in_users);
    if (command[COMMAND_WORD] == "LIST_FLIGHTS")
        handle_list_flights_command(flights, client_socket); 
    if (command[COMMAND_WORD] == "RESERVE")
        handle_reserve_command(flights, client_socket, command, logged_in_users);
    if (command[COMMAND_WORD] == "CONFIRM")
        handle_confirm_command(client_socket, command, logged_in_users);
    if (command[COMMAND_WORD] == "CANCEL")
        handle_cancel_command(client_socket, command, logged_in_users);
}

void check_reservations(int signal){
    time_t now = time(NULL);
    for (auto it = reservations.begin(); it != reservations.end();){
        Reservation* reserve = it->second;
        if (reserve->status == TEMPORARY){
            double diff_time = difftime(now, reserve->timestamp);
            if (diff_time > 30){
                cout << "Reservation " << reserve->reservation_id << " expired." << endl;
                for (auto seat : reserve->reserved_seats){
                    seat->is_reserved = false; 
                }
                delete reserve; 
                it = reservations.erase(it);
            }
            else
                it++;
        }
        else
            it++;
    }
    alarm(1);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    vector <User*> users;
    vector <Flight*> flights; 
    map <int, User*> logged_in_users; 

    signal(SIGALRM, check_reservations);

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

    alarm(1);

    while (true){
        tmp_read_fds = master_read_fds; 
        if (select(max_fd + 1, &tmp_read_fds, NULL, NULL, NULL) == -1){
            if (errno == EINTR)
                continue;
            cerr << "select error" << endl; 
            return 1; 
        }
        for (int i = 0; i <= max_fd; i++){
            if (FD_ISSET(i, &tmp_read_fds)){
                if (i == server_fd){
                    int new_client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    if (new_client_socket < 0){
                        cerr << "accept error" << endl; 
                    }
                    else {
                        FD_SET(new_client_socket, &master_read_fds);
                        max_fd = max(max_fd, new_client_socket);
                        cout << "New client connected with socket FD = " << new_client_socket << endl; 
                    }
                }
                else {
                    memset(buffer, 0, BUFFER_SIZE);
                    int num_read = read(i, buffer, BUFFER_SIZE);
                    if (num_read <= 0){
                        cout << "Client disconnected with socket FD: " << i << endl; 
                        logged_in_users.erase(i);
                        close(i);
                        FD_CLR(i, &master_read_fds);
                    }
                    else {
                        for (int j = 0; j < BUFFER_SIZE; j++){
                            if (buffer[j] == '\n'){
                                buffer[j] = '\0';
                                break; 
                            }
                        }
                        handle_command(flights, users, i, buffer, udp_socket_customer, udp_socket_airline, logged_in_users);
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