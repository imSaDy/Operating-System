#ifndef ARRAYSUM_H
#define ARRAYSUM_H

#include <pthread.h>

#define SIZE 100000000

typedef struct {
    int l, r; 
    long long sum; 
} thread_arg_type;

void* runner(void* arg);
void run_single_threaded(void);
void run_multi_threaded(int T);

#endif