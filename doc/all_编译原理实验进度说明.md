# 编译原理实验进度说明

---

2017.10.9 董仕

* + 添加位运算 '&', '^', '|', 并按照生成式优先级排列函数嵌套顺序。|| -> && -> | -> ^ -> +/-
* + 添加取模运算 '%'
* + 支持类似 C 的条件判定，（非零值判定为真）

---

2017.10.25  杨文涛

本次修改主要是对SYM_ELSE和SYM_ELSE_IF语句的补充实现。

* 添加了变量cx3，cx4，cx5。用于对各个阶段cx的储存

* 主要添加部分如下

  ```c
  	else if (sym == SYM_IF)
  	{ // if statement
  		getsym();
  		set1 = createset(SYM_THEN, SYM_DO, SYM_NULL);
  		set = uniteset(set1, fsys);
  		condition(set);
  		destroyset(set1);
  		destroyset(set);
  		if (sym == SYM_THEN)
  		{
  			getsym();
  		}
  		else
  		{
  			error(16); // 'then' expected.
  		}
  		cx1 = cx;
  		gen(JPC, 0, 0);
  		statement(fsys);
  		getsym();
  		if(sym==SYM_ELSE_IF)//add by ywt,deal with SYM_ELSE_IF,2017.10.20
          {
              getsym();
  			condition(set);//
  		    if (sym == SYM_THEN)
  		    {
  			   getsym();
  		    }
  		    else
  		    {
  			error(16); // 'then' expected.
  		    }
              cx4=cx;
              code[cx1].a=cx+1;
              gen(JPC,0,0);
              statement(fsys);
  			getsym();   
  			 if(sym==SYM_ELSE) 
             {
              getsym();
              cx5=cx;
              code[cx4].a=cx+1;
              gen(JMP,0,0);
              statement(fsys);
              code[cx5].a=cx;                                                  
             }     
  		   else
  		   code[cx4].a=cx;                                       
           }
  		else if(sym==SYM_ELSE) //add by ywt,deal with SYM_ELSE,2017.10.20
          {
              getsym();
              cx3=cx;
              code[cx1].a=cx+1;
              gen(JMP,0,0);
              statement(fsys);
              code[cx3].a=cx;                                                  
           }
  		else
  		code[cx1].a = cx;
  	}
  ```

  else语句直接使用JMP语句跳转，无需进行条件判断。

  而对于else if语句，需要考虑到结构if——else和if——else if--else，else if需要对条件进行判断，因此需要使用JPC实现，并判断接下来的语句。

* 条件短路算法遇到的问题任然没有解决，遇到的问题是，编译出的汇编代码正确，但是无法跑出正确结果，condition部分遭到破坏。现在仍然在进行更改，但对后续任务影响不大。

---

Dong Shi, 10.28

* + Add OP RET to the interpret
* + Add Procedure reserved word to factor set
* + Add Procedure handle in factor
* + Add movement of RET in statement
* + Add "break" to case OPR\_NEQ and OPR\_GEQ
* - Remove reserved word call

* ! WARNING: For allowing procedure name appear without call, I allow all factor appear without a statement. (By disabling test(facbegsys, fsys, 24) in factor)

* ! WARNING: procdure(even with return value) and constant cannot be in parameter list of another procedure

* ! WARNING: in factor, while(insert){} was changed to insert

* ! WARNING: in case RET, only one ++top left

---

董仕, 11.18

* + 增加系统级指令 exit 
    具体实现：exit -> {JMP 0}

---

董仕, 11.23

* + 扩充解释器 OPR 的 OPR\_INC 与 OPR\_DEC, 分别是栈顶变量的自增和自减
* + 扩充多字符符号 SYM\_INC 以及 SYM\_DEC, 分别表示 "++" 与 "--", 并添加其词法分析实现
* - 取消了对 error 19 的检查（以便支持++/--）

