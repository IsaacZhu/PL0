// pl0 compiler source code
#pragma warning(disable:4996)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Dong Shi, 12.1, Add time.h
#include <sys/time.h>

#include "pl0.h"
#include "set.c"

#include "zjrtest.c"
//////////////////////////////////////////////////////////////////////
// print error message.
void error(int n)
{
	int i;
	printsym();//tc
	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error

//////////////////////////////////////////////////////////////////////
void getch(void)
{
	if (cc == ll)
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;
		printf("%5d  ", cx);
		while ( (!feof(infile)) // added & modified by alex 01-02-09
			    && ((ch = getc(infile)) != '\n'))
		{
			//annotation recognition added by zjr 2017.9.17
			// /*(digits|symbols)**/
			if (ch=='/')	
			{
				ch=getc(infile);
				if (ch=='*')		
				{
					char tmpch='\0';	//record the former one char of ch
					while (ch!='/')
					{
						tmpch=ch;
						ch=getc(infile);						
					}
					if (tmpch!='*')	//error
					{
						printf("Unable to match anotation symbol!\n");
						exit(1);
					}
					ch=getc(infile);
					if (ch=='\n') 
					{
						break;	//end of line
					}
				}//if ch=='*'
				else if (ch=='/')		// "//"anotation
				{
					while (ch!='\n'){
						ch=getc(infile);
					}
					break;
				}//else if
				else // is a '/'	//zjr 11.25
				{
					printf("%c", '/'); 
					line[++ll] = '/';
					printf("%c",ch);
					line[++ll]=ch;
					continue;
				}
			}// if ch == '/'
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = ' ';
	}
	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym(void)
{
	int i, k;
	char a[MAXIDLEN + 1];
	char lastChar;

	while (ch == ' '||ch == '\t')
		getch();

	//Dong Shi, 11.23, Add ++ and --
	lastChar = ch;
	if (ch == '+')
	{
		getch();
		if(ch == '+')
		{
			sym = SYM_INC;
			getch();
			return;
		}
		else
		{
			sym = SYM_PLUS;
			return;
		}
	}
	else if (ch == '-')
	{
		getch();
		if(ch == '-')
		{
			sym = SYM_DEC;
			getch();
			return;
		}
		else
		{
			sym = SYM_MINUS;
			return;
		}
	}
	// Dong Shi, 12.1, Add "..." recognize
	else if(ch == '\"')
	{
		int count = 0;
		char last = '\"';
		getch();
		while(!(ch == '\"' && last != '\\') && count < 128)
		{
			++ count;
			tmpStack[count] = ch;
			last = ch;
			getch();
		}
		tmpStack[0] = count;

		sym = SYM_FORMAT;
		getch();
		return;
	}
	ch = lastChar;

	if (isalpha(ch))
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		}
		while (isalpha(ch) || isdigit(ch));
		a[k] = 0;
		strcpy(id, a);
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--]));
		if (++i)
			sym = wsym[i]; // symbol is a reserved word
		else
			sym = SYM_IDENTIFIER;   // symbol is an identifier
	}
	else if (isdigit(ch))
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch();
		}
		while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BECOMES; // :=
			getch();
		}
		else
		{
			//sym = SYM_NULL;       // illegal?
			sym = SYM_COLON;	//':' //zjr 12.8 //#Z2
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			getch();
		}
		//right shift op //added by zjr //12.8 #Z6 
		else if (ch == '>')
		{
			sym=SYM_RSHIFT;
			getch();
		}
		else
		{
			sym = SYM_GTR;     // >
		}
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_LEQ;     // <=
			getch();
		}
		else if (ch == '>')
		{
			sym = SYM_NEQ;     // <>
			getch();
		}
		//left shift op //added by zjr //12.8 #Z6 
		else if (ch == '<')
		{
			sym=SYM_LSHIFT;
			getch();
		}
		else
		{
			sym = SYM_LES;     // <
		}
	}
	else if (ch == '&')
	{
		getch();
		if (ch == '&')
		{
			sym = SYM_AND;	  	//&&
			getch();
		}
		else
		{
			sym = SYM_BITAND;	//&
		}
	}
	else if (ch == '|')
	{
		getch();
		if (ch == '|')
		{
			sym = SYM_OR;		//||
			getch();
		}
		else
		{
			sym = SYM_BITOR;	//|
		}
	}
	else if (ch == '=')			//ljq
	{
		getch();
		if(ch == '=')
		{
			sym = SYM_LOGIEQU;	//==
			getch();
		}
		else
		{
			sym = SYM_EQU;		//=
		}
	}
	else if (ch == '^')
	{
		getch();
		sym = SYM_BITXOR;
	}
	else if (ch == '%')
	{
		getch();
		sym = SYM_MOD;
	}

	else
	{ // other tokens
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
			sym = ssym[i];
			getch();
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym

//////////////////////////////////////////////////////////////////////
// generates (assembles) an instruction.
void gen(int x, int y, int z)
{
	if (cx > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx++].a = z;
} // gen

//////////////////////////////////////////////////////////////////////
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(symset s1, symset s2, int n)
{
	symset s;

	if (! inset(sym, s1))
	{
		error(n);
		s = uniteset(s1, s2);
		while(! inset(sym, s))
			getsym();
		destroyset(s);
	}
} // test

//////////////////////////////////////////////////////////////////////
//int dx;  // data allocation index
//moved to pl0.h //zjr 11.17


// enter object(constant, variable or procedre) into table.
void enter(int kind)
{
	mask* mk;
	array* arr;
	dimensionHead* dhead;
	dimension* dim;
	int arraySize = 1;

	//check if there's parameters who has same name	//zjr 11.21
	int pos=position(id);
	int i;
	if (pos!=0)
	{
		mk=(mask *)&table[pos];
		if (mk->level==level)		//same level, error
		{
			error(34);
			return;
		}
		else				//different level,cover it
		{
			if (mk->kind==ID_VARIABLE||mk->kind==ID_PROCEDURE)
			{
				strcpy(mk->name,"/cover");//delete this variable
			}
			else	//array or parray
			{
				arr=(array *)&table[pos];
				strcpy(arr->name,"/cover");
				pos++;
				dhead=(dimensionHead *)&table[pos];
				strcpy(dhead->name,"/cover");
				for (i=0;i<dhead->depth;++i)
				{
					pos++;
					dim=(dimension *)&table[pos];
					strcpy(dim->name,"/cover");
				}
			}
		}
	}
	

	tx++;
	strcpy(table[tx].name, id);
	table[tx].kind = kind;
	switch (kind)
	{
	case ID_CONSTANT:
		if (num > MAXADDRESS)
		{
			error(25); // The number is too great.
			num = 0;
		}
		table[tx].value = num;
		break;
	case ID_VARIABLE:
		mk = (mask*) &table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
	case ID_PROCEDURE:
		strcpy(funcname, id);	//record function name temporily //added by zjr 17.10.27
		mk = (mask*) &table[tx];
		mk->level = level; 
		break;
	case ID_ARRAY:					//<===================added for array===============
		arr = (array *) &table[tx];
		arr->level = level;
		arr->address = dx;
		tx++;
		strcpy(table[tx].name, id);
		table[tx].kind = kind;
		dhead = (dimensionHead*) &table[tx];
		dhead->depth = 0;
		do
		{
			dhead->depth ++;
			getsym();
			if (sym == SYM_NUMBER)
			{
				tx ++;
				strcpy(table[tx].name, id);
				table[tx].kind = kind;
				dim = (dimension*) &table[tx];
				dim->width = num;
				arraySize = arraySize * num;
				getsym();
			}
			else
			{
				error(28);
			}

			if (sym == SYM_RSBRAC)
			{
				getsym();
			}
			else
			{
				error(29);
			}
		}
		while(sym == SYM_LSBRAC);
		dx = dx + arraySize;
		break;
	} // switch
} // enter

//////////////////////////////////////////////////////////////////////
// locates identifier in symbol table.
int position(char* id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	if(table[i].kind == ID_ARRAY){			//倒着找的，数组要特别处理
		while (strcmp(table[i - 1].name, id) == 0 && i!=1)	//modified by zjr //11.17
			i --;
		if (i==1)	//has been arrived head of symbol table
		{
			if (table[i].kind==ID_ARRAY && strcmp(table[1].name, id) == 0)	return 1;
			else return 0;
		}
	}
	//handle PARRAY //zjr 11.19 //#Z15
	else if(table[i].kind==ID_PARRAY)
	{
		while (strcmp(table[i - 1].name, id) == 0 && i!=1)	
			i --;
		if (i==1)
		{
			if (table[i].kind==ID_PARRAY && strcmp(table[1].name, id) == 0)	return 1;
			else return 0;
		}
	}
	return i;
} // position

//added by zjr //for arguments passing //11.17 //#Z3
int old_position(char *id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
}
//////////////////////////////////////////////////////////////////////
//declare a const 
//called by block()
void constdeclaration()
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES)	//可能有Bug
		{
			if (sym == SYM_BECOMES)
				error(1); // Found ':=' when expecting '='.
			getsym();
			if (sym == SYM_NUMBER)
			{
				enter(ID_CONSTANT);
				getsym();
			}
			else
			{
				error(2); // There must be a number to follow '='.
			}
		}
		else
		{
			error(3); // There must be an '=' to follow the identifier.
		}
	} else	error(4);
	 // There must be an identifier to follow 'const', 'var', or 'procedure'.
} // constdeclaration

