#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "ArraySum.h"


static int *array;

static void print_results(long long sum, struct timespec start, struct timespec end){
    double time_elapsed = (end.tv_sec - start.tv_sec);
    time_elapsed += (end.tv_nsec - start.tv_nsec) * 1e-9; 
    printf("Sum: %lld\n", sum);
    printf("Time: %.6f seconds\n", time_elapsed);
}

void* runner(void* arg){
    thread_arg_type* data = (thread_arg_type*)arg;
    data->sum = 0;
    for (int i = data->l; i < data->r; i++){
        data->sum += array[i];
    }
    return NULL;
}

void run_single_threaded(void){
    struct timespec start, end; 
    printf("\n Running Single Threaded... \n");
    clock_gettime(CLOCK_MONOTONIC, &start);

    long long sum = 0;
    for (int i = 0; i < SIZE; i++){
        sum += array[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    print_results(sum, start, end);   
}

void run_multi_threaded(int T){
    struct timespec start, end; 
    pthread_t threads[T];
    thread_arg_type thread_args[T];
    int block_size = SIZE / T;
    int mod = SIZE % T;
    printf("\n Running Multi Threaded (%d threads)...  \n", T);
    clock_gettime(CLOCK_MONOTONIC, &start);

    int last = 0; 
    for (int i = 0; i < T; i++){
        thread_args[i].l = last;
        thread_args[i].r = last + block_size;  
        if (mod){
            thread_args[i].r++;
            mod--;
        }
        last = thread_args[i].r;
        if (pthread_create(&threads[i], NULL, runner, &thread_args[i]) != 0){
            perror("Thread creation failed");
            exit(1);
        }
    }
    long long total_sum = 0;
    for (int i = 0; i < T; i++){
        pthread_join(threads[i], NULL);
        total_sum += thread_args[i].sum;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    print_results(total_sum, start, end);
}

int main(){
    srand(time(NULL));

    array = malloc(SIZE * sizeof(int));
    if (array == NULL){
        perror("Memory allocation failed");
        return 1; 
    }
    for (int i = 0; i < SIZE; i++){
        array[i] = rand() % 100;
    }

    run_single_threaded();
    int T; 
    printf("T = "); 
    if (scanf("%d", &T) != 1)
        return 1;
    printf("Array Size = %d\n", SIZE);
    run_multi_threaded(T);
    free(array);
    return 0;
}

