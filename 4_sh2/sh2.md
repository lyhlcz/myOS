# 主要内容

在sh1.c基础上增加简单重定向功能（即重定向符`>`和`<`）

例：

```
#使用重定向符>将echo命令的输出写入log文件
$>>>echo daffa > log
$>>>cat log
daffa
$>>>echo aaa >log
$>>>cat log
aaa

#使用重定向符<将sh2.c文件作为wc命令的输入
$>>>wc -l sh2.c
201 sh2.c
$>>>wc -l < sh2.c
201
```

**注**：

只考虑单个重定向符，且重定向在输入命令的末尾的情况

即`echo aaa > log1 > log2`  、`echo abc > log def`等输入形式不考虑

所有输入形式均为`待执行命令 重定向符 文件名`

# 解析重定向符

根据上述带有重定向符输入形式的特点，可将输入的串在重定向符处划分为两部分，前半部分用于进行命令名称提取以及其他sh1中的工作，后半部分用于重定向工作

重定向解析在输入命令后、获取命令名称前进行

```c
//new command input
#define newCommand() {\
    do{\
        printf("\033[40;36m$>>>\033[m");\
        fgets(command, 81, stdin);\
        command[strlen(command)-1] = '\0';\
    }while(!command[0]);\
    redirect();\
    strcpy(cmd, command);\
    strcpy(cmd, strtok(cmd, " "));\
}
//redirect为新增的重定向过程
```
定义

```c
#define NO_FILE -1	//文件不存在
#define NO_RE 0		//无重定向
#define RE_OUT 1	//输出重定向
#define RE_IN 2		//输入重定向
```

重定向过程初步代码


```c
int redirect(char* cmd, char* filename){
    int i = 0;
    *(filename) = '\0';
    int fd;

    //get command and filename
    for(;;i++){
        switch (*(cmd+i)){
            case '>':
                *(cmd+i) = '\0';
                if (*(cmd+i+1)){
                    tmp = strtok(cmd+i+1, " ");
                    if (tmp)
                        strcpy(filename, tmp);
                    else *filename = '\0';
                }
                if (!*filename){
                    printf("Error: endline with '>'\n");
                    return NO_FILE;
                }
                printf("redirect stdout\n");
                return RE_OUT;
            case '<':
                *(cmd+i) = '\0';
                if (*(cmd+i+1)){
                    tmp = strtok(cmd+i+1, " ");
                    if (tmp)
                        strcpy(filename, tmp);
                    else *filename = '\0';
                }
                if (!*filename){
                    printf("Error: endline with '<'\n");
                    return NO_FILE;
                }
    			printf("redirect stdin\n");
                return RE_IN;
            case '\0':
                return NO_RE;
        }
    }
}
```

# 输入/输出重定向

重定向工作与命令自身执行的过程无关，但必须在命令执行前完成，因此可在解析重定向符后直接进行输入/输出重定向

## 输出重定向

输出重定向符为`>`，作用为将命令执行产生的输出按刷新的方式写入（即每次都相当于重写一个全新的文件）到重定向的文件

若文件不存在会创建一个新文件

```c
fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, 0666);
*tfd = dup(1);
dup2(fd, 1); //redirect stdout
close(fd);
```

## 输入重定向

输入重定向符为`<`，作用为将文件内容作为命令执行要输入的参数

若文件不存在会报错`No such file or directory`

```c
fd = open(filename, O_RDONLY);
if (fd < 0){
    return NO_FILE;
}
*tfd = dup(0);
dup2(fd, 0); //redirect stdin
close(fd);
```

# 恢复输入/输出

当次命令结束后需将文件描述符0/1/2重新定向到对应的特殊“文件”，这里设定文件描述符`tfd`充当容器，在输入输出重定向时将stdin/stdout“暂存”于该容器中；在恢复输入/输出时再次使用重定向将文件描述符定向到该容器中的内容

重定向过程修改为

```c
int redirect(char* cmd, char* filename, int* tfd){
    int i = 0;
    *(filename) = '\0';
    int fd;

    //get command and filename
    for(;;i++){
        switch (*(cmd+i)){
            case '>':
                *(cmd+i) = '\0';
                if (*(cmd+i+1)){
                    tmp = strtok(cmd+i+1, " ");
                    if (tmp)
                        strcpy(filename, tmp);
                    else *filename = '\0';
                }
                if (!*filename){
                    printf("Error: endline with '>'\n");
                    return NO_FILE;
                }
                //----------------更新内容-------------------
                fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, 0666);
                *tfd = dup(1);	//将tfd指向stdout
                dup2(fd, 1); //redirect stdout
                close(fd);
                //-----------------------------------------
                return RE_OUT;
            case '<':
                *(cmd+i) = '\0';
                if (*(cmd+i+1)){
                    tmp = strtok(cmd+i+1, " ");
                    if (tmp)
                        strcpy(filename, tmp);
                    else *filename = '\0';
                }
                if (!*filename){
                    printf("Error: endline with '<'\n");
                    return NO_FILE;
                }
                //---------------更新内容-------------------
                fd = open(filename, O_RDONLY);
                if (fd < 0){
                    return NO_FILE;
                }
                *tfd = dup(0);	//将tfd指向stdin
                dup2(fd, 0); //redirect stdin
                close(fd);
                //-----------------------------------------
                return RE_IN;
            case '\0':
                return NO_RE;
        }
    }
}
```

恢复输入/输出

```c
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
```

至此整个shell的流程更新为

```c
init();
newCommand();	//新增了redirect过程
cmdID = cmdCheck();
while(cmdID != 0){
    //若为NO_FILE，跳过命令执行过程
    if (rdFlag == NO_FILE){
    	is_NO_FILE();	//处理NO_FILE情况
    }
    else{
        switch (cmdID){
            case 1:		mycd();break;
            case 2:		mypwd();break;
            default:	mysys();
        }
    }
    unredirect();	//恢复输入/输出
    newCommand();	//新增了redirect过程
    cmdID = cmdCheck(cmd);
}
exit();
```

