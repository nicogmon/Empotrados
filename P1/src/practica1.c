#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 4


typedef struct {
    int id;
} ThreadArgs;

void *thread_function(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    double epoch;
    double elapsed;
    
    for (int i = 1; i < 6; i++){
        
        struct timespec start_time, current_time;
        

        clock_gettime(CLOCK_REALTIME, &start_time);

        volatile unsigned long long j;

        for (j=0; j < 32000000ULL; j++)
        
        clock_gettime(CLOCK_REALTIME, &current_time);

        epoch = start_time.tv_sec + start_time.tv_nsec / 1e9;

        elapsed = (current_time.tv_sec  + (current_time.tv_nsec / 1e9)) - (start_time.tv_sec +  (start_time.tv_nsec / 1e9));

        printf("[%.9f] Thread %d - Iteracion %d: Coste= %.9f s.", epoch, args->id, i, elapsed );

        if (elapsed > 0.9){
            printf("(fallo temporal)\n");
            continue;
        }
        printf("\n");
        
        usleep((0.9 - elapsed) * 1000000);

    }
    free(args);
    
}


int
main(int argc, char *argv[])
{
    pthread_t threads[MAX_THREADS];
    ThreadArgs *thread_args[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        thread_args[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        thread_args[i]->id  = i+1;
        pthread_create(&threads[i], NULL, thread_function, (void *)thread_args[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(threads[i], NULL);

    }
    return 0;

}
