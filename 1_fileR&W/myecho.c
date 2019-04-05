#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>

int main(int argc, char* args[]){
        /*
        int an = sizeof(args)/sizeof(char*);
        printf("an = %d %lu %lu\n", an,sizeof(args),sizeof(char*));
        int i = 1;
        for(; i<an; i++){
                printf("%s", args[i]);
        }*/
        int i=1;
        int n_flag = 1;
        if(args[1]){
                if(!strcmp(args[1], "-n")){
                        i++;
                        n_flag = 0;
                };
                if(args[i]){
                        printf("%s", args[i++]);
                        while(args[i]){
                                printf(" %s", args[i++]);
                        }
                }
        }

        if(n_flag){
                printf("\n");
        }
        return 0;
}