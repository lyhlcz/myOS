#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include "mysys.c"

#define MAX_PATH 260

#define NO_FILE -1
#define NO_RE 0
#define RE_OUT 1
#define RE_IN 2

#define PP_ERROR -1
#define NO_PP 0
#define PP_WRITE 1
#define PP_READ 2

//new command input
#define newCommand() {\
    do{\
        printf("\033[40;36m$>>>\033[m");\
        fgets(command, 81, stdin);\
        command[strlen(command)-1] = '\0';\
    }while(!command[0]);\
    rdFlag = redirect(command, filename, &tfd);\
    if (rdFlag == NO_RE)\
        ppFlag = pipeCheck(command, command2, fd);\
    strcpy(cmd, command);\
    strcpy(cmd, strtok(cmd, " "));\
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

void mycd(char* command){
    char* sp;
    char* tmp;
    char tmp2[101];
    struct passwd* pw = NULL;

    pw = getpwuid(getuid());
    if (pw){
        //get homepath
        char home[81] = "/home/";
        strcat(home, pw->pw_name);
        //get command path
        strtok_r(command, " ", &sp);
        tmp = strtok_r(sp, " ", &sp);

        if (tmp&&strlen(tmp)){
            strcpy(tmp2, tmp);
            if ((sp = strchr(tmp2, '~'))&&*(sp+1)!='~'){
                *sp = '\0';
                strcat(tmp2, home);
                strcat(tmp2, tmp+(sp-tmp2)+1);
            }
            if (chdir(tmp2)){
                printf("shell: cd: %s: No such file or directory\n", tmp2);
            }
        }
        else{ //go to homepath
            chdir(home);
        }
    }
    else {  //error
        printf("Username Error!\n");
    }

    return;
}

void mypwd(){
    char path[MAX_PATH+1];
    //printf("it's pwd\n");
    getcwd(path, MAX_PATH);
    printf("%s\n", path);
    return;
}

int redirect(char* cmd, char* filename, int* tfd){
    int i = 0;
    *filename = '\0';
    int fd;
    char* tmp;

    //get command and filename
    for(;;i++){
        switch (*(cmd+i)){
            case '>':
                *(cmd+i) = '\0';
                if (*(cmd+i+1)){
                    //strcpy(filename, cmd+i+1);
                    tmp = strtok(cmd+i+1, " ");
                    if (tmp)
                        strcpy(filename, tmp);
                    else *filename = '\0';
                }
                if (!*filename){
                    printf("Error: endline with '>'\n");
                    return NO_FILE;
                }
                fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, 0666);
                *tfd = dup(1);
                dup2(fd, 1); //redirect stdout
                close(fd);
                return RE_OUT;
            case '<':
                *(cmd+i) = '\0';
                if (*(cmd+i+1)){
                    //strcpy(filename, cmd+i+1);
                    tmp = strtok(cmd+i+1, " ");
                    if (tmp)
                        strcpy(filename, tmp);
                    else *filename = '\0';
                }
                if (!*filename){
                    printf("Error: endline with '<'\n");
                    return NO_FILE;
                }
                fd = open(filename, O_RDONLY);
                if (fd < 0){
                    return NO_FILE;
                }
                *tfd = dup(0);
                dup2(fd, 0); //redirect stdin
                close(fd);
                return RE_IN;
            case '\0':
                return NO_RE;
        }
    }
}

void unredirect(int flag, int tfd){
    switch (flag){
        case RE_OUT:
            close(1);
            dup2(tfd, 1);
            close(tfd);
            break;
        case RE_IN:
            close(0);
            dup2(tfd, 0);
            close(tfd);
            break;
    }
}

int pipeCheck(char* cmd, char* cmd2, int* fd){
    int i = 0;
    *cmd2 = '\0';
    char* tmp;

    for(;;i++){
        switch (*(cmd+i)){
            case '|':
                *(cmd+i) = '\0';
                if (*(cmd+i+1)){
                    //tmp = strtok(cmd+i+1, " ");
                    tmp = cmd+i+1;
                    if (tmp)
                        strcpy(cmd2, tmp);
                    else *cmd2 = '\0';
                }
                if (!*cmd2){
                    printf("Error: endline with '|'\n");
                    return NO_PP;
                }
                if (pipe(fd))
                    return PP_ERROR;
                else
                    return PP_WRITE;
            case '\0':  return NO_PP;
        }
    }
}

int main(){
    //init
    int exitFlag = 0;
    char message[81] = "";
    char command[81];
    char cmd[81];
    int cmdID;

    char filename[81];
    int rdFlag = NO_RE; //redirect flag
    int tfd;    //tmp fd

    int fd[2]; //pipe fd
    int ppFlag = NO_PP; //pipe flag
    char command2[81];

    //welcome
    printf("\033[47;34m-----------------------------------------------------------\033[m\n");
    printf("\033[47;34m|                welcome to lyh's shell |\033[m\n");
    printf("\033[47;34m-----------------------------------------------------------\033[m\n");

    //main cycle
    newCommand();
    while(cmdID = cmdCheck(cmd)){  //cmdID = 0 -> exit
        switch (ppFlag){
            case PP_ERROR:  continue;
            case PP_WRITE:
                tfd = dup(1);   //save stdout
                dup2(fd[1], 1);
                close(fd[1]);
                break;
            case PP_READ:
                tfd = dup(0);   //save stdin
                dup2(fd[0], 0);
                close(fd[0]);
                break;
        }

        if (rdFlag == NO_FILE){
            if (*filename)
                printf("shell: %s: No such file or directory\n", filename);
        }
        else{
            switch(cmdID){
                case 1: //cd
                    mycd(command);
                    break;
                case 2: //pwd
                    mypwd();
                    break;
                default: //common cmd
                    //printf("%d   %s\n",getpid(),command);
                    if(mysys(command) == 65280){    //(-1)&0xFF00 = 65280
                        printf("shell: %s: command not found!\n", cmd);
                    }
                    //else printf("%d   %s\n",getpid(), cmd);
            }
        }

        if (ppFlag == PP_WRITE){
            dup2(tfd, 1);
            strcpy(command, command2);
            strcpy(cmd, command);
            strcpy(cmd, strtok(cmd, " "));
            ppFlag = PP_READ;
            continue;   //if flag is PP_WRITE, use command2 as the next cmd
        }
        else if (ppFlag == PP_READ){
            dup2(tfd, 0);
        }
        else
            unredirect(rdFlag, tfd);

        rdFlag = NO_RE;
        ppFlag = NO_PP;

        newCommand();
    }

    //exit
    exitShell(exitFlag, message);

    return 0;
}