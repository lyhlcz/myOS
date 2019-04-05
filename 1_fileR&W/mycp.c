#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>

int mycp(char* sourcefile, char* newfile){
        printf("sourcefile:%s\nnewfile:%s\n",  sourcefile, newfile);
        int f1, f2, size;
		char buffer[81];

        f1 = open(sourcefile, O_RDONLY);
        if (f1 < 0){
                close(f1);
                return -1;
        }
        f2 = open(newfile, O_CREAT | O_WRONLY);
        if (f2 < 0){
                close(f2);
                close(f1);
                return -1;
        }

        while((size = read(f1, buffer, 80)) > 0){
                printf("%d\n", size);
                write(f2, buffer, size);
        }

        close(f1);
        close(f2);

        return 0;
}

int main(int argc, char* args[]){
        mycp(args[1], args[2]);
        return 0;
}