//////////////////////////////////////////////////////////////////////
void vardeclaration(void)
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_LSBRAC)
		{
			enter(ID_ARRAY);	//ljq
		}
		else
		{
			enter(ID_VARIABLE);
		}
	}
	else
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////
//able to output exact OPR code now //zjr 11.27
void listcode(int from, int to)
{
	int i;
	char str[10];
	char *oprset[24]={"OPR_RET", "OPR_NEG", "OPR_ADD", "OPR_MIN",
	"OPR_MUL", "OPR_DIV", "OPR_ODD", "OPR_LOGIEQU",
	"OPR_NEQ", "OPR_LES", "OPR_LEQ", "OPR_GTR",
	"OPR_GEQ", 
	"OPR_AND", "OPR_OR", "OPR_ANTI",
	"OPR_BITAND","OPR_BITOR","OPR_BITXOR","OPR_MOD",
	"OPR_INC", "OPR_DEC","OPR_LSH","OPR_RSH"};	//add OPR_LSH and OPR_RSH //zjr 12.8 #Z10
	printf("\n");
	for (i = from; i < to; i++)
	{
		//zjr 11.27
		if (code[i].f==1 && code[i].a<24)	//OPR
		{
			printf("%5d %s\t%d\t%s\n", i, mnemonic[code[i].f], code[i].l, oprset[code[i].a]);
		}
		else
		printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
	printf("\n");
} // listcode

//////////////////////////////////////////////////////////////////////
void factor(symset fsys)
{
	void expression(symset fsys);
	void logi_or_expression(symset fsys);
	int i, depth;/**/
	symset set, set1;

	int thisLevel;
	int thisAddress;

	//Dong Shi, 10.29, disable "factor cannot appear without a statement" check
	//test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	inset(sym, facbegsys);
	//while (inset(sym, facbegsys))
	//{
		if (sym == SYM_IDENTIFIER)
		{
			if ((i = position(id)) == 0)
			{
				error(11); // Undeclared identifier.
			}
			else
			{
				switch (table[i].kind)
				{
					mask* mk;
					array *arr;/**/
					dimensionHead* dhead;
					dimension* dim;
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
					break;
				case ID_VARIABLE:
					mk = (mask*) &table[i];
					gen(LOD, level - mk->level, mk->address);	
					//Dong Shi, 12.3, store tmp level and address (for inc or dec)
					activeLevel = level - mk->level;
					activeAddress = mk->address;
					thisLevel = level - mk->level;
					thisAddress =  mk->address;
					break;
				case ID_PROCEDURE:
					//Dong Shi, 10.29, Handle Procedure
					//error(21); // Procedure identifier can not be in an expression.
					//Dong Shi, 10.29, copy (zjr 10.27) "call" work to here
					//support parameters now //modified by zjr 17.10.27
					getsym();
					if (sym == SYM_LPAREN)
					{
						int *paralist=nodeplist(id);	//get paralist	//zjr 11.17	//#Z14
						int pnum=0;			//record parameters number
						int papos,j,paramnum;
						paramnum=nodeparam(id);
						mask *basemask;
						basemask=(mask *)&table[position("/pbase")];
						gen(ASTO,0,basemask->address);	//store top to base

						getsym();
						
						if (sym != SYM_RPAREN)			//call pro()
						{ 
							symset s1;
							while (sym != SYM_RPAREN)	//not ")"
							{
								if (sym == SYM_COMMA)	
								{
									getsym();
									continue;
								}
								else					//#1
								{
									//handle array type parameter	//zjr 11.17 //#Z14
									if (paralist[pnum]==ID_ARRAY) //#2
									{
										papos=position(id);
										if (papos==0)	error(11);	//undeclared
										
										else if (table[papos].kind==ID_PARRAY)
										{
											arr=(array *)&table[papos];
											gen(LOD,level-arr->level,arr->address);			//load start address of array from stack
											gen(APOP,0,basemask->address);
											papos++;
											dhead=(dimensionHead *)&table[papos];	//to get num of dimension
											gen(LIT,0,dhead->depth);
											gen(APOP,0,basemask->address);
											for (j=0;j<(dhead->depth);++j)		//put dimensions to stack
											{
												papos++;
												dim=(dimension *)&table[papos];
												gen(LIT,0,dim->width);
												gen(APOP,0,basemask->address);
											}
											pnum+=(dhead->depth+2);	
											getsym();
										}
										else if (table[papos].kind==ID_ARRAY)
										{
											arr=(array *)&table[papos];
											gen(LEA,level-arr->level,arr->address);
											gen(APOP,0,basemask->address);
											papos++;
											dhead=(dimensionHead *)&table[papos];	//to get num of dimension
											gen(LIT,0,dhead->depth);
											gen(APOP,0,basemask->address);
											for (j=0;j<(dhead->depth);++j)		//put dimensions to stack
											{
												papos++;
												dim=(dimension *)&table[papos];
												gen(LIT,0,dim->width);
												gen(APOP,0,basemask->address);
											}
											pnum+=(dhead->depth+2);				
											getsym();
										}
										else error(31);				//type mismatch
									} //#2
									//variable
									else
									{
										s1=createset(SYM_RPAREN,SYM_COMMA,SYM_NULL); //end when meet ")",","
										expression(s1);		//analyse the expression in argument
										gen(APOP,0,basemask->address);	//clear stack
										//getsym();
									}//else variable
									if (pnum>paramnum)	//parameters not compatible 
									{
										error(31);
										break;
									}
									
								}//else not comma #1
							}//while
							//the while end means that sym==')'
							destroyset(s1);
						}// if sym not rparen						
					}// if sym lparen

					mk = (mask*) &table[i];
					gen(CAL, level - mk->level, mk->address);
					break;

				case ID_ARRAY:

					arr = (array*) &table[i];
					gen(LEA, level - arr->level, arr->address);
					i ++;
					dhead = (dimensionHead*) &table[i];
					i ++;
					getsym();
					if(sym != SYM_LSBRAC)
					{
						error(30);
					}
					depth = 1;	
					getsym();
					set1 = createset(SYM_RSBRAC, SYM_LSBRAC, SYM_NULL);
					set = uniteset(set1, fsys);
					expression(set);
					while(sym == SYM_RSBRAC && depth < dhead->depth)
					{
						getsym();
						if(sym != SYM_LSBRAC)
						{
							error(30);
						}
						getsym();
						i ++;
						depth ++;
						dim = (dimension*) &table[i];
						gen(LIT, 0, dim->width);
						gen(OPR, 0, OPR_MUL);
						expression(set);
						gen(OPR, 0, OPR_ADD);
					}
					gen(OPR, 0, OPR_ADD);
					gen(LODAR, 0, 0);
					destroyset(set);
					destroyset(set1);
					//printf("array finished!\n");
					/**************************a load instruction to be loaded*******************/
					break;
				//added by zjr //11.17 //#Z6
				//if it's an array parameter instead of normal array
				case ID_PARRAY:
					mk=(mask *)&table[i];
					gen(LOD,level-mk->level,mk->address);	//get the start address of array
					//int ppos=0;
					i=0;
					while (strcmp(table[++i].name, mk->name) != 0);	//get position of this array
					i++;
					//copy ljq's work
					dhead = (dimensionHead*) &table[i];
					i ++;
					getsym();
					if(sym != SYM_LSBRAC)
					{
						error(30);
					}
					depth = 1;	
					getsym();
					set1 = createset(SYM_RSBRAC, SYM_LSBRAC, SYM_NULL);
					set = uniteset(set1, fsys);
					expression(set);
					while(sym == SYM_RSBRAC && depth < dhead->depth)
					{
						getsym();
						if(sym != SYM_LSBRAC)
						{
							error(30);
						}
						getsym();
						i ++;
						depth ++;
						dim = (dimension*) &table[i];
						gen(LIT, 0, dim->width);
						gen(OPR, 0, OPR_MUL);
						expression(set);
						gen(OPR, 0, OPR_ADD);
					}
					gen(OPR, 0, OPR_ADD);
					gen(LODAR, 0, 0);
					destroyset(set);
					destroyset(set1);

					break;
				} // switch
			}
			getsym();

			//Dong Shi, 12.3, Add ++/-- handling
			if (sym == SYM_INC || sym == SYM_DEC)
			{
				while (sym == SYM_INC || sym == SYM_DEC)
				{
					if(sym == SYM_INC)
						gen(OPR, 0, OPR_INC);
					else
						gen(OPR, 0, OPR_DEC);

					gen(STO, activeLevel, activeAddress);
					gen(LOD, activeLevel, activeAddress);

					if(sym == SYM_INC)
						gen(OPR, 0, OPR_DEC);
					else
						gen(OPR, 0, OPR_INC);
					
					getsym();
				}
			}
			//Dong Shi, 12.3, Add assign handling
			else if (sym == SYM_BECOMES)
			{
				getsym();
				expression(fsys);
				gen(STO, thisLevel, thisAddress);
				gen(LOD, thisLevel, thisAddress);


			}
		}
		else if (sym == SYM_NUMBER)
		{
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			gen(LIT, 0, num);
			getsym();
		}
		else if (sym == SYM_LPAREN)
		{
			lev++;	//ywt 11.28	//level of short-cir-cal
			getsym();
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			expression(set);
			destroyset(set);
			if (sym == SYM_RPAREN)
			{
				lev--;	//ywt 11.28	//level of short-cir-cal
				getsym();

				//Dong Shi, 12.3, Add ++/-- handling
				if (sym == SYM_INC || sym == SYM_DEC){
					while (sym == SYM_INC || sym == SYM_DEC)
					{
						if(sym == SYM_INC)
							gen(OPR, 0, OPR_INC);
						else
							gen(OPR, 0, OPR_DEC);

						gen(STO, activeLevel, activeAddress);
						gen(LOD, activeLevel, activeAddress);

						if(sym == SYM_INC)
							gen(OPR, 0, OPR_DEC);
						else
							gen(OPR, 0, OPR_INC);
						
						getsym();
					}
				}			
				//Dong Shi, 12.3, Add assign handling
				else if (sym == SYM_BECOMES)
				{
					getsym();
					expression(fsys);
					gen(STO, thisLevel, thisAddress);
					gen(LOD, thisLevel, thisAddress);
				}
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		//Dong Shi, 12.3, Add ++/-- handling
		else if (sym == SYM_INC || sym == SYM_DEC)
		{
			int INCDECSym = sym;

			getsym();
			factor(fsys);

			if(INCDECSym == SYM_INC)
				gen(OPR, 0, OPR_INC);
			else
				gen(OPR, 0, OPR_DEC);
			
			gen(STO, activeLevel, activeAddress);
			gen(LOD, activeLevel, activeAddress);
		}
		else if(sym == SYM_MINUS) // UMINUS,  Expr -> '-' Expr
		{  
			 getsym();
			 //expression(fsys);
			 factor(fsys);
			 gen(OPR, 0, OPR_NEG);
		}//================added by lijiquan
		else if(sym == SYM_ANTI)	//ANTI, Expr -> '!' Expr
		{
			getsym();
			//expression(fsys);
			factor(fsys);
			gen(OPR, 0, OPR_ANTI);
		}
		else if(sym == SYM_ODD)
		{
			getsym();
			//expression(fsys);
			factor(fsys);
			gen(OPR, 0, 6);
		}
		//Dong Shi, 12.1, Add random
		else if (sym == SYM_RANDOM)
		{
			int region = 2147483647;

			getsym(); // '('
			getsym(); // ')' or parameter
			//Dong Shi, 12.3, Add constant parameter
			if (sym == SYM_NUMBER)
			{
				region = num;

				getsym(); //')'
			}
			
			getsym();

			struct timeval tpstart;
			gettimeofday(&tpstart,NULL);
    		srand(tpstart.tv_usec);

			gen(LIT, 0, rand()%region);
		}
		//test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);
	//} // while
} // factor

//////////////////////////////////////////////////////////////////////
void term(symset fsys)
{
	int mulop;
	symset set;
	
	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_MOD, SYM_NULL));
	factor(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH || sym == SYM_MOD)
	{
		mulop = sym;
		getsym();
		factor(set);
		if (mulop == SYM_TIMES)
		{
			gen(OPR, 0, OPR_MUL);
		}
		else if(mulop == SYM_SLASH)
		{
			gen(OPR, 0, OPR_DIV);
		}else 
		{
			gen(OPR, 0, OPR_MOD);
		}
	} // while
	destroyset(set);
} // term

//////////////////////////////////////////////////////////////////////
void additive_expression(symset fsys)			//<===========================name changed by lijiquan
{
	int addop;
	symset set;
	
	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
	
	term(set);
	while (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_PLUS)
		{
			gen(OPR, 0, OPR_ADD);
		}
		else
		{
			gen(OPR, 0, OPR_MIN);
		}
	} // while

	destroyset(set);
} // addictive_expression

