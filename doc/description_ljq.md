###实现用"=="判断是否相等

修改部分：

1. pl0.h
   1. symtype添加SYM_LOGIEQU；
   2. oprcode添加OPR_LOGIEQU；
2. pl0.c
   1. getsym()添加对=和==的区分
   2. condition()内将swith内原有的SYM_EQU改为SYM_LOGIEQU，该分支内的OPR同理（PS：ATTENTION！！！这里只改了condition()是因为我的版本里设计判断是否相等的只有condition()。若后续版本中在其他的地方有用到这个也得同理修改）
   3. interpret()内将switch内原来的OPR_EQU改为OPR_LOGIEQU即可（因为这里的==实际上完成了原来的=的作用，后续再用=代替:=）



###数组的实现

1. pl0.h
   1. symtype中原本应该是有中括号符SYM_LSBRAC和SYM_RSBRAC的，不太记得了，最好确认一下（不过这么好的SYM名估计不是我起的）
   2. idtype添加ID_ARRAY
   3. opcode添加LEA，LODAR，和STOAR
   4. err_msg添加了数组相关的28～30
   5. 汇编指令集的符号集mnemonic添加了新添的三个指令的字符串
   6. 新添三个结构体变量array, dimensionHead, dimension。在符号表中，第一个存储数组符号，第二个存储数组维数，第三个存储数组的各维大小（这个方式超级粗暴，感觉有待改进，但是又没想到更好的方式，如果有人想到请跟我联系）
2. pl0.c
   1. enter()内添加了数组相关的，因为数组的本质还是var，因此在做语法分析的时候会进入vardeclaration()再进入enter()。我的数组的语法分析也写在enter()里面了，但是总感觉不太好。如果有意见或是建议一定要跟我说。
   2. position()内也有修改。由于数组在符号表内的存储结构，而position()是从后往前找，因此这里数组找到之后还要额外往前找到第一个结构体。但是这里貌似有一个bug，还没想清楚怎么改。就是如果有两个block，一个嵌套另一个，如果前后用了同一个名字做数组，这里可能会寻址到最前面的。在我改之前还请各位手下留情lol。
   3. factor()内在switch里添加了ID_ARRAY分支，但是最前面还有一些添加的变量声明，不要漏了。
   4. statement()内最前面也有一些添加的变量声明，然后就是SYMI_IDENTIFIER的if分支内有很大的修改，这个感觉直接用我的分支内的代码快替换原来的就行了。
   5. interpret()后面有新添的三个指令的实现
   6. 另外，在这个里面，factor()和statement()内我擅自使用了set，为了保证后面的调用test()不出错，但是实际上我还没有完全弄清楚，可能实际上是有问题的。如果有人完全理解了，希望能给我讲一下/泪奔