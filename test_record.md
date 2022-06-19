当我在35、38、80行的``printf``加上``\n``的时候，输出是正确的。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./test 
aaaa
bbbbb
2
<
parentzzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ second child, parent pid = 1

```


但是当我把``\n``去掉的时候，就会输出如下所示。
```
zzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ ./test 
aaaabbbbb2<aaaabbbbb2<parentzzy@VM-20-4-ubuntu:~/Document/apue.3e/zzy$ aaaabbbbb2<second child, parent pid = 1

```

有一些重复的输出。是不是和buffer有关？


可以使用输入重定向的命令，wc cat