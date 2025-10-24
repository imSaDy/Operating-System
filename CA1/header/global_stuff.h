#pragma once

using namespace std;

#define TCP_PORT 8080

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