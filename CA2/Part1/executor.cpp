#include "executor.h"
#include "commands.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <sys/select.h> // Required for select()

using namespace std;

// Helper: Uses select() to wait for data to become available on the file descriptor.
// Returns true if data is ready, false if timeout.
bool wait_for_data(int fd, int seconds) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    // select pauses here until 'fd' has data OR 'seconds' pass.
    int result = select(fd + 1, &read_fds, NULL, NULL, &timeout);

    return (result > 0 && FD_ISSET(fd, &read_fds));
}

void execute_single_command(string cmd_str) {
    vector<string> args = split_spaces(cmd_str);
    if (args.empty()) exit(0);

    // --- Redirection Handling ---
    if (args.size() >= 2) {
        if (args[args.size() - 2] == ">" && args.back() == "np") {
            // Write: Standard blocking open is fine because main.cpp holds a reader.
            int fd = open("np", O_WRONLY);
            if (fd >= 0) { dup2(fd, STDOUT_FILENO); close(fd); }
            args.pop_back(); args.pop_back();
        } 
        else if (args[args.size() - 2] == "<" && args.back() == "np") {
            // Read: MUST use O_NONBLOCK + select to prevent hanging if writers are dead.
            int fd = open("np", O_RDONLY | O_NONBLOCK);
            
            if (fd >= 0) {
                // Wait up to 2 seconds for data
                if (wait_for_data(fd, 2)) {
                    dup2(fd, STDIN_FILENO);
                } else {
                    // Optional: Print error or just close if timed out
                    // cerr << "Error: Timeout waiting for data in np" << endl;
                }
                // We close 'fd' here because dup2 created a copy on STDIN
                // However, we must keep the connection alive for the command.
                // Closing 'fd' is safe because STDIN (0) now holds the reference.
                close(fd);
            }
            args.pop_back(); args.pop_back();
        }
    }

    // --- Command Execution ---
    if (args[0] == "print_n") {
        // Updated to use the CLI wrapper that checks for Stdin fallback
        cmd_print_n_cli(args);
    } else if (args[0] == "len") {
        cmd_len();
    } else if (args[0] == "echo") {
        cmd_echo(args);
    } else if (args[0] == "np") {
        // "np" command (Reader): Uses same Non-Blocking + Select logic
        int fd = open("np", O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            if (wait_for_data(fd, 2)) {
                char buffer[1024];
                ssize_t bytes;
                while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
                    write(STDOUT_FILENO, buffer, bytes);
                }
            }
            close(fd);
        }
    } else {
        vector<char*> c_args;
        for (auto &s : args) c_args.push_back(&s[0]);
        c_args.push_back(nullptr);
        execvp(c_args[0], c_args.data());
        exit(1); 
    }
    exit(0); 
}

void run_pipeline(string input_line) {
    vector<string> commands = split(input_line, '|');
    int num_cmds = commands.size();

    int prev_pipe_read = -1;
    int curr_pipe[2];
    vector<int> pids;

    for (int i = 0; i < num_cmds; i++) {
        if (i < num_cmds - 1) {
            if (pipe(curr_pipe) < 0) { perror("pipe"); return; }
        }

        int pid = fork();
        if (pid == 0) {
            // CHILD
            if (i > 0 && prev_pipe_read != -1) {
                dup2(prev_pipe_read, STDIN_FILENO);
                close(prev_pipe_read);
            }
            if (i < num_cmds - 1) {
                dup2(curr_pipe[1], STDOUT_FILENO);
                close(curr_pipe[1]);
                close(curr_pipe[0]);
            }
            execute_single_command(commands[i]);
        } else {
            // PARENT
            pids.push_back(pid);
            if (i > 0 && prev_pipe_read != -1) close(prev_pipe_read);
            if (i < num_cmds - 1) {
                close(curr_pipe[1]);
                prev_pipe_read = curr_pipe[0];
            }
        }
    }
    for (int pid : pids) waitpid(pid, NULL, 0);
}