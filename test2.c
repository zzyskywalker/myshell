#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXARGS 128   /* 最大命令数量*/
#define MAXLINE 1024   /* 每条命令的最大长度*/
/* 全局变量 */
int argc;      
char* a[5]; 



void excutecmd(char* argv[MAXARGS]){
	
	int pid;

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {		/* child */
		execvp(argv[0], argv);
        printf("11111");
		err_ret("couldn't execute: %s", argv[0]);
		exit(127);
	}

	/* parent */
	if ((pid = waitpid(pid, NULL, 0)) < 0)
		err_sys("waitpid error");
	printf("%%\n ");
	
}

int main(){
    char* a[5]; 
    a[0] = "ls";
    a[1] = "-a";
    a[2] = NULL;
    a[3] = NULL;

    excutecmd(a);
    int i;
    for(i=0;i<5;i++){

        printf("%p\n",a[i]);

    };

	int fd;
	printf("fd%d\n",fd);
	char tem[1]="1";

	fd = tem[0]-'0';
	printf("%d\n",fd);
}