//handle left or right shift expression
//zjr 12.8 #Z7
void shift_expression(symset fsys)
{
	symset set;
	int shiftop;
	set=uniteset(fsys,createset(SYM_LSHIFT,SYM_RSHIFT,SYM_NULL));
	additive_expression(set);
	while (sym == SYM_LSHIFT || sym == SYM_RSHIFT)
	{
		shiftop=sym;
		getsym();
		additive_expression(set);
		if (shiftop == SYM_LSHIFT)	//'<<'
		{
			gen(OPR,0,OPR_LSH);
		}
		else	//'>>'
		{
			gen(OPR,0,OPR_RSH);
		}
	}
}//shift expr

//zjr 11.27
//handle relation expression
void rel_expr(symset fsys)
{
	symset set;
	int relop;
	set = uniteset(fsys, createset(SYM_LOGIEQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL));
	shift_expression(set); //zjr 12.8 #Z8
	while(sym==SYM_GTR||sym==SYM_LOGIEQU
			||sym==SYM_NEQ||sym==SYM_LES||sym==SYM_GEQ
			||sym==SYM_LEQ)
	{
		relop=sym;
		getsym();
		shift_expression(set);	//zjr 12.8 #Z8
		switch (relop)
		{
			case SYM_LOGIEQU:			
				gen(OPR, 0, OPR_LOGIEQU);
				break;
			case SYM_NEQ:
				gen(OPR, 0, OPR_NEQ);
				break;
			case SYM_LES:
				gen(OPR, 0, OPR_LES);
				break;
			case SYM_GEQ:
				gen(OPR, 0, OPR_GEQ);
				break;
			case SYM_GTR:
				gen(OPR, 0, OPR_GTR);
				break;
			case SYM_LEQ:
				gen(OPR, 0, OPR_LEQ);
				break;
		} // switch
	}
	destroyset(set);
}//rel_expr

