# 参数传递实现

朱嘉润 17.10.28

------

## 目录

### 1.实现的功能

### 2.实现思路

### 3.具体实现

### 4.更多工作

------

## 1.实现的功能

1. 支持函数定义时加参数，具体格式如：

   ```
   procedure funcname(arg1,arg2,arg3<,more args...>);
   begin
   	<func body> 	//you can use arguments here
   end;
   ```

2. 不再支持基于call的带参函数调用

3. 支持复杂的作用域规则：在函数嵌套时，除了传递的同名参数，子函数都可以调用父函数的变量并改变其值，简要的说就是同名变量以作用域最近的为准，如：

   ```
   var b,c,d;
   procedure myfunc1(a);
   begin
   	a:=a+1;
   	b:=b+2; 	//not declared!
   end;
   //main
   begin
   	b:=0;
   	c:=1;
   	myfunc1(c);
   end.
   ```

   最终结果是b为2，而a为3。

4. 由于实现原因，仍然支持不带参无括号的过程声明和调用。

5. [内部功能]为每个作用域（函数/过程）添加了自己的符号表。Func指针指向当前正在操作的函数的符号表结点。

6. 支持return

7. 支持函数参数表达式，如：

   ```
   var a,b,d;
   procedure foo(c);
   begin
   	c:=c+1;
   	return c;
   end;
   begin
   	a:=1;
   	b:=2;
   	d:=foo(a+b);
   end.
   ```

------

## 2.实现思路

1. 用链表实现一个结构，每个结点保存一个符号表。    
2. 把table变成指针，指向某个符号表 。
3. 每次创建新的procedure，就创建一个符号表，把table指向这个结点，复制其父节点的符号表。
4. 对于参数列中出现的父结点已有的变量，以新的为准，覆盖父结点的。
5. 对于函数声明，进行参数检测，复制到一个临时的数组，在创建结点时把参数名复制过去。
6. 至此子函数部分的声明代码基本完成，接下来在其调用时（遇到CALL），利用APOP指令，将参数逐个放到栈顶。

------

## 3.具体实现

1. 添加链表式的table定义，实现链表的插入和删除函数，将table修改成指针。每次链表插入和删除时，改变table指向，改变dx，tx。

2. 修改enter函数，每次遇到ID_PROCEDURE记录下这个函数名，为下面返回函数汇编地址做准备。修改pl0.h，添加tmpaddress，记录函数汇编地址。

3. 修改block函数，在函数开头建立新结点，函数结尾删除结点；   在调用函数处添加被调用者地址记录：用在block结尾处tmpaddress记录地址，在调用函数结束后将这个地址赋到父函数的符号表中去。

4. 在pl0.h中添加position函数的声明，以支持在其他函数中调用。

5. 修改pl0.h，添加一个临时字符数组，存储参数列。注意，暂时最多支持50个参数，参数名最长50。   在pl0.h中添加全局变量funcparam，记录参数个数  。修改了block函数，在sym_procedure处添加了参数声明的代码，将参数放入一个临时的参数表（tmpparam），并计算参数个数。产生式如下：

   ```
   ProcDecl -> procedure id ‘(’ ParaList ‘)’ | procedure id ‘(’ ‘)’ParaList -> ParaList ‘,’ Para | ParaPara   -> id dimDeclaration 
   ```

    暂时不修改procedure后面带分号这个设定   修改block函数的INT，加上参数个数。

6. 使用一个新的param_enter函数，将临时参数表中的参数一个个加入到符号表。

7. 添加PAS指令，用于参数传递。具体是往top+4开始的空间填充东西。   

   （1）修改pl0.h中关于汇编指令的三个定义   

   （2）在解释器中添加PAS指令   

8. 修改statement函数中SYM_CALL的部分：若有参数需要传递，便生成PAS，具体而言是先生成PAS再生成

9. 测试用例：exf.txt

10. 将base指针插入符号表：

   （1）修改pl0.h，添加类型：ID_POINTER    

   （2）修改block函数：在执行statement之前将base插入符号表，类型为ID_POINTER，为了避免与变量名冲突，base指针称为"/pbase"（因为变量名中不能有特殊符号）。如果符号表中已有/pbase指针，重写/pbase指针的level。

11. 在调用处实现参数逐个入栈：     

    （1）在pl0.h中添加指令 APOP，ASTO     

    （2）APOP两个域： 第一个域废弃，为0，第二个域为/pbase的地址             

    ASTO两个域： 第一个域废弃，为0，第二个域为/pbase的地址     

    （3）修改interpret函数：在解释器中实现APOP，具体是将栈顶参数放到base+1处，同时base++，然后top变成base+1     

    （4）修改interpret函数：在解释器中实现ASTO，具体是将top存到/pbase中     

    （5）修改factor函数：在函数调用处（ID_PROCEDURE），先将当前栈顶指针存到base，然后循环，对每个参数表达式执行expression（），此后参数位于栈顶，执行APOP，将参数下沉到base+1处，top也下沉到此处。

12. 在函数开头实现从栈中获取参数值：     

    （1）在pl0.h中添加指令LODA     

    （2）LODA指令两个域：第一个域是参数个数，第二个域为参数次序，为1,2,3...     

    （3）修改interpret函数：在解释器中实现LODA，调用时栈如图：![QQ截图20171102193641](C:\Users\l\Desktop\QQ截图20171102193641.jpg)

    （4）修改block函数：在生成INT后，利用全局的Func指针，获得当前函数的参数个数，逐个生成LODA指令，从栈中取出参数。     

    （5）在pl0.h及interpret函数中删除PAS指令

------

## 4.更多工作

1. 仍需要删除分号，这个很简单，但是最好能实现return后再改，因为又要动SYM_CALL部分。
2. 由于仍不理解error的工作原理，因此只是简单地做了一些错误检测，没有做详尽的改动。包括参数不匹配，函数声明不对等等均不知道调用后会产生何种后果。这里需要做更多改进。
3. 暂时不清楚statement处的ID_PROCEDURE有没有用，目测是没有了，但是暂时不动这部分。