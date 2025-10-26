#pragma once

#include <vector>

using namespace std;

#define TCP_PORT 8080
#define UDP_PORT_AIRLINE 8081
#define UDP_PORT_CUSTOMER 8082

const int BUFFER_SIZE = 1024; 

struct User {
    string username;
    string password;
    string role;
    User(string _username, string _password, string _role){
        username = _username;
        password = _password; 
        role = _role; 
    }
};

struct Seat {
    string id; 
    bool is_reserved; 
    Seat(string _id) {
        id = _id; 
        is_reserved = false;
    }
};

struct Flight {
    string flight_id;
    string origin;
    string destination;
    string time;
    vector<Seat*> seat_map;
  
    Flight(string _id, string _origin, string _dest, string _time) {
        flight_id = _id;
        origin = _origin;
        destination = _dest;
        time = _time;
    }

    void add_seat(Seat* seat){
        seat_map.push_back(seat);
    }
};