

## 支持嵌套的continue和break语句的实现

### ?.1 实现的功能

类似C语言的break和continue语句的实现。

### ?.2 实现的原理

仿照C语言的实现原理，当遇到break跳出循环或者跳出switch语句，遇到continue的时候，跳出本层的循环语句执行。

### ?.3 实现的思路

break和continue都可以视为一个JMP指令，一个是跳转到该函数体以外，而另一个是跳过本层循环，只是回填的地址不同而已，在考虑嵌套的时候就要考虑多层的break和continu。

### ?.4 具体实现

实现的时候，当遇到break，生成一条JMP指令，并用一个二位数组记录此时的cx值，此时，数组的一维坐标标识的嵌套的层数，这样便于回填的时候不会回填错位置，同时，用一个以为数组记录相应层次里的break的数量。continue语句也如此。在回填的时候，先检测该层中相应的break和continue是不是为空，如果为空不会填，否则，回填相应的回填地址。

### ?.5 效果

break测试：

```pl0
var result,a,i;
begin
        i:=0;
	result:=5;
        do
        {
           if(a==3)
           then
              {
                      do
                      {
                        if(i==2)
                        then 
                              break;
                        else ;
                              i++;
                      };while(i<a);
              };
           else ;
           a++;
        };while(a<result);
        printf("hello @0 @1\n", a,i);
end.
```

输出结果

```
Begin executing PL/0 program.
hello 5 2
End executing PL/0 program.
```

continue测试：

```c
var i,j;
begin
     j:=0;
     i:=0;
     do
     {
         i++;
         if(i==5)
         then
             continue;
        else ;
         if(i==6)
         then
             continue;
        else ;
        j++;
     };while(i<10);
     printf("@0 @1\n",i,j);
end.
```

输出结果：

```c
Begin executing PL/0 program.
10 8
End executing PL/0 program.

```





### ?.6 后续工作

还要优化一下一些细节部分的实现。