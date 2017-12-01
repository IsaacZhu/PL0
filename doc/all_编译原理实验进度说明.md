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