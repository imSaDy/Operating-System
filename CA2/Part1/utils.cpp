#include "utils.h"
#include <sstream>

using namespace std;

vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        // Trim leading/trailing whitespace
        size_t first = token.find_first_not_of(" \t");
        if (first == string::npos) continue; 
        size_t last = token.find_last_not_of(" \t");
        tokens.push_back(token.substr(first, (last - first + 1)));
    }
    return tokens;
}

vector<string> split_spaces(const string &s) {
    vector<string> tokens;
    stringstream ss(s);
    string temp;
    while (ss >> temp) tokens.push_back(temp);
    return tokens;
}