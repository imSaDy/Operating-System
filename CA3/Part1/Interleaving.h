#ifndef INTERLEAVING_H
#define INTERLEAVING_H

#include <pthread.h>

typedef struct {
    int id; 
    int N; 
} thread_arg_type; 

void* runner(void* arg);

#endif