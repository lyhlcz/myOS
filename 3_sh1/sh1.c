#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysys.c"

//new command input
#define newCommand() {\
    do{\
        printf("\033[40;36m$>>>\033[m");\
        fgets(command, 81, stdin);\
        command[strlen(command)-1] = '\0';\
    }while(!command[0]);\
    strcpy(cmd, command);\
    strtok(cmd, " ");\
}

//special command list
char *speCmdList[]={
    "exit",
    "cd",
    "pwd"
};

//check command is special or common
int cmdCheck(char* cmd){
    int n = sizeof(speCmdList)/sizeof(char*);
    int i = 0;
    for(;i<n;i++){
        if(!strcmp(cmd, *(speCmdList+i))){
            return i;
        }
    }
    return -1;
}

void exitShell(int flag, char* m){
        if (flag==0){
                printf("exit()\n");
        }
        else if (m){
                printf("\033[47;31mExit Error:\t%s\033[m\n", m);
        }
        else{
                printf("\033[47;31mUnkown exit error !\033[m\n");
        }
}

void mycd(char* cmd){
    printf("it's cd\n");
}

void mypwd(){
    printf("it's pwd\n");
}

int main(){
        int exitFlag = 0;
        char message[81] = "";
        char command[81];
    char cmd[81];
    int cmdID;

        //welcome
        printf("\033[47;34m-----------------------------------------------------------\033[m\n");
        printf("\033[47;34m|                welcome to lyh's shell     |\033[m\n");
        printf("\033[47;34m-----------------------------------------------------------\033[m\n");

        //init

        //main cycle
        newCommand();
    while(cmdID = cmdCheck(cmd)){  //cmdID = 0 -> exit
        switch(cmdID){
            case 1: //cd
                mycd(command);
                break;
            case 2: //pwd
                mypwd();
                break;
            default: //common cmd
                //printf("%s\n",command);
                if(mysys(command) == 65280){    //(-1)&0xFF00 = 65280
                    printf("shell: %s: command not found!\n", cmd);
                }
        }
        newCommand();
    }
    /*
    while(strcmp(command, "exit")){
        if(mysys(command) == 65280){    //(-1)&0xFF00 = 65280
            printf("shell: %s: command not found!\n", strtok(command, " "));
        }
        newCommand();
        }*/

    //exit
	exitShell(exitFlag, message);

	return 0;
}