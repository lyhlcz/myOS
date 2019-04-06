#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

void split(char* str, char* token, char* args[]){
    //get args
    int index=0;
    char* p;
    char* saveptr;
    p = strtok_r(str, token, &saveptr);
    while (p){
        args[index++] = p;
        p = strtok_r(saveptr, token, &saveptr);
    }
    args[index] = NULL;
}

int mysys(char* command){
    pid_t pid;

    pid = fork();
    if (pid == 0){
        //child process
        char* args[21];
        int index = 0;
        char* p;
        char* str = (char*)malloc(sizeof(command));
        int exeFlag;

        strcpy(str, command);

        //get args
        split(str, " ", args);
        //exec command
        exeFlag = execvp(args[0], args);
        //printf("exe flag is %d\n", exeFlag);
        //exit
        exit(exeFlag);
    }
    else{
        //parent process
        int status;
        wait(&status);
        //printf("status is %d\n", status);
        if (!WIFEXITED(status)) {
            printf("child process error!\n");
            return -1;
        }
        return status;
    }
}

int main(){
	printf("----------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("----------------------------------\n");
    mysys("ls /");
    printf("----------------------------------\n");
    return 0;
}