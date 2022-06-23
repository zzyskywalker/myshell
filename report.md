<!--
 * @Description: 
 * @Author: zzy
 * @Date: 2022-06-21 19:11:20
 * @LastEditTime: 2022-06-23 15:39:44
 * @LastEditors:  
-->
# APUE 上机作业-命令解释器 shell

本份报告包括
* 需求分析
* 详细设计
* 测试报告
* 总结报告

***

## 需求分析

需要实现的有四个功能：
1. 程序从控制台执行，启动后显示一个命令提示符。例如 “->”。用户可以通过给特定的环境变量赋值来改变命令提示符的形式；
2. 通过某个特殊的命令或按键组合可以正常地关闭本程序；
3. 提供后台运行机制。用户提交的任务可以通过某种指示使之在后台运行，例如：-> bg job1  &lt;CR&gt;将使任务 job1 在后台运行，并马上返回给用户一个新的提示符；
4. 提供输出重定向。通过指定文件名将任务的所有输出覆盖写到文件中而不是送到标准输出上；
5. 提供输入重定向。通过指定文件名使得任务从相应的文件中去获取所需的数据，而不是从标准输入上。

***

## 详细设计

### **持续的得到输入并且输出命令提示符**

首先我们需要从终端中持续的获得输入的命令，使用`while (fgets(buf, MAXLINE, stdin) != NULL){...}`,不断地将输入的命令存进buf中去，每次使用Enter键都会进行一次循环。在循环中，使用`fputs(command_prompt, stdout)`，可以在终端上输出命令提示符，我们只需要对`char command_prompt[] = "->"`进行不同的赋值就可以改变命令提示符的形式。

完成***功能1***。



### **正常地关闭程序**

我们设定退出命令为**exit**，当获得输入的命令后，我们使用`strcmp(buf,"exit")`进行比较，如果结果为0，那就可以调用`exit(0)`函数进行退出。

完成***功能2***。


### **切割字符串**

我们需要对输入的字符串进行切割，将其分为**命令**和**参数**。本程序中此函数的名字为`void splitcmd(char *buf)`。

首先用`int argc`储存命令及参数的个数；`char* argv[MAXARGS]`储存命令和参数本身，这是一个指针数组，数组的元素是指针，每个指针指向一个命令或参数字符串。

因为输入的字符串是用空格作为间隔的，所以`delim = strchr(buf, ' ')`可以得到第一个空格的位置,使用以下命令可以将输入的字符串指针`buf`存入`argv`的第`argc`个元素，然后在空格的位置使用`\0`进行截断，再将`buf`指针后移一位，这样就可以将空格之间的命令或者参数存到`argv`中。不断重复这个过程就可以将输入的字符串切割。
```c
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
```

### **输出重定向**

本程序定义输出重定向符号为`>`,我们通过将`argv`中的储存的字符串一一和`>`进行比较`strcmp(backargv[i],">")`，如果不存在的话就没有输出重定向；如果存在的话就调用输出重定向函数`int output_redirect(char* backargv[argc+1])`。

输出重定向函数首先确定`>`的位置`int redircet_sign`，正常情况下一位即`argv[redircet_sign+1]`应该是输出的文件名，但是为了防止用户忘记输入文件名，我们添加一个默认文件名`"noout.txt"`。

接下来只需要使用`open(backargv[redircet_sign+1],O_WRONLY|O_CREAT,0777)`打开这个文件，并且使用本功能的**核心函数**`dup2(fd,STDOUT_FILENO)`将标准输出定向到文件fd去即可。

本函数的返回值为打开的文件的fd，以便于后续关闭该文件。

完成***功能4***。

### **输入重定向**

输入重定向与输出重定向思路相似，只是符号变为`<`，默认的输入文件为`"/dev/null"`。同样的步骤打开文件，然后使用**核心函数**`dup2(fd,STDIN_FILENO)`将标准输入定向到文件fd。本函数的返回值为打开的文件的fd，以便于后续关闭该文件。

完成***功能5***。

### **前台执行函数**

为了执行输入的命令，需要用到`fork()`和`execvp()`两个函数。此函数名为`void foreground()`。

在此函数中我们会用`fork()`生成一个子进程，在子进程中，先判断是否调用重定向函数，再使用`execvp(argv[0], argv)`运行想要的命令。

