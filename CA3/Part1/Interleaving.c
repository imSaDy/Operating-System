#include "Interleaving.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void *runner(void *arg){
    thread_arg_type* info = (thread_arg_type*)arg;
    int sleep_time = rand() % 4; 
    sleep(sleep_time);
    printf("I'm thread %d of %d\n", info->id, info->N);
    free(info);
    return NULL;
}

int main(){
    int N; 
    srand(time(NULL));
    printf("N = "); 
    if (scanf("%d", &N) != 1)
        return 1; 
    
    pthread_t threads[N];
    for (int i = 0; i < N; i++){
        thread_arg_type* args = malloc(sizeof(thread_arg_type));
        if (args == NULL){
            perror("Failed to allocate args"); 
            return 1; 
        }
        args->id = i; 
        args->N = N; 

        if (pthread_create(&threads[i], NULL, runner, args) != 0){
            perror("Failed to create thread");
            return 1;
        }
    }
    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);
    return 0;
}