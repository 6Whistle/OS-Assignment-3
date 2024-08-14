#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROCESSES 64

//thread func.
void *pthread_ex(void *arg){
    int start = ((int *)arg)[0], end = ((int *)arg)[1];
    int data[2];
    int *ret = (int *)malloc(sizeof(int));	//return value

    //if only 2 number, read file
    if(end - start == 2){
        FILE *file = fopen("temp.txt", "r");
        for(int i = 0; i < start / 2 + 1; i++) fscanf(file, "%d %d", &(data[0]), &(data[1]));
        *ret = data[0] + data[1];
        return (void *)ret;
    }
    
    //if more than 2 number
    int t_id[2];
    pthread_t thread[2];
    int div = (end - start) / 2 + start;
    int point1[2] = {start, div}, point2[2] = {div, end};
    int *status1, *status2;

    //make 2 child thread
    t_id[0] = pthread_create(&(thread[0]), NULL, pthread_ex, (void *)point1);
    if(t_id[0] < 0)    printf("thread error\n");

    t_id[1] = pthread_create(&(thread[1]), NULL, pthread_ex, (void *)point2);
    if(t_id[1] < 0)     printf("thread error\n");

    //wait child thread
    pthread_join(thread[0], (void **)&status1);
    pthread_join(thread[1], (void **)&status2);

    //calculate
    *ret = *status1 + *status2;
    free(status1);
    free(status2);

    return (void *)ret;
}

int main(void){
    int t_id, *status;
    int point[2] = { 0, MAX_PROCESSES * 2 };
    pthread_t thread;

    struct timespec startTime, endTime;
    double t;

    //start time store
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    
    //make 1 thread
    t_id = pthread_create(&thread, NULL, pthread_ex, (void *)point);
    if(t_id < 0){
        printf("thread error\n");
        return -1;
    }

    //wait thread end
    pthread_join(thread, (void **)&status);

    //end time store
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    t = (endTime.tv_sec - startTime.tv_sec) + (double)(endTime.tv_nsec - startTime.tv_nsec) / 1000000000;
    printf("value of thread : %d\n", *status);
    printf("%lf\n", t);
    free(status);

    return 0;
}
