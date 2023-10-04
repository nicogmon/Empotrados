#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 1


typedef struct {
    int id;
} ThreadArgs;

void *thread_function(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;

    for (int i = 1; i < 6; i++){
        time_t epoch_time;
        struct timespec start_time, current_time;
        

        clock_gettime(CLOCK_REALTIME, &start_time);

       

        volatile unsigned long long j;
        for (j=0; j < 42000000ULL; j++)
        
        clock_gettime(CLOCK_REALTIME, &current_time);
        double epoch = start_time.tv_sec + start_time.tv_nsec / 1e9;
        double elapsed = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_nsec - start_time.tv_nsec) / 1e9;

        printf("[%.9f]Thread %d - Iteracion %d: Coste= %.9f s.", epoch, args->id, i, elapsed );

        if (elapsed > 0.9){
            printf("(fallo temporal)");
        }
        printf("\n");
        //printf("%f\n", 0.9-elapsed);
        usleep((0.9 - elapsed) * 1000000);
        clock_gettime(CLOCK_REALTIME, &current_time);
        elapsed = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_nsec - start_time.tv_nsec) / 1e9;
        printf("Thread %lu - Iteracion %d: tiempo iteracion= %.9f s.\n", pthread_self(), i, elapsed );

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