#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>

int mycat(char* filename){
        int f1, size;
        char buffer[81];

        f1 = open(filename, O_RDONLY);
        if (f1 < 0){
                close(f1);
                return -1;
        }

        while((size = read(f1, buffer, 80)) > 0){
                buffer[size] = '\0';
                printf("%s", (char*)buffer);
        }

        close(f1);

        return 0;
}

int main(int argc, char* args[]){
        mycat(args[1]);
        return 0;
}