#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#define MAX_PROCESSES 10000

FILE *file;

int main(void){
    char path[20];

    int i = 0, buf;

    pid_t pid = getpid(), child = 0;
    pid_t pid_array[MAX_PROCESSES];

    struct timespec startTime, endTime;
    double t;

    //setting priority
    struct sched_param param;

    // int set_priority = sched_get_priority_min(SCHED_FIFO);
    // printf("priority = %d\n", set_priority);
    // param.__sched_priority = set_priority;

    //cd temp
    chdir("temp");

    //start time store
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    //make MAX_PROCESSES process
    for(; i < MAX_PROCESSES && pid > 0; i++){
        if((pid = fork()) < 0)	printf("fork error\n");
        if(pid > 0) {
            // if(sched_setscheduler(pid, SCHED_FIFO, &param) < 0)   printf("sched error\n");
            // nice(-20);	//nice = [-20, 20)
            pid_array[i] = pid;
        }
    }

    //child process, read each index's file
    if(pid == 0){
        sprintf(path, "%d.txt", i-1);
        file = fopen(path, "r");
        fscanf(file, "%d", &buf);
        fclose(file);
    //parent process, wait i(MAX_PROCESSES) number of process
    } else{
        for(int j = 0; j < i; j++){
            waitpid(pid_array[j], NULL, 0);
        }
	//end time store
        clock_gettime(CLOCK_MONOTONIC, &endTime);
        t = (endTime.tv_sec - startTime.tv_sec) + (double)(endTime.tv_nsec - startTime.tv_nsec) / 1000000000;
        printf("%lf\n", t);
    }

    return 0;
}
