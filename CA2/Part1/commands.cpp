#include "commands.h"
#include <iostream>

using namespace std;

void cmd_print_n(int n) {
    for (int i = 0; i < n; i++) {
        cout << "hello";
        if (i < n - 1) cout << ", ";
    }
    cout << endl;
}

void cmd_len() {
    string line, full_input;
    while (getline(cin, line)) {
        full_input += line; 
    }
    cout << full_input.length() << endl;
}

void cmd_echo(const vector<string>& args) {
    if (args.size() > 1) {
        for (size_t i = 1; i < args.size(); i++) {
            cout << args[i] << (i == args.size() - 1 ? "" : " ");
        }
        cout << endl;
    } else {
        string line;
        while (getline(cin, line)) {
            cout << line << endl;
        }
    }
}