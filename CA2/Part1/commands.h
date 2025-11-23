#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>

void cmd_print_n(int n);
void cmd_len();
void cmd_echo(const std::vector<std::string>& args);

#endif