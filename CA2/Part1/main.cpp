#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> // Required for O_NONBLOCK
#include "executor.h"

using namespace std;

int main() {
    // 1. Create the named pipe
    if (mkfifo("np", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
    }

    // 2. GLOBAL READER (The Fix)
    // We open the pipe for Reading (O_RDONLY) in Non-Blocking mode.
    // We keep this variable 'dummy_fd' open for the entire life of the shell.
    // This ensures the pipe always has at least 1 reader, so data persists.
    int dummy_fd = open("np", O_RDONLY | O_NONBLOCK);

    string input;
    while (true) {
        cout << "> "; 
        if (!getline(cin, input)) break;
        if (input == "exit") break;

        run_pipeline(input);
    }

    // 3. Cleanup
    if (dummy_fd >= 0) close(dummy_fd);
    unlink("np");
    return 0;
}