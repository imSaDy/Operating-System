#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "executor.h"

using namespace std;

int main() {
    if (mkfifo("np", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
    }

    string input;
    while (true) {
        cout << "> "; 
        if (!getline(cin, input)) break;
        if (input == "exit") break;

        run_pipeline(input);
    }

    unlink("np");
    return 0;
}