#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

// Splits a string by a specific delimiter (e.g., '|')
std::vector<std::string> split(const std::string &s, char delimiter);

// Splits a string by spaces to get arguments
std::vector<std::string> split_spaces(const std::string &s);

#endif