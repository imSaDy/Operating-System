#include "commands.h"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

void cmd_print_n(int n) {
    for (int i = 0; i < n; i++) {
        cout << "hello";
        if (i < n - 1) cout << ", ";
    }
    cout << endl;
}

void cmd_print_n_cli(const vector<string>& args) {
    int n = 0;
    string input_str;

    if (args.size() > 1) {
        input_str = args[1];
    } else {
        if (!(cin >> input_str)) {
            return;
        }
    }

    try {
        size_t pos;
        n = stoi(input_str, &pos);
        if (pos < input_str.length()) {
            throw invalid_argument("Trailing characters found");
        }
    } catch (...) {
        cerr << "Error: Invalid argument for print_n. Expected a number, got: '" << input_str << "'" << endl;
        return;
    }
    
    if (n > 0) {
        cmd_print_n(n);
    }
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
        char c;
        string line = "";
        while (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\n') break;
            line += c;
        }
        cout << line << endl;
    }
}