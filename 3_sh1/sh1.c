#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <pwd.h>
#include "mysys.c"

#define MAX_PATH 260

//new command input
#define newCommand() {\
	do{\
		printf("\033[40;36m$>>>\033[m");\
		fgets(command, 81, stdin);\
		command[strlen(command)-1] = '\0';\
	}while(!command[0]);\
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

int main(){
	//init
	int exitFlag = 0;
	char message[81] = "";
	char command[81];
	char cmd[81];
	int cmdID;
	
	//welcome
	printf("\033[47;34m-----------------------------------------------------------\033[m\n");
	printf("\033[47;34m|                welcome to lyh's shell                   |\033[m\n");
	printf("\033[47;34m-----------------------------------------------------------\033[m\n");

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
	
	//exit
	exitShell(exitFlag, message);
	
	return 0;
}