如果调用了重定向函数，我们需要使用管道函数`pipe(pipefd)`将打开的文件的fd从子进程传递到父进程中去，并关闭，因为`execvp()`会将代码替换，无法在子进程执行完命令后关闭文件。

父进程会等待子进程的运行完毕，然后做一些善后的事情，包括`free()`指针、关闭文件。


### **后台执行函数**

后台执行命令的关键词为`"bg"`，主函数会首先判断`argv[]`中的第一个元素是否为`"bg"`,是的话进入后台`background()`；否的话就进入`foreground()`。

在后台执行函数的核心思路来源于课上讲的例子：**孤儿进程会被init进程收养**。父进程创建子进程，子进程再创建一个孙子进程，退出子进程，那么孙子进程就变成了孤儿进程。此时的孤儿进程就可以执行任务，并且不需要父进程等待。

基本思路如上所述，并且为了不会有输出，此函数必须执行输出重定向函数，默认文件为`"noout.txt"`。而时候输入重定向则看输入的参数。在孙子进程中也是使用`execvp()`执行任务。

完成***功能3***。

***

## 测试报告

功能1：

运行zzyshell.out，进入zzyshell，有命令提示符`->`。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
->ls  
1.txt  bgtest  bgtest.c  Makefile  test  test2  test3  zzyshell.c  zzyshell.out
->pwd
/home/zzy/Document/apue.3e/zzy
->exit
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ 
```

通过对`char command_prompt[]`进行修改，可以改为其它的符号，比如`===>>`。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>ls
1.txt  bgtest  bgtest.c  Makefile  test  test2  test3  zzyshell.c  zzyshell.out
===>>exit
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ 
```

功能2：

输入`exit`可以退出本shell，当然常用的`Ctrl+C`和`Ctrl+D`也可以退出。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>exit
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>^C
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ 
```

功能3：

为了验证后台功能，实现了bgtest程序，该程序会在调用2s之后生成一个文件。当没有添加bg的时候会提示默认输出重定向到noout.txt，然后可以立刻输入下一个命令。也可以看出`bg`和重定向功能可以以前调用。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>./bgtest
2 seconds later ,wo create a file
===>>bg ./bgtest
without input outfile path,default noout.txt
===>>ls   
bgtest  bgtest.c  bgtest.txt  Makefile  noout.txt  test  test2  test3  zzyshell.c  zzyshell.out
===>>bg ls
without input outfile path,default noout.txt
===>>exit
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ 
```
为了直观的查看效果，动图演示如下：

![后台运行演示](https://s3.bmp.ovh/imgs/2022/06/23/a38ca82235b3ebf7.gif)


功能4：

可以将ls的输出重定向到`1.txt`上。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>ls
bgtest  bgtest.c  bgtest.txt  Makefile  noout.txt  test  test2  test3  zzyshell.c  zzyshell.out
===>>ls > 1.txt
===>>cat 1.txt
1.txt
bgtest
bgtest.c
bgtest.txt
Makefile
noout.txt
test
test2
test3
zzyshell.c
zzyshell.out
===>>
```

功能5：

使用`wc`命令来测试输入重定向。

首先在bash中查看输入重定向的效果,当输入重定向的时候，结果会少一部文件名。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ wc zzyshell.c
 434  768 9364 zzyshell.c
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ wc < zzyshell.c
 434  768 9364
```
在zzyshell中测试得到结果与上相同，证明输入重定向正确。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>wc zzyshell.c
 434  768 9364 zzyshell.c
===>>wc < zzyshell.c
 434  768 9364
===>>
```

额外测试：

输入重定向与输出重定向一起：
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>wc < zzyshell.c > wc.txt    
===>>cat wc.txt
 434  768 9364
===>>

```

后台、输入重定向与输出重定向一起，虽然看不出效果，但是可行。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./zzyshell.out 
===>>bg wc < zzyshell.c > wc2.txt
===>>cat wc2.txt
 434  768 9364
```
***

## 总结报告

本次实验完成了一个简单的shell。主要的知识是关于父子进程，主要的思想是使用`fork`来产生一个子进程。在做实验的过程中，加强了对进程的理解；体会到了如何使用进程的相关知识。除此之外，还练习了关于Linux系统下文件的打开、读、写等操作。当然我还学习了关于c语言的一些知识，并且课程之外学习Linux系统的操作，以及云服务器的配置和使用，收获颇丰。