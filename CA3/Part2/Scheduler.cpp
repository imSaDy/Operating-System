#include "Scheduler.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

void printMetrics(vector<Process> &processes, string algorithm_name){
    double total_wt = 0, total_tat = 0;
    
    cout << "------------------------------------------------------------\n";
    cout << "Algorithm: " << algorithm_name << "\n";
    cout << "------------------------------------------------------------\n";
    cout << left << setw(10) << "PID" 
         << setw(10) << "Arrival" 
         << setw(10) << "Burst" 
         << setw(10) << "Finish" 
         << setw(10) << "Wait" 
         << setw(10) << "Turnaround" << endl;

    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    for (const auto& p : processes) {
        total_wt += p.waiting_time;
        total_tat += p.turnaround_time;
        cout << left << setw(10) << p.pid 
             << setw(10) << p.arrival_time 
             << setw(10) << p.burst_time 
             << setw(10) << p.finish_time 
             << setw(10) << p.waiting_time 
             << setw(10) << p.turnaround_time << endl;
    }

    cout << "------------------------------------------------------------\n";
    cout << "Average Waiting Time: " << (total_wt / processes.size()) << endl;
    cout << "Average Turnaround Time: " << (total_tat / processes.size()) << endl;
    cout << "------------------------------------------------------------\n\n";
}

bool cmp(Process a, Process b){
    return a.arrival_time < b.arrival_time; 
}

void testFCFS(vector<Process> processes){
    sort(processes.begin(), processes.end(), cmp);
    
    int timer = 0; 
    int n = processes.size(); 
    for (int i = 0; i < n; i++){
        if (timer < processes[i].arrival_time)
            timer = processes[i].arrival_time;
        processes[i].start_time = timer; 
        timer += processes[i].burst_time; 
        processes[i].finish_time = timer; 
        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        processes[i].is_completed = true;
    }
    printMetrics(processes, "FCFS");
}

void testSJF(vector<Process> processes){
    int n = processes.size(); 
    int completed = 0; 
    int timer = 0;

    for (int i = 0; i < n; i++)
        processes[i].is_completed = false;
    
    while (completed != n){
        int idx = -1;

        for (int i = 0; i < n; i++){
            if (!processes[i].is_completed){
                if (processes[i].arrival_time <= timer){
                    if (idx == -1 || processes[i].burst_time < processes[idx].burst_time)
                        idx = i; 
                }
            }
        }
        if (idx != -1){
            processes[idx].start_time = timer; 
            timer += processes[idx].burst_time; 
            processes[idx].finish_time = timer; 
            processes[idx].turnaround_time = processes[idx].finish_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
            processes[idx].is_completed = true;
            completed++;
        }
        else {
            timer++;
        }
    }
    printMetrics(processes, "SJF (Non-Preemptive)");
}

void check_arrival_processes(vector<Process>& processes, int timer, queue <int>& ready){
    int n = processes.size(); 
    for (int i = 0; i < n; i++){
        if (!processes[i].is_ready && processes[i].remaining_time > 0 && processes[i].arrival_time <= timer){
            ready.push(i);
            processes[i].is_ready = true; 
        }
    }
}

void testRR(vector<Process> processes, int quantum){
    int n = processes.size(); 
    int timer = 0; 
    int completed = 0; 
    for (int i = 0; i < n; i++){
        processes[i].remaining_time = processes[i].burst_time; 
        processes[i].is_completed = false; 
        processes[i].is_ready = false; 
    }
    sort(processes.begin(), processes.end(), cmp);
    queue <int> ready;

    if (processes[0].arrival_time > 0)
        timer = processes[0].arrival_time; 

    check_arrival_processes(processes, timer, ready);

    while (completed != n){
        if (ready.empty()){
            timer++;
            check_arrival_processes(processes, timer, ready);
            continue; 
        }
        int id = ready.front(); 
        ready.pop();
        int cpu_time = min(quantum, processes[id].remaining_time);
        processes[id].remaining_time -= cpu_time; 

        timer += cpu_time;
        check_arrival_processes(processes, timer, ready);

        if (processes[id].remaining_time > 0){
            ready.push(id);
        }
        else {
            processes[id].finish_time = timer;
            processes[id].turnaround_time = processes[id].finish_time - processes[id].arrival_time;
            processes[id].waiting_time = processes[id].turnaround_time - processes[id].burst_time;
            completed++;
        }
    }
    printMetrics(processes, "Round Robin (Q=" + to_string(quantum) + ")");
}
