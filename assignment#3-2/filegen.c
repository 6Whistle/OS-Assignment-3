#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#define MAX_PROCESSES 10000

int main(void){
    char path[20];
    srand((unsigned int)time(NULL));
    mkdir("temp", 0755);
    chdir("temp");
    for(int i = 0; i < MAX_PROCESSES; i++){
        sprintf(path, "%d.txt", i);
        FILE *file = fopen(path, "w");
        fprintf(file, "%d", 1 + rand() % 9);
        fclose(file);
    }

    return 0;
}