#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
/*    
char array[20]="hello world";
char *buf = array;
printf("%s\n",buf);
buf++;
printf("%s\n",buf);
int out_redirect;
if( out_redirect = (strcmp(">",">")==0) ){
    printf("%d\n",out_redirect);

};*/

char* a[5];
char* b[5];

a[0] = "aaaa";
a[1] = "bbbbb";
a[2] = "<";
a[3] = NULL;


/*测试指针数组的复制并确定重定向符号位置*/
int i;
for(i=0;i<3;i++){
    int length = strlen(a[i]); 
    b[i] = (char *)malloc(length);
    strcpy(b[i],a[i]);

    if(strcmp(b[i],"<") == 0){

        printf("%d",i);
    }

    printf("%s",b[i]);
}
b[3]=NULL;
/*
for(i=0;i<3;i++){
free(b[i]);
b[i]=NULL;

}*/
/*测试后台运行*/
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
    /*freopen( "/dev/null", "w", stdout );输出重定向到null，这样就不会输出东西来了*/


    sleep(2);
    printf("second child, parent pid = %ld\n", (long)getppid());
    exit(0);
}



if (waitpid(pid, NULL, 0) != pid)	/* wait for first child */
    err_sys("waitpid error");

printf("parent");
/*这一块验证，孙子进程将输出重定向到null之后，父进程仍然会输出parent*/




exit(0);
}