void bit_and_expr(symset fsys) // Bit operator and '&'
{
	symset set;
	set = uniteset(fsys, createset(SYM_BITAND, SYM_NULL));
	//addictive_expression(set);
	rel_expr(set);	//zjr 11.27
	while(sym == SYM_BITAND)
	{
		getsym();
		//addictive_expression(set);
		rel_expr(set);	//zjr 11.27
		gen(OPR, 0, OPR_BITAND);
	}
	destroyset(set);
}// bit_and_expr

void bit_xor_expr(symset fsys) // Bit operator xor '^'
{
	symset set;
	set = uniteset(fsys, createset(SYM_BITXOR, SYM_NULL));
	bit_and_expr(set);
	while(sym == SYM_BITXOR)
	{
		getsym();
		bit_and_expr(set);
		gen(OPR, 0, OPR_BITXOR);
	}
	destroyset(set);
}

void bit_or_expr(symset fsys)	//Bit operator or '|'
{
	symset set;
	set = uniteset(fsys, createset(SYM_BITOR, SYM_NULL));
	bit_xor_expr(set);
	while(sym == SYM_BITOR)
	{
		getsym();
		bit_xor_expr(set);
		gen(OPR, 0, OPR_BITOR);
	}//while
	destroyset(set);
}//bit_or_expr


//============================================added by lijiquan====================================
void logi_and_expression(symset fsys)//----change by ywt 2017.10.25
{//deal with logical "and"
	symset set;
	set = uniteset(fsys, createset(SYM_AND, SYM_NULL));
	//addictive_expression(set);
	bit_or_expr(set);
	while (sym == SYM_AND)
	{ 
		if(sign_condition)
		{
			//ywt
		    sign_and[lev]++;              //用于记录and逻辑符的计算次数，并作为跳转指针
		    cx6[lev][sign_and[lev]]=cx;        //记录执行到and逻辑符时的cx值
		    gen(JLEZ,0,0);
		}
		getsym();
		//addictive_expression(set);
		bit_or_expr(set);
		gen(OPR, 0, OPR_AND);
	} // while
	destroyset(set);
}//logi_and_expression

void logi_or_expression(symset fsys)//----change by ywt,2017.10.25
{//deal with logical "or"
	symset set;
	set = uniteset(fsys, createset(SYM_OR, SYM_NULL));
	//mytest();//tc
	//listcode(0,cx);//tc
	logi_and_expression(set);
	while (sym == SYM_OR)
	{
		if(sign_condition)
		{
			//ywt
			while(sign_and[lev])                      //判断前面的符号是否存在and，若存在，则让上一个and继续执行
	      	{
				  /*code[cx6[sign_and]].a=cx6[sign_and]+1;*/ code[cx6[lev][sign_and[lev]]].a=cx+1;
			      sign_and[lev]--;
		    }
		    sign_or[lev]++;                     //用于记录or逻辑符的计算次数，并作为跳转指针
		    cx7[lev][sign_or[lev]]=cx;               // //记录执行到or逻辑符时的cx值
		    gen(JGZ,0,0);
		}
		getsym();
		logi_and_expression(set);
		gen(OPR, 0, OPR_OR);
	} // while
	destroyset(set);
}//logi_or_expression

//declare conditon() //zjr //12.8 #Z4
void condition(symset fsys);

void expression(symset fsys)
{//nothing just to make sure we don't need to change the function name in other functions
	symset set;

	set = uniteset(fsys, createset(SYM_NULL,SYM_QUES));		//we can change it later //zjr 12.8 #Z4
	logi_or_expression(set);			//ATTENTION, if you add something whose priority is larger than "||", change it!!
	
	//支持？ ： 表达式 zjr 12.8 //#Z4
	if (sym==SYM_QUES)			//'?'
	{    
		getsym();						
        int falsecx,s2endcx;    //记录出口地址
        falsecx=cx;
		gen(JLEZ,0,0);
        symset set2=uniteset(fsys,createset(SYM_COLON,SYM_NULL));
        expression(set2);        //在我们的代码中应该是condition_expr，因为我们这个代码的优先级错了
        if (sym==SYM_COLON)		//':' 
		{   
			getsym();
			s2endcx=cx;
            gen(JMP,0,0);        //有待回填
            code[falsecx].a=cx;        //回填第一条跳转的地址
			expression(fsys);		   //此处应当是condition_expr
            code[s2endcx].a=cx;        //回填那条JMP的跳转地址
        }//if colon
		else
		{
			error(40);
		}
    }//if ques

	destroyset(set);
} // expression
//===================================================================================

//////////////////////////////////////////////////////////////////////
void condition(symset fsys)//---change by ywt,2017.10.25
{
	int relop,i;
	symset set;
    sign_condition=1;
	// if (sym == SYM_ODD)
	// {
	// 	getsym();
	// 	expression(fsys);
	// 	gen(OPR, 0, 6);
	// }
	// else
	{
		//set = uniteset(relset, fsys);
		//set=uniteset(createset(SYM_NULL),fsys);//zjr 11.27
		memcpy(&set,&fsys,1*sizeof(symset));//zjr 11.27
		expression(set);
		//destroyset(set);
		if (! inset(sym, relset))
		{
			//error(20);
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys);
			switch (relop)
			{
			case SYM_LOGIEQU:			//modified by ljq
				gen(OPR, 0, OPR_LOGIEQU);
				break;
			case SYM_NEQ:
				gen(OPR, 0, OPR_NEQ);
				break;
			case SYM_LES:
				gen(OPR, 0, OPR_LES);
				break;
			case SYM_GEQ:
				gen(OPR, 0, OPR_GEQ);
				break;
			case SYM_GTR:
				gen(OPR, 0, OPR_GTR);
				break;
			case SYM_LEQ:
				gen(OPR, 0, OPR_LEQ);
				break;
			} // switch
		} // else
		destroyset(set);
	} // else
	for(i=0;i<5;i++)
	{
	for(;sign_or[i]>0;)//用于回填JMP_or跳转地址	//ywt
	{
		 code[cx7[i][sign_or[i]]].a=cx+1;
		 sign_or[i]--;
	}
	}
	sign_condition=0;    
} // condition

/////////////////////////////////////
void Endcondition(int JPcx)//add by ywt 2017.10.25,用于回填JMP_and跳转地址
{
	int i;
	for(i=0;i<5;i++)
	{
	for(;sign_and[i]>0;)//
	{ 
		code[cx6[i][sign_and[i]]].a=JPcx;
		sign_and[i]--;
	 } 
	}
}

