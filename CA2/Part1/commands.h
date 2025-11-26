#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>

// Core logic
void cmd_print_n(int n);

// Wrapper that handles args vs stdin logic
void cmd_print_n_cli(const std::vector<std::string>& args);

void cmd_len();
void cmd_echo(const std::vector<std::string>& args);

#endif