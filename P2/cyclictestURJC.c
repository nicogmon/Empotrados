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
#define MAX_MEDIDAS 10000000
#define USLEEP_TMP 50000
double medias[20] = {0};
double max_latencias[21] = {0};
FILE *csv_file;


struct Threadinfo{
    double latencia_media;
    double latencia_max;
    double * medidas;
    int cpu;

};typedef struct Threadinfo Threadinfo;

Threadinfo *threadinfo_ptrs[30];

void * thread_actions(void * arg){
    int num_medidas = 0;
    int iteracion = 0;
    int  * cpu = (int *) arg;
    double tmp_total = 0;
    double elapsed;

    Threadinfo * threadinfo = (Threadinfo *) malloc(sizeof(Threadinfo));
    threadinfo->medidas = (double *) malloc(sizeof(double) * MAX_MEDIDAS * 2);
    memset(threadinfo->medidas, 0, MAX_MEDIDAS * sizeof(double));
    threadinfo->latencia_media = 0;
    threadinfo->latencia_max = 0;

    struct timespec start_time, current_time, timer_start, timer_end;

    int result = 0;
    cpu_set_t cpuset;
    pthread_t thread = pthread_self();

    /* Set affinity mask to include CPUs 0 to 7 */

    CPU_ZERO(&cpuset);
    
    CPU_SET(*cpu, &cpuset);

    result = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    if (result != 0){
        printf("Error: pthread_setaffinity_np\n");
    }    
    

    struct sched_param param = {0};
    param.sched_priority = 99; // Prioridad 99
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0) {
        perror("Error: pthread_setschedparam");
    }

    clock_gettime(CLOCK_MONOTONIC, &timer_start);
    current_time = timer_start;

    //printf("Thread %d\n", *cpu);
    threadinfo->cpu = *cpu;
    while((current_time.tv_sec - timer_start.tv_sec) < 60){
        
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        usleep(USLEEP_TMP);
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed = (current_time.tv_sec  + (current_time.tv_nsec / 1e9)) - (start_time.tv_sec +  (start_time.tv_nsec / 1e9)) - (USLEEP_TMP/1e6);
        elapsed = elapsed * 1e9;
        //printf("elapsed = %.9f\n", elapsed);
        iteracion = num_medidas/2;
        threadinfo->medidas [num_medidas] = iteracion;
        threadinfo->medidas [num_medidas + 1] = elapsed;

        if (elapsed > threadinfo->latencia_max){
            threadinfo->latencia_max = elapsed;
        }

        if (num_medidas == MAX_MEDIDAS * 2){   
            printf("Error: demasiadas medidas\n");
            break;
        }

        //fprintf(stderr, "latencia %d = %.9f s.\n", num_medidas, medidas[num_medidas]);
        //fprintf(stderr, "tiempo total %ld\n", (currentime.tv_sec - timer_start.tv_sec));
        tmp_total += threadinfo->medidas[num_medidas + 1];
        num_medidas += 2;

    }
    threadinfo->latencia_media = tmp_total / (num_medidas / 2);
    printf("[%d] - latencia media = %.0f ns. | max = %.0f\n", *cpu,  threadinfo->latencia_media,  threadinfo->latencia_max);

    //medias[*cpu] = tmp_total / num_medidas;
    //printf("medias en thread [%d] = %.9f\n", *cpu, medias[*cpu]);
    //max_latencias[*cpu] = max_latencia;
    free(arg);
    pthread_exit((void *) threadinfo);
}


int
main(int argc, char *argv[])
{

    if (argc > 1){
        fprintf(stderr, "Usage: %s\n", argv[0]);
    }
    fprintf(stderr,"Starting measurements\n");
    static int32_t latency_target_value = 0;
    int latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    write(latency_target_fd, &latency_target_value, 4);
    
    
    umask(0);
    csv_file = fopen("cyclictestURJC.csv", "a+");
    if (csv_file == NULL) {
        perror("Error al abrir el archivo CSV");
        exit(EXIT_FAILURE);
    }

    pthread_t pthread_ids[N];
    
    for(int i = 0; i < N ; i++){
        int  * cpu = (int *) malloc(sizeof(int));     
        *cpu = i;
        //printf("Creating thread %d\n", *cpu);

        pthread_create(&pthread_ids[i], NULL, thread_actions, (void *) cpu);
    }
    double latencia_med_sum = 0;
    double max_total = 0;
    double latencia_med_total = 0;
    for(int i = 0; i < N; i++){
        Threadinfo * threadinfo;
        int z = 0;        
        pthread_join(pthread_ids[i], (void **) &threadinfo);
        threadinfo_ptrs[i] = threadinfo;
        
        //free(threadinfo->medidas);
        //free(threadinfo);
    }

    
    for(int i = 0; i < N; i++){
        int z = 0; 
        //fprintf(stderr,"iteracion %d\n", i);
        //fprintf(stderr, "cpu %d\n", threadinfo_ptrs[i]->cpu);
        //Escribe las medidas en el archivo CSV
        //fprintf(csv_file, "cpu, iteracion, latencia\n");
        while(threadinfo_ptrs[i]->medidas[z+1] != 0){
            fprintf(csv_file, "%d, %d, %.5f\n", threadinfo_ptrs[i]->cpu, (int)  threadinfo_ptrs[i]->medidas[z],  (threadinfo_ptrs[i]->medidas[z+1] / 1e3 ));
            z += 2;
        }
        latencia_med_sum += threadinfo_ptrs[i]->latencia_media;
        if (max_total < threadinfo_ptrs[i]->latencia_max){
            max_total = threadinfo_ptrs[i]->latencia_max;
        }
        
    }
    
    latencia_med_total = latencia_med_sum / N;
    printf("\nTotal Latencia media  = %d ns | max = %.0f s.\n", (int) latencia_med_total,  max_total );
    for (int i = 0; i < N; i++){
        free(threadinfo_ptrs[i]->medidas);
        free(threadinfo_ptrs[i]);
    }
    // Cierra el archivo CSV
    fclose(csv_file);
}