注：按照正常逻辑，自增自减应该在 factor 里实现。但目前 PL0 目前不支持左值入栈，也就不存在连续赋值（a := b := c)
更不用说 ((++++++(++i--))--) 这种~~鬼畜~~写法了。因此考虑现阶段将其当做特殊的 statement 处理。也就是说不管是
var ++ 还是 ++ var，都等同于 var := var + 1 的语法糖。还是由于左值不入栈的原因，左自增和右自增完全没有任何区别。

---

zjr 11.25
* + fix a bug in annotation analysis, support div now (ex10)

---
zjr 11.27
* + support relop in expression now(in function rel_expr)! example:ex14 

---

董仕 12.1

* + 新增保留字 printf 与 random
* + 解释器新增指令 OUTS，用来输出输出栈的字符与值
* + 添加类似于 C 的 printf 内置函数，功能详见完整文档
* + 添加 random() 内置函数，无参，依赖 linux 系统的 sys/time.h 函数
* - 去掉 pl0 的存入则打印的规则

---

董仕 12.3

* + 为 random 添加参数，若有参数只能为 NUMBER 类型也就是立即常数
* + 添加解释器指令 IN，用于输入整数
* + 添加内置函数 input(var)，用于用键盘输入的整数为变量赋值
* + 添加一般化的自增自减处理
* + 添加连续赋值
* + 添加列表式赋值

---

ZJR 12.9

1.添加了对 ？ ： 这种表达式的支持

2.添加了对左移、右移运算符的支持

3.具体文档稍后补上

---

ZJR 12.9

1.添加了对连续赋值和赋值运算符（such as 加等，乘等）的支持

2.文档稍后补上

---

董仕 12.12

* + 添加类型 list （但是未广泛支持）
* + 使用 list 实现连续列表赋值


---

ZJR 12.13

1.支持函数作为参数

2.原来使用APOP和ASTO指令是避免有谁写出不清栈的指令，影响过程调用。但是处理掉LODAR以后，应该只有STOAR不清栈了，但是有STOAR的地方都小心翼翼地处理了，该加POP的地方都加了POP，因此这两条指令应该也完成了历史任务，而且他们会影响函数作为参数，因此删除掉。同样的可以删除的还有\pbase，但是这个影响的地方稍稍有点多，暂时先不动，对效果不影响。

3.例子可以参考ex22

---

ZJR 12.13

1.修改列表式赋值，现在列表式赋值右值支持表达式，而且支持全运算符。

2.example见ex22

---

ZJR 12.14

支持变量引用作为参数，即形如： 

```
var m;
procedure p(&j);
begin
	j+=2;
end;
begin
	m:=1;
	p(m);
end.
```

输出的结果M是3。这里本质上其实是传指针了。

---

ZJR 12.14

解决了一个历史遗留问题，即不支持直接调用一个有参函数但不用其返回值。即现在可以： 

```
var i;
procedure p(i);
begin
	i:=i+1;
end;
begin
	p(i);
end.
```

具体实现也很简单，就是把factor里procedure的工作复制到statement里，并加了几个条件判断防止出错。

例子可以看ex23，或者直接看上面。

---

ZJR 12.14

支持了CALLSTACK对 引用变量的输出，并优化了callstack的显示效果。

---

ZJR 12.15

解决了几个老大难问题：

1. if后面只能跟一条语句
2. if后面如果不接else，后面无法再接语句。
3. {}后面必须有;。
4. Else if 数量仅限一个。

具体思路：

1和2是因为if多取了一个sym，因此后面无法正确分析句子。这里非常取巧地先把sym存起来，然后把当前sym改为；,在getsym中，如果发现存起来的sym不是0，则将sym设为它，并将其置0。这样就巧妙地完成了sym的回退。

3是因为}不在stat判定符集中，但是;在其中，因此遇到}时，不再getsym，而是将sym改成；。

4是因为设计逻辑问题，纯粹是设计者有点脑残。在这里把else if无效化，而是判别成else嵌套的if，即可处理。

---

ZJR 12.15

支持过程作为参数。文档见报告，例子是ex25。

很秀的功能，嗯！





  













