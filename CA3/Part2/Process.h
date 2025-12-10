#ifndef PROCESS_H
#define PROCESS_H 

#include <string>

using namespace std; 

struct Process {
    string pid; 
    int arrival_time; 
    int burst_time;
    int start_time; 
    int finish_time; 
    int waiting_time; 
    int turnaround_time; 
    int remaining_time; 
    bool is_completed; 
};

#endif