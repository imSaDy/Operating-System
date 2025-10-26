#pragma once

#include <vector>
#include <string>
#include <time.h>

using namespace std;

#define TCP_PORT 8080
#define UDP_PORT_AIRLINE 8081
#define UDP_PORT_CUSTOMER 8082

const int BUFFER_SIZE = 1024; 

enum ReservationStatus {
    TEMPORARY,
    CONFIRMED
};

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

    string get_as_flight_list_item(){
        int total = seat_map.size();
        int free = 0;
        string result = "";
        for (auto seat : seat_map){
            if (!seat->is_reserved){
                free++;
            }
        }
        result += "FLIGHT " + flight_id + " " + origin + " " + destination + " " + time + " SEATS_AVAILABLE=" + to_string(free) + "/" + to_string(total) + "\n";
        return result; 
    }
    Seat* find_seat_by_id(string id){
        for (auto seat : seat_map){
            if (seat->id == id)
                return seat; 
        }
        return NULL; 
    }
};

struct Reservation{
    int id; 
    string flight_id;
    string username; 
    vector <Seat*> reserved_seats; 
    ReservationStatus status; 
    time_t timestamp; 
    Reservation(int _id, string _flight_id, string _username) {
        id = _id;
        flight_id = _flight_id;
        username = _username;
        status = TEMPORARY;
        timestamp = time(NULL);
    }
};