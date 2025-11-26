#include "commands.h"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>

using namespace std;

void cmd_print_n(int n) {
    for (int i = 0; i < n; i++) {
        cout << "hello";
        if (i < n - 1) cout << ", ";
    }
    cout << endl;
}

// Helper for the CLI command version of print_n
// It decides whether to use the argument or read from stdin
void cmd_print_n_cli(const vector<string>& args) {
    int n = 0;
    if (args.size() > 1) {
        // Case 1: Argument provided (print_n 5)
        n = stoi(args[1]);
    } else {
        // Case 2: No argument, read from Stdin (Previous Pipe)
        // Expected input format: a number as a string (e.g., "13\n")
        string input;
        if (cin >> input) {
            try {
                n = stoi(input);
            } catch (...) {
                return; // Invalid number, do nothing
            }
        }
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
        // Arguments provided: echo arg1 arg2
        for (size_t i = 1; i < args.size(); i++) {
            cout << args[i] << (i == args.size() - 1 ? "" : " ");
        }
        cout << endl;
    } else {
        // No arguments: Read from STDIN (Pipe/Redirection)
        // Read byte-by-byte to consume EXACTLY one line
        char c;
        string line = "";
        while (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\n') break;
            line += c;
        }
        cout << line << endl;
    }
}