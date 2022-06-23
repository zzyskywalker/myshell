<!--
 * @Description: 
 * @Author: zzy
 * @Date: 2022-06-22 23:12:38
 * @LastEditTime: 2022-06-23 20:31:29
 * @LastEditors:  
-->

# zzyshell
**Implementation of a basic LINUX shell in C.**

## 简介
本程序主要实现的是一个简单的shell，命名为zzyshell，它可以执行简单的命令并提供输入和输出重定向以及后台执行的功能。
本程序主要文件包括zzyshell.c和bgtest.c。其中zzyshell.c为shell的源代码；bgtest.c是为了测试后台运行功能而写的，它的作用是在2秒中之后生产一个txt文件。

***

## 环境要求
本程序尽在Ubuntu20.04上进行了测试，可以正常运行。未在其他平台实验。

include的头文件包括stdio.h、stdlib.h、string.h、sys/types.h、sys/stat.h、fcntl.h、unistd.h。

***
## 主要实现的功能

需要实现的有四个功能：
1. 程序从控制台执行，启动后显示一个命令提示符。例如 “->”。用户可以通过给特定的环境变量赋值来改变命令提示符的形式；
2. 通过某个特殊的命令或按键组合可以正常地关闭本程序；
3. 提供后台运行机制。用户提交的任务可以通过某种指示使之在后台运行，例如：-> bg job1  &lt;CR&gt;将使任务 job1 在后台运行，并马上返回给用户一个新的提示符；
4. 提供输出重定向。通过指定文件名将任务的所有输出覆盖写到文件中而不是送到标准输出上；
5. 提供输入重定向。通过指定文件名使得任务从相应的文件中去获取所需的数据，而不是从标准输入上。

***
## 运行
首先使用以下命令编译文件
```
gcc zzyshell.c -o zzyshell.out
gcc bgtest.c -o bgtest.out
```
然后`./zzyshell.out`就可以运行shell，接着测试其功能，例如：
```
ls -l 
ls > 1.txt
wc < zzyshell.c
bg ./bgtest.out
exit
```
***

## 程序结构
本程序除主函数之外包括以下函数。
* `void splitcmd(char *buf);`，作用是将输入的字符串分割成命令。
* `void background();`，作用是在后台执行任务。
* `void foreground();`，作用是在前台执行任务。
* `int  is_redirect();`，作用是判断是否重定向，包括输入和输出。
* `int output_redirect(char* backargv[argc+1]);`，作用是输出重定向。
* `int input_redirect(char* backargv[argc+1]);`，作用是输入重定向

本程序的主要的全局变量为：
* `char command_prompt[] = "->";`,保存的命令提示符的形式，可以更改。
* `int argc;`,保存拆分后的命令的数目。
* `char* argv[MAXARGS];`,保存拆分后的命令。
* `int out_redirect;`,输出重定向标识符。
* `int in_redirect;`,输入重定向标识符。

***
Author：zzy

本程序源代码存储在[github](https://github.com/zzyskywalker/myshell.git)。

鸣谢：
@[ZhangChunXian](https://github.com/ZhangChunXian)
@[ckdepaul13](https://github.com/ckdepaul13)
@[Sourav Aich](https://github.com/souravaich)，
感谢他们的项目带来的思路。