void testtable()	//print table //zjr 11.17 //test code
{
	mask *mk;
	array *arr;
	dimensionHead *dhead;
	dimension *dim;
	int flag=0;
	int i;
	//tmppram mode
	/*for (i=0;i<funcparam;++i)	//tx for symbol table   funcparam for tmpparam
	{
		if (tmpparam[i].kind==ID_VARIABLE)
		{
			flag=0;
			mk=(mask *)&tmpparam[i];
			printf("%d :variable %s\n",i,mk->name);
		}
		else if (tmpparam[i].kind==ID_ARRAY)
		{
			if (flag==0)	//head
			{
				arr=(array *)&tmpparam[i];
				printf("%d :array %s level:%d\n",i,arr->name,arr->level);
				flag=1;
			}
			else if (flag==1)	//dim head
			{
				dhead=(dimensionHead *)&tmpparam[i];
				printf("%d :array %s depth:%d\n",i,dhead->name,dhead->depth);
				flag=2;
			}
			else
			{
				dim=(dimension *)&tmpparam[i];
				printf("%d :array %s width:%d\n",i,dim->name,dim->width);
			}
		}
	}
*/
	//symbol table mode
	for (i=0;i<=tx;++i)	//tx for symbol table   funcparam for tmpparam
	{
		if (table[i].kind==ID_VARIABLE)
		{
			flag=0;
			mk=(mask *)&table[i];
			printf("%d :variable %s\n",i,mk->name);
		}
		else if (table[i].kind==ID_ARRAY)
		{
			if (flag==0)	//head
			{
				arr=(array *)&table[i];
				printf("%d :array %s level:%d\n",i,arr->name,arr->level);
				flag=1;
			}
			else if (flag==1)	//dim head
			{
				dhead=(dimensionHead *)&table[i];
				printf("%d :array %s depth:%d\n",i,dhead->name,dhead->depth);
				flag=2;
			}
			else
			{
				dim=(dimension *)&table[i];
				printf("%d :array %s width:%d\n",i,dim->name,dim->width);
			}
		}
		else if (table[i].kind==ID_PARRAY)
		{
			printf("%d :parray %s \n",i,table[i].name);
			flag=0;
		}
		else
		{
			mk=(mask *)&table[i];
			printf("%d :%d %s level:%d addr:%d\n",i,mk->kind,mk->name,mk->level,mk->address);
			flag=0;
		}
	}//table mode
	
	printf("dx is %d ;tx is %d\n",dx,tx);
	//exit(2);
}

void mytest()	//for test //zjr 11.19
{
	printf("here\n");//tc
}

void printsym()
{
	char *symbset[]={
	"SYM_NULL",
	"SYM_IDENTIFIER",
	"SYM_NUMBER",
	"SYM_PLUS",
	"SYM_MINUS",
	"SYM_TIMES",
	"SYM_SLASH",
	"SYM_ODD",
	"SYM_EQU",
	"SYM_NEQ",
	"SYM_LES",
	"SYM_LEQ",
	"SYM_GTR",
	"SYM_GEQ",
	"SYM_LPAREN",
	"SYM_RPAREN",
	"SYM_COMMA",
	"SYM_SEMICOLON",
	"SYM_PERIOD",
	"SYM_BECOMES",
    	"SYM_BEGIN",
	"SYM_END",
	"SYM_IF",
	"SYM_THEN",
	"SYM_WHILE",
	"SYM_DO",
	"SYM_CALL",
	"SYM_CONST",
	"SYM_VAR",
	"SYM_PROCEDURE",
	"SYM_ELSE",
	"SYM_ELSE_IF",
	"SYM_EXIT",
	"SYM_RETURN",
	"SYM_FOR",
	"SYM_BITAND",		//the folowing two changed by lijiquan
	"SYM_BITOR",
	"SYM_ANTI",
	"SYM_LSBRAC",
	"SYM_RSBRAC",
	"SYM_LBRACE",
	"SYM_RBRACE",
	//=================added by lijiquan
	"SYM_AND",
	"SYM_OR",
	"SYM_LOGIEQU",

	"SYM_BITXOR", 	// ^
	"SYM_MOD",			// %

	//Dong Shi, 11.23, add SYM_INC, SYM_DEC
	"SYM_INC",
	"SYM_DEC",

	"SYM_FORMAT",
	"SYM_PRINTF",
	"SYM_RANDOM",

	//Dong Shi, 12.3, add SYM_INPUT
	"SYM_INPUT",

	//ZJR 12.8 ADD '?' AND ':',WHICH IS SYM_QUES AND SYM_COLON #Z1
	"SYM_QUES",
	"SYM_COLON"
};
	printf("sym is %s\n",symbset[sym]);
}
