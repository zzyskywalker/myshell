#include <stdio.h>
#include <stdlib.h>
#include "apue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/*命令提示符的形式*/
char command_prompt[] = "->";

#define MAXARGS 128   /* 最大命令数量*/
#define MAXLINE 1024   /* 每条命令的最大长度*/

/* 全局变量 */
int argc;      
char* argv[MAXARGS];      

/*标识符，每次需要重新初始化*/
int out_redirect;   /*标识是否输出重定向*/
int in_redirect;	/*标识是否输入重定向*/



void splitcmd(char *buf);
void excutecmd();
void background();
void foreground();
void input_redirect();
void output_redirect();
void in_out_redirect();
int  is_redirect();
int output_redirect2(char** backargv);

int
main(void)
{
	char	buf[MAXLINE];
	pid_t	pid;
	int		status;

	if (fputs(command_prompt, stdout) == EOF){
		err_sys("output error");
	};
	
	
	while (fgets(buf, MAXLINE, stdin) != NULL){
		buf[strlen(buf) - 1] = '\0';/*将最后一位换行符修改*/
		if (strcmp(buf,"exit") == 0)
			exit(0);

		out_redirect = 0;
		in_redirect = 0;

		splitcmd(buf);	/*在这里将命令行中的输入切分成了命令数组*/
		is_redirect();
		printf("%d,%d\n",out_redirect,in_redirect);
		excutecmd();
		/*
		is_redirect();

		if(strcmp(argv[0],"bg") == 0){

			background();

		}else{
			
			
			foreground();
		};
			*/
		if (fputs(command_prompt, stdout) == EOF)
			err_sys("output error");

	};
	

	if (ferror(stdin))
		err_sys("input error");

	exit(0);
}

void splitcmd(char *buf){

	
	
	char array[MAXLINE];   
	char *backbuf = array;
	char *delim;                /*储存分隔符的位置比如空格、斜杠*/
	int bg;                     /* background job */
	buf[strlen(buf)] = ' ';  /*上面有个把\n换成\0的步骤，
	所以在这里计算长度的时候要注意*/
	
	strcpy(backbuf,buf); 
	while (*backbuf && (*backbuf == ' ')) /* ignore leading spaces */
		backbuf++;


	argc = 0;
	/*构造命令数组，以单引号和空格分隔*/
	if (*backbuf == '\'') {
		backbuf++;
		delim = strchr(backbuf, '\'');/*如果是以单引号开头的话，buf++相当于略过，然后找到下一个单引号的位置*/
	}
	else {
		delim = strchr(backbuf, ' ');
	}
	
	while (delim) {
		argv[argc++] = backbuf;   /*将字符数组赋给argv的第几个元素，然后用\0截断，这样就只保留两个\0之间的字符*/
		*delim = '\0';
		backbuf = delim + 1;
		while (*backbuf && (*backbuf == ' ')) /* ignore spaces */
			backbuf++;
		
		if (*backbuf == '\'') {
			backbuf++;
			delim = strchr(backbuf, '\'');
		}
		else {
			delim = strchr(backbuf, ' ');
		}
	}
	argv[argc] = NULL;
	/*
	int i;
	for( i = 0;i<argc;i++){
		printf("%s\n",argv[i]);
	};
	*/

}


void excutecmd(){
	
	int pid;
	char* backargv[argc+1];
	int fd;
	int outredirect_sign ;
	int inredirect_sign ;
	int i;
	for(i=0;i<argc;i++){
		int length = strlen(argv[i]);
		backargv[i] = (char *)malloc(length);
		strcpy(backargv[i],argv[i]);

		printf("%s\n",backargv[i]);
	};
	

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {		/* child */
		if(out_redirect){
			printf("out_redirect\n");
			

		}
		output_redirect2(&backargv); 
		execvp(backargv[0], backargv);
    	if (fd != STDIN_FILENO) {
                close(fd);
            }
		err_ret("couldn't execute: %s", backargv[0]);
		exit(127);


	}

	/* parent */
	if ((pid = waitpid(pid, NULL, 0)) < 0)
		err_sys("waitpid error");
	
	
	/*printf("%s finished\n",parameter[0]);*/
	int j;
	for(j=0;j<argc;j++){

		free(backargv[j]);
		backargv[j] = NULL;
	}

}

int is_redirect(){
	int i;
	for(i=0;i<argc;i++){

		if( in_redirect = (strcmp(argv[i],"<")==0) )
			break;

	};

	for(i=0;i<argc;i++){

	if( out_redirect = (strcmp(argv[i],">")==0) )
		break;

	};

	return(0);

}

