#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Race.h"

int global_counter = 0; 
int max_count = 0;
int flag_use_mutex = 0;
pthread_mutex_t lock; 

void* runner(void* arg){
    for (int i = 0; i < max_count; i++){
        if (flag_use_mutex){
            pthread_mutex_lock(&lock);
            global_counter++;
            pthread_mutex_unlock(&lock);
        }
        else {
            global_counter++;
        }
    }
    return NULL; 
}

int main(){
    int M; 
    printf("Number of threads M = ");
    if (scanf("%d", &M) != 1)
        return 1; 
    
    printf("Max Count = ");
    if (scanf("%d", &max_count) != 1)
        return 1; 
    
    printf("Mutex Lock Enable = ");
    if (scanf("%d", &flag_use_mutex) != 1)
        return 1; 
    
    if (flag_use_mutex){
        if (pthread_mutex_init(&lock, NULL) != 0){
            perror("Mutex init failed");
            return 1; 
        }
    }

    pthread_t threads[M]; 
    thread_arg_type thread_args[M];
    for (int i = 0; i < M; i++){
        thread_args[i].id = i;
        if (pthread_create(&threads[i], NULL, runner, &thread_args[i]) != 0){
            perror("Thread creation failed");
            return 1; 
        }
    }

    for (int i = 0; i < M; i++){
        pthread_join(threads[i], NULL);
    }

    if (flag_use_mutex){
        pthread_mutex_destroy(&lock);
    }
    int expected = M * max_count;
    printf("Final Counter:    %d\n", global_counter);
    printf("Expected Counter: %d\n", expected);
    if (global_counter != expected) {
        printf("RESULT: \033[0;31mRace Condition Detected! (Data Loss)\033[0m\n");
    } else {
        printf("RESULT: \033[0;32mSuccess! (Count is correct)\033[0m\n");
    }
    return 0;
}