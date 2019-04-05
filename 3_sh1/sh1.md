# 主要内容

1. 实现shell程序，读取用户输入的命令，并利用mysys执行命令
2. 实现内置命令cd、pwd、exit

# shell基本思路

```
初始化；

输入一行命令；

while 输入的命令不为exit

	根据命令种类进行不同处理；
	输入一行命令；

end while

exit处理；
```

在主函数中的主要工作为对输入的命令字符串进行解析，然后调用相应的处理执行函数

命令目前分三类：

1. 一般命令：直接利用mysys执行
2. cd、pwd等：需在shell程序中实现
3. exit：结束主循环

将一般命令以外的命令都作为特殊命令，定义特殊命令列表

```c
//special command list
char *speCmdList[]={
    "exit",
    "cd",
    "pwd"
};
```

其中`exit`索引为0，便于特殊处理

命令种类解析函数

```c
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
//----------------------------------------------
//返回值为非负数表示在特殊命令中对应的编号（其中0为exit）
//返回值为-1则表示命令按一般形式处理即可
//----------------------------------------------
```

输入的字符串由于可能带有参数以及空格，并不能直接用于命令种类判定，因此解析前需提取命令名称（即输入的字符串按空格划分后的第一个子串）

```c
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
//-----------------------------------------------
//提取出的命令名称存在cmd中
//若输入的字符串为空，直接跳过后面的流程重新输入
//-----------------------------------------------
```

由此进一步得到伪代码	

```c
init();
newCommand();
cmdID = cmdCheck();
while cmdID != 0 :
    switch cmdID :
        case 1:		mycd();
        case 2:		mypwd();
        default:	mysys();
	end switch
    newCommand();
	cmdID = cmdCheck(cmd);
end while
exit();
```

# cd/pwd/exit

由于cd/pwd/exit等命令需要对shell本身的状态进行修改，因此不能采用子进程方式执行，只能内嵌实现

## exit

即结束shell程序，若为异常退出则输出错误信息

## cd

通过系统调用`chdir`能进行工作目录切换

```c
//-------------------------------------------------------------------
//函数名称:	chdir
//函数原型:	int chdir(const char *path); 
//函数功能:	切换到目标目录path
//返回值:	成功返回0，失败返回-1
//-------------------------------------------------------------------
```

**特殊情况**：

当输入为`cd`、`cd ~`、 `cd ~/`时，表示切换到当前用户目录，即`/home/usrname`（其中`usrname`为用户名）

但实际上直接使用`~`或者`~/`作为参数，系统调用并不能正确执行

此时需要使用`passwd`结构体内的`pw_name`成员（头文件`pwd.h`）获取当前用户名，然后自行生成对应目录名称

## pwd

通过系统调用`getcwd`能获取当前工作目录的绝对路径

```c
//-------------------------------------------------------------------
//函数名称:	getcwd
//函数原型:	char *getcwd(char *buf, size_t size);
//函数功能:	将当前工作目录的绝对路径复制到参数buf所指的内存空间中,参数size为buf的空间大小
//返回值:	成功返回指向当前目录的指针，错误返回NULL
//-------------------------------------------------------------------
```

