#ifndef RACE_H
#define RACE_H

#include <pthread.h>

typedef struct {
    int id; 
} thread_arg_type;

extern int global_counter; 
extern int max_count; 
extern int flag_use_mutex; 
extern pthread_mutex_t lock; 

void* runner(void* arg);

#endif