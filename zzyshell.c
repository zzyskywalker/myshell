#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/*命令提示符的形式,可以更改*/
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
void background();
void foreground();

int  is_redirect();
int output_redirect(char* backargv[argc+1]);
int input_redirect(char* backargv[argc+1]);

int
main(void)
{
	char	buf[MAXLINE];
	pid_t	pid;
	int		status;

	if (fputs(command_prompt, stdout) == EOF){
		printf("output error\n");
	};
	
	
	while (fgets(buf, MAXLINE, stdin) != NULL){
		buf[strlen(buf) - 1] = '\0';/*将最后一位换行符修改*/
		if (strcmp(buf,"exit") == 0)
			exit(0);

		out_redirect = 0;
		in_redirect = 0;

		splitcmd(buf);	/*在这里将命令行中的输入切分成了命令数组*/
		is_redirect();


		if(strcmp(argv[0],"bg") == 0){

			background();

		}else{
			
			
			foreground();
		};

		if (fputs(command_prompt, stdout) == EOF)
			printf("output error\n");

	};
	

	if (ferror(stdin))
		printf("input error\n");

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
	/*构造命令数组，以e空格分隔*/
		delim = strchr(backbuf, ' ');
	
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
void foreground(){
	int pid;
	char* inbackargv[argc+1];
	char* outbackargv[argc+1];
	int outfd=STDOUT_FILENO;
	int infd =STDIN_FILENO;
	
    int pipefd[2];

    char outtempbuf[1];
	char intempbuf[1];

    if(pipe(pipefd)<0) /*为了从子进程将fd传到父进程，然后父进程关闭fd*/
        printf("pipe error\n");
	int i;
	for(i=0;i<argc;i++){   /*备份一下argv，因为要进行修改*/
		int length = strlen(argv[i]);
		inbackargv[i] = (char *)malloc(length);
		strcpy(inbackargv[i],argv[i]);

	};
    inbackargv[argc]= NULL;
	for(i=0;i<argc;i++){   /*备份一下argv，因为要进行修改*/
		int length = strlen(argv[i]);
		outbackargv[i] = (char *)malloc(length);
		strcpy(outbackargv[i],argv[i]);

	};
    outbackargv[argc]= NULL;
    
    
	if ((pid = fork()) < 0) {
		printf("fork error\n");
	} else if (pid == 0) {		/* child */

		if(in_redirect){

			infd = input_redirect(inbackargv);
            /*将重定向的fd转换成字符串并通过通道传到父进程*/
            int bytes = sprintf(intempbuf, "%d", infd); 
            close(pipefd[0]);
            write(pipefd[1], intempbuf, 1);
		};

		if(out_redirect){
            outfd = output_redirect(outbackargv);
            /*将重定向的fd转换成字符串并通过通道传到父进程*/
            int bytes = sprintf(outtempbuf, "%d", outfd); 
            /*printf("%s",outtempbuf);*/
			
            close(pipefd[0]);
            write(pipefd[1], outtempbuf, 1);
			
		};
		if(in_redirect){
				execvp(inbackargv[0], inbackargv);
				printf("couldn't execute: %s\n", inbackargv[0]);
		}else{

				execvp(outbackargv[0], outbackargv);
				printf("couldn't execute: %s\n", outbackargv[0]);
		};
    	if (outfd != STDOUT_FILENO) {
    		    close(outfd); 
            }

		if (infd != STDIN_FILENO) {
                close(infd);
            }
		
		exit(127);


	}

	/* parent */

    

	if ((pid = waitpid(pid, NULL, 0)) < 0)
		printf("waitpid error\n");

	if(out_redirect){
        close(pipefd[1]);
        if(read(pipefd[0], outtempbuf, 1)<0)
            printf("read error");
        outfd = outtempbuf[0]-'0';

		if (outfd != STDOUT_FILENO) {
            
            close(outfd);
            /*printf("close file\n");*/
        }
    }


	
	if(in_redirect){
        close(pipefd[1]);
        if(read(pipefd[0], intempbuf, 1)<0)
            printf("read error");
        infd = intempbuf[0]-'0';

		if (infd != STDIN_FILENO) {
            
            close(infd);
            /*printf("close file\n");*/
        }
    }


	/*printf("%s finished\n",parameter[0]);*/
	int j;
	for(j=0;j<argc;j++){

		free(inbackargv[j]);
		inbackargv[j] = NULL;
		free(outbackargv[j]);
		outbackargv[j] = NULL;
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
	char* inbackargv[argc+1];
	char* outbackargv[argc+1];
	/*需要把前面的bg去掉，所以都前移一下*/
	int i;
	for(i=0;i<argc-1;i++){   /*备份一下argv，因为要进行修改*/
		int length = strlen(argv[i+1]);
		inbackargv[i] = (char *)malloc(length);
		strcpy(inbackargv[i],argv[i+1]);

	};
	inbackargv[argc-1]= NULL;
    inbackargv[argc]= NULL;

	for(i=0;i<argc-1;i++){   /*备份一下argv，因为要进行修改*/
		int length = strlen(argv[i+1]);
		outbackargv[i] = (char *)malloc(length);
		strcpy(outbackargv[i],argv[i+1]);

	};
    outbackargv[argc-1]= NULL;
	outbackargv[argc]= NULL;

	/*为了后台运行不输出，我们加重输出标识符，在重输出函数中输出到默认文件中*/
	if(!out_redirect){
	outbackargv[argc-1] = (char *)malloc(1);
	outbackargv[argc-1] =">";
	}

	if ((pid = fork()) < 0) {
		printf("fork error\n");
	} else if (pid == 0) {		/* first child */
		if ((pid = fork()) < 0)
			printf("fork error\n");
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

		/*重输出是必须的*/
		output_redirect(outbackargv);

		if(in_redirect){

			input_redirect(inbackargv);
			};

		if(in_redirect){
				execvp(inbackargv[0], inbackargv);
				printf("couldn't execute: %s\n", inbackargv[0]);
		}else{

				execvp(outbackargv[0], outbackargv);
				printf("couldn't execute: %s\n", outbackargv[0]);
		};

		exit(1);
	}


	if (waitpid(pid, NULL, 0) != pid)	/* wait for first child */
		printf("waitpid error\n");

	int j;
	for(j=0;j<argc-1;j++){

		free(inbackargv[j]);
		inbackargv[j] = NULL;
		free(outbackargv[j]);
		outbackargv[j] = NULL;
	}

}

int output_redirect(char* backargv[argc+1]){

	int i;
	int redircet_sign;
/*
    printf("%s\n",backargv[0]);
    printf("%s\n",backargv[1]);
    printf("%s\n",backargv[2]);
    printf("%s\n",backargv[3]);
*/
    for(i=0;i<argc;i++){
		if(backargv[i]!=NULL){
			if(strcmp(backargv[i],">") == 0){
				redircet_sign = i; /*找出输出重定向符号的位置*/
			};
		}
	}
/*这时的redircet_sign就是输出符号的分隔区域，那么后面的应该就是输出文件的路径*/

/* 输出重定向的话，那么，argv[redircet_sign+1]肯定是文件路径，argv[redircet_sign]是>符号*/

	int fd;
    if(backargv[redircet_sign+1]==NULL){
		backargv[redircet_sign+1] =(char *)malloc(9);
		backargv[redircet_sign+1]="noout.txt";
        printf("without input outfile path,default noout.txt\n");

        

    };
	fd = open(backargv[redircet_sign+1],O_WRONLY|O_CREAT,0777);

	if(fd <0){
		printf("file open error\n");

		return 1;
	}
	/*printf("%d",fd);*/
	if(dup2(fd,STDOUT_FILENO) < 0)    {
        close(fd);
        return 1; 
    }
	backargv[redircet_sign]= NULL;
	return fd;

}

int input_redirect(char* backargv[argc+1]){

	int i;
	int redircet_sign;

    for(i=0;i<argc;i++){
		if(backargv[i]!=NULL){
			if(strcmp(backargv[i],"<") == 0){
				redircet_sign = i; /*找出输出重定向符号的位置*/
			};
		}
	}
/*这时的redircet_sign就是输出符号的分隔区域，那么后面的应该就是输出文件的路径*/

/* 输出重定向的话，那么，argv[redircet_sign+1]肯定是文件路径，argv[redircet_sign]是>符号*/

	int fd;
    if(backargv[redircet_sign+1]==NULL){
		backargv[redircet_sign+1] =(char *)malloc(9);
     	backargv[redircet_sign+1]="/dev/null";
        printf("without input infile path,default /dev/null\n");
    };

	fd = open(backargv[redircet_sign+1],O_RDONLY);

	if(fd <0){
		printf("file open error\n");

		return 1;
	}

	if(dup2(fd,STDIN_FILENO) < 0)    {
        close(fd);
        return 1; 
    }
	backargv[redircet_sign]= NULL;
	return fd;

}