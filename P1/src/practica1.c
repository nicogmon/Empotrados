#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 1

void *thread_function(void *arg)
{
    for (int i = 1; i < 6; i++){
        time_t epoch_time;
        struct timespec start_time;
        struct timespec current_time;

        clock_gettime(CLOCK_MONOTONIC, &start_time);
        volatile unsigned long long j;
        for (j=0; j < 42000000ULL; j++)

        clock_gettime(CLOCK_MONOTONIC, &current_time);
        double elapsed = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_nsec - start_time.tv_nsec) / 1e9;

        printf("[%.9f]Thread %lu - Iteracion %d: Coste= %.9f s.",(double)time(NULL), pthread_self(), i, elapsed );
        if (elapsed > 0.5){
            printf("(fallo temporal)");
        }
        printf("\n");
        printf("%f\n", 0.9-elapsed);
        usleep((0.9 - elapsed) * 1000000);
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_nsec - start_time.tv_nsec) / 1e9;
        printf("Thread %lu - Iteracion %d: tiempo iteracion= %.9f s.\n", pthread_self(), i, elapsed );

    }
    
}




int
main(int argc, char *argv[])
{
    pthread_t threads[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    return 0;

}