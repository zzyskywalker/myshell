#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(){
    sleep(2);
    open("bgtest.txt",O_WRONLY|O_CREAT,0777);
    
    printf("2 seconds later ,wo create a file\n");

}