//Dong Shi, 12.1, add format translate
void formatTranslate(){
	int length = tmpStack[0];
	int i;
	int count = 0;
	char current, next, id;

	for(i = 1; i <= length; ++ i){
        current = tmpStack[i];

		switch(current){
			case '\\':
			++ i;
			next = tmpStack[i];
			switch(next){
				case '@':
				++ count;
				IOStack[IOStackNum][count] = '@';
				break;
				case 'a':
				++ count;
				IOStack[IOStackNum][count] = '\a';
				break;
				case 'b':
				++ count;
				IOStack[IOStackNum][count] = '\b';
				break;
				case 'n':
				++ count;
				IOStack[IOStackNum][count] = '\n';
				break;
				case 'r':
				++ count;
				IOStack[IOStackNum][count] = '\r';
				break;
				case 't':
				++ count;
				IOStack[IOStackNum][count] = '\t';
				break;
				case '\"':
				++ count;
				IOStack[IOStackNum][count] = '\"';
				break;
				case '\'':
				++ count;
				IOStack[IOStackNum][count] = '\'';
				break;
				case '\\':
				++ count;
				IOStack[IOStackNum][count] = '\\';
				break;
			}
			break;
			case '@':
			++ i;
			id = '0' - tmpStack[i];
			++ count;
			IOStack[IOStackNum][count] = id-1;
            break;
            default:
            ++ count;
            IOStack[IOStackNum][count] = current; 
            break;
		}
	}

	IOStack[IOStackNum][0] = count;
}

//////////////////////////////////////////////////////////////////////
void statement(symset fsys)
{
	int i, cx1, cx2,cx3,cx4,cx5,depth,cx6;
	symset set1, set;
	void formatTranslate();

	int thisLevel, thisAddress;

	//Dong Shi, 12.3, initial AssignStackTop
	AssignStackTop = 0;

	if (sym == SYM_IDENTIFIER)
	{ // variable assignment
		mask* mk;
		array* arr;
		dimensionHead* dhead;
		dimension* dim;
		if (! (i = position(id)))
		{
			error(11); // Undeclared identifier.
		}
		//Dong Shi, 10.29, Add support for naked procedure call 
		else if (table[i].kind == ID_PROCEDURE)
		{
			//Dong Shi, 10.29, copy (zjr 10.27) "call" work to here
			//support parameters now //modified by zjr 17.10.27
			getsym();
			//Argument expression available now!//zjr 17.11.2
			if (sym == SYM_LPAREN)
			{	
				mask *basemask;
				basemask=(mask *)&table[position("/pbase")];
				gen(ASTO,0,basemask->address);	//store top to base

				getsym();
						
				if (sym != SYM_RPAREN)			//call pro()
				{ 
					symset s1;
					while (sym != SYM_RPAREN)	//not ")"
					{
						if (sym == SYM_COMMA)
						{	
							getsym();
							continue;
						}
						else
						{
							s1=createset(SYM_RPAREN,SYM_COMMA,SYM_NULL); //end when meet ")",","
							expression(s1);		//analyse the expression in argument
						}
					}//while
					//the while end means that sym==')'
					destroyset(s1);
				}// if sym not rparen						
			}// if sym lparen
			mk = (mask*) &table[i];
			gen(CAL, level - mk->level, mk->address);
			getsym();
			return;
		}
		//support ID_PRRAY //zjr //11.17 //#Z8
		else if (table[i].kind != ID_VARIABLE && table[i].kind != ID_ARRAY && table[i].kind != ID_PARRAY)	//IF CONST
		{
			error(12); // Illegal assignment.
			i = 0;
		}		
		
		getsym();
		if(table[i].kind == ID_ARRAY)	//处理数组取值
		{
			arr = (array*) &table[i];
			gen(LEA, level - arr->level, arr->address);
			i ++;
			dhead = (dimensionHead*) &table[i];
			i ++;
			if(sym != SYM_LSBRAC)
			{
				error(30);
			}
			depth = 1;	
			getsym();
			set1 = createset(SYM_RSBRAC, SYM_LSBRAC, SYM_NULL);
			set = uniteset(set1, fsys);
			expression(set);
			while(sym == SYM_RSBRAC && depth < (dhead->depth))
			{
				getsym();
				if(sym != SYM_LSBRAC)
				{
					error(30);
				}
				getsym();
				i ++;
				depth ++;
				dim = (dimension*) &table[i];
				gen(LIT, 0, dim->width);
				gen(OPR, 0, OPR_MUL);
				expression(set);
				gen(OPR, 0, OPR_ADD);
			}
			gen(OPR, 0, OPR_ADD);
			destroyset(set);
			destroyset(set1);
			getsym();
		}//IF ID_ARRAY
		//added by zjr //11.17 //#Z8
		//handle that if a left value is array parameter
		else if (table[i].kind==ID_PARRAY)
		{
			mk=(mask *)&table[i];
			gen(LOD,level-mk->level,mk->address);	//get the start address of array
			int ppos=0;
			while (strcmp(table[++ppos].name, mk->name) != 0);	//get position of this array
			ppos++;
			//copy ljq's work
			dhead = (dimensionHead*) &table[ppos];
			ppos ++;
			if(sym != SYM_LSBRAC)
			{
				error(30);
			}
			depth = 1;	
			getsym();
			set1 = createset(SYM_RSBRAC, SYM_LSBRAC, SYM_NULL);
			set = uniteset(set1, fsys);
			expression(set);
			while(sym == SYM_RSBRAC && depth < dhead->depth)
			{
				getsym();
				if(sym != SYM_LSBRAC)
				{
					error(30);
				}
				getsym();
				ppos ++;
				depth ++;
				dim = (dimension*) &table[ppos];
				gen(LIT, 0, dim->width);
				gen(OPR, 0, OPR_MUL);
				expression(set);
				gen(OPR, 0, OPR_ADD);
			}
			gen(OPR, 0, OPR_ADD);
			destroyset(set);
			destroyset(set1);
			getsym();
		}
		else  	//id_variable
		{		
			mk = (mask*) &table[i];
			//Dong Shi, 12.3, record mk
			activeLevel = level - mk->level;
			activeAddress = mk->address;
			thisLevel = level - mk->level;
			thisAddress = mk->address;
		}

		if (sym == SYM_BECOMES)
		{
			getsym();
		}
		//Dong Shi, 11.23, Add var ++ and var --
		//Dong Shi, 12.3, recode the implement of inc and dec
		else if (sym == SYM_INC || sym == SYM_DEC)
		{
			gen(LOD, activeLevel, activeAddress);

			//Dong Shi, 12.3, Add ++/-- handling
			while (sym == SYM_INC || sym == SYM_DEC)
			{
				if(sym == SYM_INC)
					gen(OPR, 0, OPR_INC);
				else
					gen(OPR, 0, OPR_DEC);

				getsym();
			}

			gen(STO, activeLevel, activeAddress);

			return;
		}
		//Dong Shi, 12.3, Add list assign
		else if (sym == SYM_COMMA)
		{
			++ AssignStackTop;
			AssignStackLeft[2*AssignStackTop] = thisLevel;
			AssignStackLeft[2*AssignStackTop+1] = thisAddress;

			while (sym == SYM_COMMA)
			{
				getsym();
				if (sym == SYM_IDENTIFIER)
				{
					if ((i = position(id)) == 0)
					{
						error(11); // Undeclared identifier.
					}
					else
					{
						if (table[i].kind == ID_VARIABLE)
						{
							mask *tmpMask = (mask*) &table[i];
							++ AssignStackTop;
							AssignStackLeft[2*AssignStackTop] = level - tmpMask->level;
							AssignStackLeft[2*AssignStackTop+1] = tmpMask->address;
						}
						else
						{
							error(12);
						}
					}
				}
			}
		}
		else
		{
			error(13); // ':=' expected.
		}

		expression(fsys);

		//Dong Shi, 12.3, Add list assignment
		int expressionStackCounter = 0;
		while(sym == SYM_COMMA)
		{
			getsym();
			expression(fsys);
		}

		if(AssignStackTop != 0)
		{
			while(AssignStackTop != 0)
			{
				gen(STO, AssignStackLeft[2*AssignStackTop], AssignStackLeft[2*AssignStackTop+1]);
				-- AssignStackTop;
			}
		}
		else if(table[i].kind == ID_VARIABLE)
		{
			if (i)
			{
				gen(STO, level - mk->level, mk->address);
			}
		}
		//add situation of ID_PARRAY //11.17 //zjr //#Z8
		else if (table[i].kind == ID_ARRAY||table[i].kind == ID_PARRAY)
		{
			gen(STOAR, 0, 0);
			/* a instuction of storing to be added ***************************************/
		}
	}//ID_VARIABLE
	//Dong Shi, 10.29, Add movement of RET
	else if (sym == SYM_RETURN)
	{
		getsym();
		expression(fsys);
		gen(RET, 0, Func->funcparam);//zjr 11.20
	}
	//Dong Shi, 10.29, remove call
	else if (sym == SYM_CALL)
	{
		error(27); //call not supported anymore
	}
	//Dong Shi, add exit
	else if (sym == SYM_EXIT)
	{
		getsym();
		gen(JMP, 0, 0);
	}
	else if (sym == SYM_IF)
	{ // if statement
		getsym();
		set1 = createset(SYM_THEN, SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		//destroyset(set);	//why? zjr11.27
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
		   Endcondition(code[cx4].a);      //用于回填JMP_and跳转地址                                               
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
		Endcondition(code[cx1].a);    //用于回填JMP_and跳转地址
	}
	else if (sym == SYM_BEGIN)
	{ // block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(10);
			}
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)
		{
			getsym();
		}
		else
		{
			error(17); // ';' or 'end' expected.
		}
	}
	else if (sym == SYM_WHILE)
	{ // while statement
		cx1 = cx;
		getsym();
		set1 = createset(SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		cx2 = cx;
		gen(JPC, 0, 0);
		if (sym == SYM_DO)
		{
			getsym();
		}
		else
		{
			error(18); // 'do' expected.
		}
		statement(fsys);
		gen(JMP, 0, cx1);
		code[cx2].a = cx;
	}
	else if (sym == SYM_FOR)
	{ // IF statement
		getsym();
		if (sym == SYM_LPAREN)
		{
			getsym();
		}
		else
		{
			error(18); // '(' expected.
		}
		statement(fsys);
		getsym();
		cx1 = cx;
		set1 = createset(SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		getsym();
		cx2 = cx;
		gen(JPC, 0, 0);
		set1 = createset(SYM_RPAREN, SYM_NULL);
		set = uniteset(set1, fsys);
		cx6=cx;
		gen(JMP,0,0);
		statement(set);
		gen(JMP,0,cx1);
		if (sym == SYM_RPAREN)
		{
			getsym();
		}
		else
		{
			error(18); // '(' expected.
		}
		destroyset(set1);
		destroyset(set);
		code[cx6].a=cx;
		statement(fsys);
		gen(JMP, 0, cx6+1);
		code[cx2].a = cx;
		Endcondition(code[cx2].a);
	}
	// Dong Shi, 12.3, delete inc and dec implement in statement
	//Dong Shi, 11.23, disable error 19 check(for supporting ++ and --)
	else if(sym==SYM_LBRACE)
	{
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_RBRACE, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(10);
			}
			if(sym!=SYM_RBRACE)
			statement(set);
			else break;
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_RBRACE)
		     getsym();
		else
		{
			error(17); // ';' or 'end' expected.
		}		
	}
	//Dong Shi, 12.1, Add SYM_PRINTF
	else if (sym == SYM_PRINTF)
	{
		int count = 0;
		getsym();
		if (sym == SYM_LPAREN)
		{
			getsym();
			if (sym == SYM_FORMAT)
			{
				formatTranslate();
				getsym();
				while (sym == SYM_COMMA)
				{
					++ count;
					getsym();
					expression(fsys);
				}
				
				gen(OUTS, count, IOStackNum);
				++ IOStackNum;
				
				if (sym != SYM_RPAREN){error(22);}
				getsym();
			}
			else{error(36);}
		}
		else{error(35);}
	}
	//Dong Shi, 12.3, Add SYM_INPUT handling
	else if (sym == SYM_INPUT)
	{
		getsym(); //'('
		getsym(); //ID
		if (sym == SYM_IDENTIFIER)
		{
			if ((i = position(id)) == 0)
			{
				error(11); // Undeclared identifier.
			}
			else
			{
				if (table[i].kind == ID_VARIABLE)
				{
					mask* mk;
					mk = (mask*) &table[i];
					gen(IN, 0, 0);
					gen(STO, level - mk->level, mk->address);
				}
				else
				{
					error(38);
				}
			}
		}
		getsym(); //')'
		getsym();
	}
	//Dong Shi, 12.3, make it possible let a factor be a statement
	else
	{
		// Of course you can have a try to change it into an expression
		factor(fsys);
	}
	//test(fsys, phi, 19);
} // statement

