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
                args[index++]=p;
                p = strtok_r(saveptr, token, &saveptr);
        }
        args[index] = NULL;
}

void mysys(char* command){
        pid_t pid;

        pid = fork();
        if (pid == 0){
                //child process
                char* args[21];
                int index = 0;
                char* p;
                char* saveptr;
                char* str = (char*)malloc(sizeof(command));

                strcpy(str, command);

                //get args
                split(str, " ", args);
                /*
                p = strtok_r(str, " ", &saveptr);
                while (p){
                        args[index++]=p;
                        //args[index]=(char*)malloc(sizeof(p));
                        //strcpy(args[index++], p);

                        //printf("%s\n", args[index-1]);

                        p = strtok_r(saveptr, " ", &saveptr);
                }
                args[index] = NULL;
                */

                //exec command
                execvp(args[0], args);
                //exit
                exit(0);
        }
        else{
                //parent process
                int status;
                wait(&status);
                if (!WIFEXITED(status)) {
                        printf("child process error!\n");
                }
        }

        return;
}

int main(){

        printf("----------------------------------\n");
        mysys("echo HELLO WORLD");
        printf("----------------------------------\n");
        mysys("ls /");
        printf("----------------------------------\n");
        return 0;
}