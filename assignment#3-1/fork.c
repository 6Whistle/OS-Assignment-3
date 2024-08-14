#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_PROCESSES 64

//file pointer
FILE *file;

int main(void){
    //point = [start, end)
    int start = 0, end = MAX_PROCESSES * 2, resultState, leftState, rightState;
    
    pid_t pid, child1, child2;
    struct timespec startTime, endTime;
    double t;

    //start time store
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    //make child process
    if((pid = fork()) < 0)	printf("fork error\n");

    //child process
    if(pid == 0){
	//while point is currect
        while(end - start > 1){
            //if only 2 number, read file
            if(end - start == 2){
                file = fopen("temp.txt", "r");
                for(int i = 0; i < start / 2 + 1; i++)  fscanf(file, "%d %d", &leftState, &rightState);
                fclose(file);
                exit(leftState + rightState);
            }

	    //if more than 2 number, divide point
            int div = (end - start) / 2 + start;
            
	    //Make 2 child process
	    if((child1 = fork()) < 0){
                printf("fork error\n");
                return -1;
            }
            if(child1 > 0){
                if((child2 = fork()) < 0){
                    printf("fork error\n");
                    return -1;
                }
            }

	    //[start ~ div)
            if(child1 == 0){
                end = div;
	    //[div ~ end)
            }else if(child2 == 0){
                start = div;
            //2 process's parent
	    }else{
                waitpid(child1, &leftState, 0);
                waitpid(child2, &rightState, 0);
                exit(WEXITSTATUS(leftState) + WEXITSTATUS(rightState));
            }
        }
    }
    else if(pid > 0){
	//wait result
        waitpid(pid, &resultState, 0);
        //end time store
	clock_gettime(CLOCK_MONOTONIC, &endTime);
        t = (endTime.tv_sec - startTime.tv_sec) + (double)(endTime.tv_nsec - startTime.tv_nsec) / 1000000000;
        printf("value of fork : %d\n", WEXITSTATUS(resultState));
        printf("%lf\n", t);
    }

    return 0;
}
