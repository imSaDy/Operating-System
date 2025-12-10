#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector> 
#include <string> 
#include "Process.h"

using namespace std; 

void printMetrics(vector <Process>& processes, string algorithm_name);
void testFCFS(vector <Process> processes);
void testSJF(vector <Process> processes);
void testRR(vector <Process> processes, int quantum);

#endif