//operation to chain list //added by zjr 17.10.27
void initchainlist()
{
	stlist.level=0;
	stlist.next=NULL;
	stlist.funcparam=0;
	stlist.stable=(comtab *)calloc(TXMAX,sizeof(comtab));
	stlist.localtx=0;
	strcpy(stlist.funcname,"\0");	//zjr 11.20
} 

//enter parameters to symbol table
void param_enter()
{
	int i,pos,j,apos;
	mask *mk;
	dimensionHead *dhead;
	array *arr;
	//for (i=0;i<funcparam;++i)	//modified by zjr 11.17
	i=0;
	while(i<funcparam)
	{
		pos=position(tmpparam[i].name); //get position
		if (pos==0)	//this parameter don't exist ->  build a new table item
		{	
			//support array now //zjr 11.17 #Z2
			if (tmpparam[i].kind==ID_ARRAY)
			{
				//array name
				memcpy(&table[++tx],&tmpparam[i],1*sizeof(comtab));
				mk=(mask *)&table[i];
				arr=(array *)&tmpparam[i];
				mk->kind=ID_PARRAY;			//zjr 11.19 #Z17
				mk->level=level;
				mk->address=dx;
				dx++;
				i++;
				//read dimhead
				dhead=(dimensionHead *)&tmpparam[i];
				memcpy(&table[++tx],&tmpparam[i],1*sizeof(comtab));
				table[tx].kind=ID_PARRAY;	//zjr 11.19 #Z17
				dx++;
				//i++;
				//read width of every dim
				for (j=0;j<dhead->depth;++j)	
				{
					i++;
					memcpy(&table[++tx],&tmpparam[i],1*sizeof(comtab));
					table[tx].kind=ID_PARRAY;	//zjr 11.19 #Z17
					//i++;
					dx++;
				}
			}//if id_array
			//it's ID_VARAIBLE
			else
			{
				strcpy(table[++tx].name,tmpparam[i].name);
				table[tx].kind=ID_VARIABLE;
				mk=(mask *)&table[tx];
				mk->level=level;
				mk->address=dx++;	//data +1	
			}
		}//if pos==0
		else	//else -> change its level and address, make it a local parameter
		{
			if (tmpparam[i].kind==ID_ARRAY)	//read an array need to 
			{
				//read array head
				//cover the former array //#Z17
				arr=(array *)&table[pos];				
				arr->level=level;
				arr->address=dx;	
				arr->kind=ID_PARRAY;
				apos=pos;
				apos++;
				dx++;
				//read dim head
				dhead=(dimensionHead *)&table[apos];
				//#Z17
				dhead->kind=ID_PARRAY;
				dx++;
				for (j=0;j<dhead->depth;++j)	
				{
					apos++;
					table[apos].kind=ID_PARRAY;	//zjr 11.19 #Z17
					dx++;
				}

				i+=(dhead->depth+1);
			}
			else	//It's variable.Cover it!
			{
				table[pos].kind=ID_VARIABLE;
				mk=(mask *)&table[pos];
				mk->level=level;
				mk->address=dx++;
			}
		}//else
		++i;
	}//while
}

