#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "executor.h"

using namespace std;

int main() {
    if (mkfifo("np", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
    }

    int dummy_fd = open("np", O_RDONLY | O_NONBLOCK);

    string input;
    while (true) {
        cout << "> "; 
        if (!getline(cin, input)) break;
        if (input == "exit") break;

        run_pipeline(input);
    }

    if (dummy_fd >= 0) close(dummy_fd);
    unlink("np");
    return 0;
}