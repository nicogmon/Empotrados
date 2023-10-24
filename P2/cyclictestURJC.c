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

#define N ((int) sysconf(_SC_NPROCESSORS_ONLN))
#define MAX_MEDIDAS 100000
#define USLEEP_TMP 5000

FILE *csv_file;

void * thread_actions(void * arg){
    int num_medidas = 0;
    int iteracion = 0;
    int  * cpu = (int *) arg;
    double  * medidas;
    medidas = (double *) malloc(sizeof(double) * MAX_MEDIDAS);
    memset(medidas, 0, MAX_MEDIDAS * sizeof(double));
    double tmp_total = 0;
    double elapsed;
    struct timespec start_time, current_time, timer_start, timer_end;

    int s;
    cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();

    /* Set affinity mask to include CPUs 0 to 7 */

    CPU_ZERO(&cpuset);
    
    CPU_SET(*cpu, &cpuset);

    s = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    if (s != 0)
        printf("Error: pthread_setaffinity_np\n");
    /*s = pthread_getaffinity_np(thread, sizeof(cpuset), &cpuset);
    if (s != 0)
        printf("Error: pthread_getaffinity_np\n");*/

    struct sched_param param = {0};
    param.sched_priority = 99; // Prioridad 99
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0) {
        perror("Error: pthread_setschedparam");
    }

    clock_gettime(CLOCK_MONOTONIC, &timer_start);
    current_time = timer_start;

    printf("Thread %d\n", *cpu);
    while((current_time.tv_sec - timer_start.tv_sec) < 60){
        
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        usleep(USLEEP_TMP);
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed = (current_time.tv_sec  + (current_time.tv_nsec / 1e9)) - (start_time.tv_sec +  (start_time.tv_nsec / 1e9)) - (USLEEP_TMP/1e6);
        //printf("elapsed = %.9f\n", elapsed);
        iteracion = num_medidas/3;
        medidas [num_medidas] = *cpu;
        medidas [num_medidas + 1] = iteracion;
        medidas [num_medidas + 2] = elapsed;

        if (num_medidas == MAX_MEDIDAS){   
            printf("Error: demasiadas medidas\n");
            break;
        }
        //fprintf(stderr, "latencia %d = %.9f s.\n", num_medidas, medidas[num_medidas]);
        //fprintf(stderr, "tiempo total %ld\n", (currentime.tv_sec - timer_start.tv_sec));
        tmp_total += medidas[num_medidas + 2];
        num_medidas += 3;
        
    }
    
    
    

    printf("[%d] - latencia media = %.9f s.\n", *cpu, tmp_total / num_medidas);
    free(arg);
    pthread_exit((void *) medidas);
}


int
main(int argc, char *argv[])
{

    if (argc > 1){
        fprintf(stderr, "Usage: %s\n", argv[0]);
    }
    static int32_t latency_target_value = 0;
    int latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    write(latency_target_fd, &latency_target_value, 4);
    
    csv_file = fopen("cyclictestURJC.csv", "w");
    if (csv_file == NULL) {
        perror("Error al abrir el archivo CSV");
        exit(EXIT_FAILURE);
    }

    pthread_t pthread_ids[N];
    
    for(int i = 0; i < N ; i++){
        int  * cpu = (int *) malloc(sizeof(int));     
        *cpu = i;
        printf("Creating thread %d\n", *cpu);

        pthread_create(&pthread_ids[i], NULL, thread_actions, (void *) cpu);
    }

    for(int i = 0; i < N; i++){
        double * medidas;
        int z = 0;
        pthread_join(pthread_ids[i], (void **) &medidas);
        //Escribe las medidas en el archivo CSV
        while(medidas[z] != 0){
            fprintf(csv_file, "%d, %d, %.9f\n", (int) medidas[0], (int)  medidas[z+1], medidas[z+2]);
            z += 3;
        }
        free(medidas);
        
    }

    // Cierra el archivo CSV
    fclose(csv_file);
}