void nodeinsert()
{
	stnode *P; //new node
	stnode *Q;

	int i;
	int *paralist;
	Q=Func;
	P=(stnode *)malloc(sizeof(stnode));//zjr 11.20
	P->stable=(comtab *)calloc(TXMAX,sizeof(comtab));	
	
	memcpy(P->stable,Q->stable,TXMAX*sizeof(comtab));
	table=P->stable;	//point to new symbol table
	param_enter();		//copy parameters
	P->level=level; 
	P->next=Q->next;	//#Z11
	Q->next=P;			//zjr 11.20
	P->funcparam=funcparam;
	P->localtx=tx;		//need more consideration
	strcpy(P->funcname,funcname);
	
	//add paralist to new node	//zjr 11.17 	//#Z13
	paralist=P->paralist;
	for (i=0;i<funcparam;++i)
	{
		paralist[i]=tmpparam[i].kind;
	}	
	Func=P;			//control
}
//old node stay in list instead of deleting //in order to get paralist 
//zjr 11.17//#Z10
void nodedelete()
{
	stnode *P;
	stnode *Q;
	P=&stlist;
	if (P->next==NULL) //only head exist
	{
		return;
	}
	/*Q=P->next;
	while (Q->next!=NULL) //look for tail
	{
		P=P->next;
		Q=Q->next;
	}
	strcpy(funcname,Q->funcname);
	free(Q);*/
	Q=Func;
	while (P->next!=Q) //look for prior of Q
	{
		P=P->next;
	}
	//P->next=NULL;
	table=P->stable;
	tx=P->localtx;
	Func=P;		    //change control	
}

//get a paralist from one of list node //zjr 11.17 //#Z12
//input : name of procedure
//output : int *paralist
int* nodeplist(char *name)
{
	stnode *P;
	P=&stlist;
	while(P!=NULL && strcmp(P->funcname,name)!=0) P=P->next;	//look for procedure "name"
	if (P==NULL)	//procedure not found
	{
		error(33);
		return NULL;
	}
	return P->paralist;
}

//return amount of parameters of a procedure //zjr 11.19 //#Z16
int nodeparam(char *name)
{
	stnode *P;
	P=&stlist;
	while (P!= NULL&&strcmp(P->funcname,name)!=0) P=P->next;
	if (P==&stlist || P==NULL) return 0; 	//no function error
	//P->funcparam=funcparam;
	return P->funcparam;
}
//end of operation to chain list
	
//////////////////////////////////////////////////////////////////////
void block(symset fsys)
{
	int cx0; // initial code index
	mask* mk;
	int block_dx;
	int savedTx;
	symset set1, set;

	dx = 3;
	block_dx = dx;
	
	tmptx=tx;
	nodeinsert();	//create a new node //added by zjr 17.10.27
	funcparam=0;//initialize funcparam //added by zjr 17.10.27

	mk = (mask*) &table[tmptx];
	mk->address = cx;
	gen(JMP, 0, 0);

	int sonpos;	//added by zjr 17.10.27
	mask *tmpmask;
	char tmpfuncname[MAXIDLEN + 1];	

	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	do
	{
		if (sym == SYM_CONST)
		{ // constant declarations
			getsym();
			do
			{
				constdeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);
		} // if

		if (sym == SYM_VAR)
		{ // variable declarations
			getsym();
			do
			{
				vardeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);
		} // if
		block_dx = dx; //save dx before handling procedure call!
		while (sym == SYM_PROCEDURE)
		{ // procedure declarations
			getsym();
			if (sym == SYM_IDENTIFIER)
			{
				enter(ID_PROCEDURE);
				strcpy(tmpfuncname,id); 	//record funcname temporarily //added by zjr 17.10.27
				getsym();
			}
			else
			{
				error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
			}

		
			//added by zjr  17.10.26 parameters passing
			if (sym == SYM_LPAREN) 			//that means   procedure xx(
			{
				memset(tmpparam,0,50*sizeof(comtab));
				funcparam=0;
				getsym();
				if (sym != SYM_RPAREN)
				{ 
					while (sym != SYM_RPAREN)	//not ")"
					{
						if (sym == SYM_IDENTIFIER)
						{
							getsym();
							if (sym==SYM_LSBRAC)		//is an array parameter //zjr 11.17 #Z1
							{
								array* arr;
								dimensionHead* dhead;
								dimension* pdim;
								int arraySize = 1;
							
								arr = (array *) &tmpparam[funcparam];
								strcpy(tmpparam[funcparam].name, id);
								arr->kind=ID_ARRAY;	//head
								arr->level = level;
								//arress need to be backpatch
								funcparam++;
								strcpy(tmpparam[funcparam].name, id);	//dim head
								tmpparam[funcparam].kind = ID_ARRAY;
								dhead = (dimensionHead*) &tmpparam[funcparam];
								dhead->depth = 0;
								do	//width of every dimension
								{
									dhead->depth ++;
									getsym();
									if (sym == SYM_NUMBER)
									{
										funcparam ++;
										strcpy(tmpparam[funcparam].name, id);
										tmpparam[funcparam].kind = ID_ARRAY;
										pdim = (dimension*) &tmpparam[funcparam];
										pdim->width = num;
										arraySize = arraySize * num;
										getsym();
									}
									else
									{
										error(28);
									}
									if (sym == SYM_RSBRAC)
									{
										getsym();
									}
									else
									{
										error(29);
									}
								}
								while(sym == SYM_LSBRAC);
								++funcparam;
							}//if lsbrac
							else
							{
								strcpy(tmpparam[funcparam].name,id);
								tmpparam[funcparam].kind=ID_VARIABLE;
								++funcparam;
								//getsym();	//next sym
							}
						}//if identifier
						else if (sym == SYM_COMMA)
						{
							getsym();	//next sym
							continue;
						}
						else 			//not "," ! an error occured
						{
							error(5); 	//missing ","    //need to be changed to be better
						}
					}//while not rparen
					getsym();
				}
				else 					//procedure xx()	
				{
					funcparam=0;
					getsym();	//next sym
				}
			}//if lparen
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}

			level++;
			savedTx = tx;
			
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);
			tx = savedTx;
			level--;
			
			//deliver address to father symbol table  //added by zjr 17.10.27
			sonpos=position(tmpfuncname);
			tmpmask=(mask *)&table[sonpos];
			tmpmask->address=tmpaddress;
			
			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}
			
		} // while sym_procedure
		dx = block_dx; //restore dx after handling procedure call!
		
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);
	}
	while (inset(sym, declbegsys));

	code[mk->address].a = cx;
	
	mk->address = cx;
	tmpaddress=cx;	//record address //added by zjr 17.10.27	
	


	cx0 = cx;
	//put base pointer used in arguments passing into symbol table //added by zjr 17.11.2
	int pos=position("/pbase");
	if (pos==0)	// base doesn't exist
	{
		tx++;
		strcpy(table[tx].name, "/pbase");
		table[tx].kind = ID_POINTER;
		mk=(mask *)&table[tx];
		mk->level=level;
		mk->address=dx;
		dx++;
		block_dx++;
	}
	else
	{
		mk=(mask *)&table[pos];
		mk->level=level;
	}
	
	gen(INT, 0, block_dx);
	//gen instructions to load arguments //zjr 17.11.2
	int i;
	for (i=0;i<Func->funcparam;++i)
	{
		gen(LODA,Func->funcparam,i+1);
	}
	
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);

	//gen(OPR, 0, OPR_RET); // return
	//multipop arguments	//modified by zjr //11.21
	gen(OPR,Func->funcparam,OPR_RET);

	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);
	

	nodedelete();	    //a procedure has beened analyzed, delete its symbol table
} // block

