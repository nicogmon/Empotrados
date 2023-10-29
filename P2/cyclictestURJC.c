#define _GNU_SOURCE  
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define N ((int) sysconf(_SC_NPROCESSORS_ONLN))
#define MAX_MEASURES 10000000
#define USLEEP_TMP 50000
FILE *csv_file;

struct Threadinfo {
    double avg_latency;
    double max_latency;
    double * measures;
    int cpu;
};typedef struct Threadinfo Threadinfo;



void * thread_actions(void * arg) {

    int measurement_count = 0, iteration = 0, result = 0;
    int * cpu = (int *) arg;
    double total_time = 0, elapsed = 0;

    Threadinfo * threadinfo = (Threadinfo *) malloc(sizeof(Threadinfo));
    threadinfo->measures = (double *) malloc(sizeof(double) * MAX_MEASURES * 2);
    memset(threadinfo->measures, 0, MAX_MEASURES * sizeof(double));
    threadinfo->avg_latency = 0;
    threadinfo->max_latency = 0;

    struct timespec start_time, current_time, timer_start, timer_end;

    cpu_set_t cpuset;
    pthread_t thread = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(*cpu, &cpuset);

    result = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    if (result != 0) {
        printf("Error: pthread_setaffinity_np\n");
    }    
    
    struct sched_param param =  {0};
    param.sched_priority = 99;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0)  {
        perror("Error: pthread_setschedparam");
    }

    clock_gettime(CLOCK_MONOTONIC, &timer_start);
    current_time = timer_start;

    threadinfo->cpu = *cpu;
    
    while((current_time.tv_sec - timer_start.tv_sec) < 60) {
        
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        usleep(USLEEP_TMP);
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed = (current_time.tv_sec  + (current_time.tv_nsec / 1e9)) - (start_time.tv_sec +  (start_time.tv_nsec / 1e9)) - (USLEEP_TMP/1e6);
        elapsed = elapsed * 1e9;
        
        iteration = measurement_count/2;
        threadinfo->measures [measurement_count] = iteration;
        threadinfo->measures [measurement_count + 1] = elapsed;

        if (elapsed > threadinfo->max_latency) {
            threadinfo->max_latency = elapsed;
        }

        if (measurement_count == MAX_MEASURES * 2) {   
            printf("Error: demasiadas medidas\n");
            break;
        }    

        total_time += threadinfo->measures[measurement_count + 1];
        measurement_count += 2;
    }
    threadinfo->avg_latency = total_time / (measurement_count / 2);
    printf("[%d] - latencia media = %.0f ns. | max = %.0f\n", *cpu,  
                            threadinfo->avg_latency,  threadinfo->max_latency);

    free(arg);
    pthread_exit((void *) threadinfo);
}


int
main(int argc, char *argv[])
{
    if (argc > 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
    }

    fprintf(stderr,"Starting measurements\n");
    static int32_t latency_target_value = 0;
    int latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    write(latency_target_fd, &latency_target_value, 4);
    
    umask(0);
    csv_file = fopen("cyclictestURJC.csv", "a+");
    if (csv_file == NULL) {
        perror("Error opening CSV file");
        exit(EXIT_FAILURE);
    }

    pthread_t pthread_ids[N];
    
    for(int i = 0; i < N ; i++) {
        int  * cpu = (int *) malloc(sizeof(int));     
        *cpu = i;
        pthread_create(&pthread_ids[i], NULL, thread_actions, (void *) cpu);
    }
    
    double avg_lat_sum = 0, max_total = 0;
    Threadinfo *threadinfo_ptrs[N];

    for(int i = 0; i < N; i++) {
        Threadinfo * threadinfo;
        int z = 0;        
        pthread_join(pthread_ids[i], (void **) &threadinfo);
        threadinfo_ptrs[i] = threadinfo;
    }

    for(int i = 0; i < N; i++) {
        int z = 0; 

        while(threadinfo_ptrs[i]->measures[z + 1] != 0) {
            fprintf(csv_file, "%d, %d, %.5f\n", threadinfo_ptrs[i]->cpu, (int)  
                        threadinfo_ptrs[i]->measures[z],  (threadinfo_ptrs[i]->measures[z + 1] / 1e3 ));
            z += 2;
        }
        avg_lat_sum += threadinfo_ptrs[i]->avg_latency;
        if (max_total < threadinfo_ptrs[i]->max_latency) {
            max_total = threadinfo_ptrs[i]->max_latency;
        }
    }
    
    double latencia_med_total = avg_lat_sum / N;
    printf("\nTotal Latencia media  = %d ns | max = %.0f s.\n", (int) latencia_med_total,  max_total );
    for (int i = 0; i < N; i++) {
        free(threadinfo_ptrs[i]->measures);
        free(threadinfo_ptrs[i]);
    }

    fclose(csv_file);
    return 0;
}

