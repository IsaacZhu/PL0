# ? 支持嵌套的switch语句的实现

## ?.1 实现的功能

类似C语言的switch语句的实现。

## ?.2 实现的原理

仿照C语言的实现原理，将case视为一个用于记录跳转位置的标识符，default同理。然后将switch后面的statement视为一个整体，在其后生成相应的跳转指令，跳转回原先的位置。

## ?.3 实现的思路

将case视为一个用于记录跳转位置的标识符，default同理。然后将switch后面的statement视为一个整体，在其后生成相应的跳转指令，跳转回原先的位置。交换switch和statement的位置。

## ?.4 具体实现

实现的时候，关键在于实现switch后面的表达式和case后面的常量的比较，我在读入switch标识符的时候，增加JMP指令，跳转到switch后面的statement执行完毕的位置，接下来，生成statement里面的语句的汇编，在这个过程中，如果遇到case，记录case后面的数值，并记录此时的cx的数值，default的位置也要记录。当statement生成结束，交换switch（）括号中的表达式与statement的位置，并且在两者之间加入JMP指令，待回填，注意，在嵌套的时候，要考虑已经回填的地址的改变。接下来，加入新的指令SWIT，和CMP，为下面的条件判断做准备，用JLEZ判断是不是需要跳转，注意，default放在最后，直接调用JMP指令，跳转到default标识的位置。回填JMP。

## ?.5 效果

```pl0
var result,a;
begin
	result:=5;
        a:=2;
        switch (result) 
        {
         case 6:
                {
                        result:=7;
                        break;
                };
         case 5:{
                 switch(a)
                 {
                         case 1:
                         {
                                 result:=3;
                                 break;
                         };
                         case 2:
                         {
                                 result:=4;
                         };
                 };
                 break;
                };
         case 7:{
                 result:=1;
                 };
         default:{
                 result:=2;
         };
         };
        printf("@0\n", result);
end.

```

输出结果

```
Begin executing PL/0 program.
4
End executing PL/0 program.
```

## ?.6 后续工作

还要优化一下一些细节部分的实现。