void background(){

    int pid;
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {		/* first child */
		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid > 0)
			exit(0);	/* parent from second fork == first child */

		/*
		 * We're the second child; our parent becomes init as soon
		 * as our real parent calls exit() in the statement above.
		 * Here's where we'd continue executing, knowing that when
		 * we're done, init will reap our status.
		 */
		/*
		上课的时候关于孤儿进程的例子，父进程退出后，子进程交给init，这样我们的shell进程就可以继续了。
		
		*/
		freopen( "/dev/null", "w", stdout );/*输出重定向到null，这样就不会输出东西来了*/


		sleep(2);
		printf("second child, parent pid = %ld\n", (long)getppid());
		exit(0);
	}

	if (waitpid(pid, NULL, 0) != pid)	/* wait for first child */
		err_sys("waitpid error");



}

void foreground(){

	int pid;

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {		/* child */
		execvp(argv[0], argv);
		err_ret("couldn't execute: %s", argv[0]);
		exit(127);
	}

	/* parent */
	if ((pid = waitpid(pid, NULL, 0)) < 0)
		err_sys("waitpid error");
	printf("%% ");
}

void input_redirect(char* argv[MAXARGS]){
	/*这里是只有输入重定向，输入输出都重定向的在另一个函数*/
	char* backargv[argc+1];
	int i;
	int redircet_sign;
	for(i=0;i<argc;i++){
		int length = strlen(argv[i]);
		backargv[i] = (char *)malloc(length);
		strcpy(backargv[i],argv[i]);

		if(strcmp(backargv[i],"<") == 0){
			redircet_sign = i; /*找出输入重定向符号的位置*/
		};
	}
	/*这时的redircet_sign就是输入符号的分隔区域，那么后面的应该就是输入文件的路径*/
	/* 输入重定向的话，那么，argv[redircet_sign+1]肯定是文件路径，argv[redircet_sign]是<符号*/
	backargv[argc]=NULL;
	int fd;
	fd = open(backargv[redircet_sign+1],O_RDONLY);

	if(fd <0){
		printf("file open error\n");

		return 1;
	}
	if(dup2(fd,STDIN_FILENO) < 0)    {
        close(fd);
        return 1; 
    }
	backargv[redircet_sign] = NULL;/*把重定向符号的位置赋为NULL，这时候前面就是命令*/

	excutecmd();/*执行重定向前面的命令*/

	close(fd);/*关闭文件*/

	int j;/*清空指针*/
	for(j=0;j<argc;j++){

		free(backargv[j]);
		backargv[j] = NULL;
	}
	return 1;
	
}

void output_redirect(char* argv[MAXARGS]){

	char* backargv[argc+1];
	int i;
	int redircet_sign;
	for(i=0;i<argc;i++){
		int length = strlen(argv[i]);
		backargv[i] = (char *)malloc(length);
		strcpy(backargv[i],argv[i]);

		if(strcmp(backargv[i],">") == 0){
			redircet_sign = i; /*找出输出重定向符号的位置*/
		};
	}
/*这时的redircet_sign就是输出符号的分隔区域，那么后面的应该就是输出文件的路径*/

/* 输出重定向的话，那么，argv[redircet_sign+1]肯定是文件路径，argv[redircet_sign]是>符号*/
	
	backargv[argc]=NULL;

	printf("%s",backargv[redircet_sign+1]);
	int fd;
	fd = open(backargv[redircet_sign+1],O_WRONLY|O_CREAT,777);

	if(fd <0){
		printf("file open error\n");

		return 1;
	}
	if(dup2(fd,STDOUT_FILENO) < 0)    {
        close(fd);
        return 1; 
    }
	backargv[redircet_sign] = NULL;/*把重定向符号的位置赋为NULL，这时候前面就是命令*/

	excutecmd();/*执行重定向前面的命令*/

	close(fd);/*关闭文件*/



	int j;
	for(j=0;j<argc;j++){

		free(backargv[j]);
		backargv[j] = NULL;
	}

	return 1;
}


int output_redirect2(char** backargv){

	int i;
	int redircet_sign;
	printf("%s",*backargv[1]);
	for(i=0;i<argc;i++){

		if(strcmp(*backargv[i],">") == 0){
			redircet_sign = i; /*找出输出重定向符号的位置*/
		};
	}
/*这时的redircet_sign就是输出符号的分隔区域，那么后面的应该就是输出文件的路径*/

/* 输出重定向的话，那么，argv[redircet_sign+1]肯定是文件路径，argv[redircet_sign]是>符号*/
	
	printf("%d",redircet_sign);

	int fd;
	printf("%s\n",*backargv[redircet_sign+1]);
	fd = open(*backargv[redircet_sign+1],O_WRONLY|O_CREAT,777);

	if(fd <0){
		printf("file open error\n");

		return 1;
	}
	printf("%d",fd);
	if(dup2(fd,STDOUT_FILENO) < 0)    {
        close(fd);
        return 1; 
    }
	*backargv[redircet_sign]= NULL;
	return fd;
}