//////////////////////////////////////////////////////////////////////
int base(int stack[], int currentLevel, int levelDiff)
{
	int b = currentLevel;
	
	while (levelDiff--)
		b = stack[b];
	return b;
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
// 解释器
void interpret()
{
	int pc;        // program counter
	int stack[STACKSIZE];
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	instruction i; // instruction register

	int tmp;
	//int itr;

	int outsItr;
	int referenceNum;
	int id;
	char outsChar;
	int length;

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 1;
	top = 3;
	stack[1] = stack[2] = stack[3] = 0;
	do
	{
		i = code[pc++];
		switch (i.f)
		{
		case LIT:
			stack[++top] = i.a;
			break;
		case OPR:
			switch (i.a) // operator
			{
			case OPR_RET:
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
				top-=i.l;//add by zjr 11.21
				break;
			case OPR_NEG:
				stack[top] = -stack[top];
				break;
			case OPR_ADD:
				top--;
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN:
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL:
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV:
				top--;
				if (stack[top + 1] == 0)
				{
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] /= stack[top + 1];
				break;
			case OPR_MOD:
				top--;
				if (stack[top + 1] == 0)
				{
					fprintf(stderr, "Runtime Error: Divided by zero when using mod.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] %= stack[top + 1];
				break;
			case OPR_ODD:
				stack[top] %= 2;
				break;
			case OPR_LOGIEQU:
				top--;
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ:
				top--;
				stack[top] = stack[top] != stack[top + 1];
				break; //Dong Shi, 10.29, correct known bug (Add break)
			case OPR_LES:
				top--;
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ:
				top--;
				stack[top] = stack[top] >= stack[top + 1];
				break;//Dong Shi, 10.29, correct known bug (Add break)
			case OPR_GTR:
				top--;
				stack[top] = stack[top] > stack[top + 1];
				break;
			case OPR_LEQ:
				top--;
				stack[top] = stack[top] <= stack[top + 1];
				//================added by lijiquan
				break;
			case OPR_AND:
				top --;
				stack[top] = stack[top] && stack[top + 1];
				break;
			case OPR_OR:
				top --;
				stack[top] = stack[top] || stack[top + 1];
				break;
			case OPR_ANTI:
				stack[top] = !stack[top];
				break;
			case OPR_BITAND:
				top --;
				stack[top] = stack[top] & stack[top + 1];
				break;
			case OPR_BITOR:
				top --;
				stack[top] = stack[top] | stack[top + 1];
				break;
			case OPR_BITXOR:
				top --;
				stack[top] = stack[top] ^ stack[top + 1];
				break;
			//Dong Shi, 11.23, Add op OPR_INC and OPR_DEC
			case OPR_INC:
				++ stack[top];
				break;
			case OPR_DEC:
				-- stack[top];
				break;
			//add left shift and right shift //ZJR 12.8 #Z9
			case OPR_LSH:
				top--;
				stack[top] = stack[top] << stack[top + 1];
				break;
			case OPR_RSH:
				top--;
				stack[top] = stack[top] >> stack[top + 1];
				break;
			} // switch
			break;
		case LOD:
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
		case STO:
			stack[base(stack, b, i.l) + i.a] = stack[top];
			//Dong Shi, 12.1, disable STO output
			//printf("%d\n", stack[top]);
			top--;
			break;
		//Dong Shi, 10.29, Add OP RET
		case RET:
			tmp = stack[top];
			top = b - 1;
			pc = stack[top + 3];
			b = stack[top + 2];
			//++ top;
			++ top;
			top-=i.a;//zjr 11.20
			stack[top] = tmp;
			break;
		case CAL:
			stack[top + 1] = base(stack, b, i.l);
			// generate new block mark
			stack[top + 2] = b;
			stack[top + 3] = pc;
			b = top + 1;
			pc = i.a;
			break;
		case INT:
			top += i.a;
			break;
		case JMP:
			pc = i.a;
			break;
		case JPC:
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		case JLEZ: 			//added by ywt
			if (stack[top] == 0)
			{
				pc = i.a;
			}
			break;
		case JGZ:
			if (stack[top] != 0)
			{
				pc = i.a;
			}
			break;
		case APOP:
			stack[++stack[base(stack,b,0)+i.a]]=stack[top];
			top=stack[base(stack,b,0)+i.a];
			break;
		//put top into /pbase	//zjr 11.2
		case ASTO:
			stack[base(stack,b,0)+i.a]=top;
			break;
		//load argument from stack //zjr 11.2
		case LODA:
			stack[base(stack,b,0)+i.a+2]=stack[base(stack,b,0)+i.a-i.l-1];
			break;
		case LEA:
			stack[++top] = base(stack, b, i.l) + i.a;
			break;
		case LODAR:
			top ++;
			stack[top] = stack[stack[top - 1]];
			break;
		case STOAR:
			stack[stack[top - 1]] = stack[top];
			//Dong Shi, 12.1, disable STOAR output			
			//printf("%d\n", stack[top]);
			top = top - 2;
			break;
		case OUTS:
			id = i.a;
			referenceNum = i.l;
			length = IOStack[id][0];

			for(outsItr = 1; outsItr <= length; ++ outsItr)
			{
				outsChar = IOStack[id][outsItr];
				if(outsChar < 0)
				{
					printf("%d", stack[top-outsChar-referenceNum]);
				}
				else
					printf("%c", IOStack[id][outsItr]);
			}
			top -= referenceNum;
			break;
		case IN:
			++ top;
			scanf("%d", &stack[top]);
			break;
		} // switch
	}
	while (pc);

	printf("End executing PL/0 program.\n");
} // interpret

//////////////////////////////////////////////////////////////////////
int main ()
{
	FILE* hbin;
	char s[80];
	int i;
	symset set, set1, set2;

	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	//创建记号流（词法分析）
	phi = createset(SYM_NULL);
	relset = createset(SYM_LOGIEQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);	//关系符集
	
	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL, SYM_LSBRAC, SYM_RSBRAC);	//声明符
	//Dong Shi, 10.29, remove SYM_CALL, add SYM_RETURN 
	//Dong Shi, 11.25, add exit
	statbegsys = createset(SYM_BEGIN, SYM_RETURN, SYM_IF, SYM_EXIT, SYM_WHILE, SYM_NULL);	//状态符
	//Dong Shi, 10.29, Add SYM_PROCEDURE to factor set
	facbegsys = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NUMBER, SYM_LPAREN, SYM_MINUS, SYM_ANTI, SYM_NULL);   //因子(factor)符

	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	//从输入流中获取一个字符
	getsym();

	set1 = createset(SYM_PERIOD, SYM_NULL);	//
	set2 = uniteset(declbegsys, statbegsys);	//合并声明和状态
	set = uniteset(set1, set2);					//再合并
	
	strcpy(funcname,"main");
	initchainlist();	//initialize to symbol table  //added by zjr 17.10.27
	block(set);

	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
	if (err == 0)
	{
		hbin = fopen("hbin.txt", "w");
		for (i = 0; i < cx; i++)
			fwrite(&code[i], sizeof(instruction), 1, hbin);
		fclose(hbin);
	}
	if (err == 0)
		//执行解释器
		interpret();
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
	listcode(0, cx);
} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
