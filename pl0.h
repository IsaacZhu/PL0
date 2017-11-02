#include <stdio.h>

#define NRW        16     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       17     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     30     // maximum number of symbols  

#define STACKSIZE  1000   // maximum storage

//关键字的枚举类型
enum symtype
{
	SYM_NULL,
	SYM_IDENTIFIER,
	SYM_NUMBER,
	SYM_PLUS,
	SYM_MINUS,
	SYM_TIMES,
	SYM_SLASH,
	SYM_ODD,
	SYM_EQU,
	SYM_NEQ,
	SYM_LES,
	SYM_LEQ,
	SYM_GTR,
	SYM_GEQ,
	SYM_LPAREN,
	SYM_RPAREN,
	SYM_COMMA,
	SYM_SEMICOLON,
	SYM_PERIOD,
	SYM_BECOMES,
    SYM_BEGIN,
	SYM_END,
	SYM_IF,
	SYM_THEN,
	SYM_WHILE,
	SYM_DO,
	SYM_CALL,
	SYM_CONST,
	SYM_VAR,
	SYM_PROCEDURE,
	SYM_ELSE,
	SYM_ELSE_IF,
	SYM_EXIT,
	SYM_RETURN,
	SYM_FOR,
	SYM_BITAND,		//the folowing two changed by lijiquan
	SYM_BITOR,
	SYM_ANTI,
	SYM_LSBRAC,
	SYM_RSBRAC,
	//=================added by lijiquan
	SYM_AND,
	SYM_OR,
	
	SYM_BITXOR, 	// ^
	SYM_MOD			// %
};

//Add ID_POINTER //zjr 17.11.2 
enum idtype
{
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE, ID_POINTER
};

//add PAS: for parameter pass //modified by zjr 17.10.27
//Dong Shi, 10.28, Add RET op
//add APOP: get stack[top] to stack[/pbase+1] and recover top to base+1 //zjr 17.11.2
//add ASTO:store top to /pbase //zjr 11.2
//add LODA: load argument from stack //zjr 11.2
//delete PAS	//zjr 11.2
enum opcode
{
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC,JPC_and,JPC_or,RET,APOP,ASTO,LODA
};

enum oprcode
{
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,
	OPR_GEQ, 
	//================added by lijiquan
	OPR_AND, OPR_OR, OPR_ANTI,
	
	OPR_BITAND,OPR_BITOR,OPR_BITXOR,OPR_MOD
};


typedef struct
{
	int f; // function code
	int l; // level
	int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
char* err_msg[] =
{
/*  0 */    "",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "",
/* 27 */    "Reserved word call is not supported anymore. ",
/* 28 */    "",
/* 29 */    "",
/* 30 */    "",
/* 31 */    "",
/* 32 */    "There are too many levels."
};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read
int  sym;        // last symbol read
char id[MAXIDLEN + 1]; // last identifier read
int  num;        // last number read
int  cc;         // character count
int  ll;         // line length
int  kk;
int  err;
int  cx;         // index of current instruction to be generated.
int  level = 0;
int  tx = 0;

char line[80];

instruction code[CXMAX];
int funcparam=0;

//关键字集
char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while","else","else if","exit","return","for"
};

//关键字代号集，与关键字一一对应
int wsym[NRW + 1] =
{
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE,
	SYM_ELSE,SYM_ELSE_IF,SYM_EXIT,SYM_RETURN,SYM_FOR
};

//符号代号集，与符号一一对应
int ssym[NSYM + 1] =
{
	SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
	SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON,SYM_BITAND,SYM_BITOR,SYM_ANTI,SYM_LSBRAC,SYM_RSBRAC,SYM_BITXOR,SYM_MOD
};

//符号集
char csym[NSYM + 1] =
{
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';','&','|','!','[',']','^','%'
};

//汇编指令集
//Dong Shi, 10.29, Add RET
//zjr , 11.2 ,Add APOP,ASTO,LODA. Delete PAS
#define MAXINS   14
char* mnemonic[MAXINS] =
{
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC","JPC_and","JPC_or", "RET","APOP","ASTO","LODA"
};

typedef struct
{
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
} comtab;

//comtab table[TXMAX];
comtab *table; //将table变成指针 //modified by zjr 17.10.27

typedef struct
{
	char  name[MAXIDLEN + 1];
	int   kind;
	short level;
	short address;
} mask;

FILE* infile;

//to store symbol table list  //added by zjr 17.10.27
typedef struct stnode
{
	comtab *stable;
	struct stnode *next;
	int level;
	int funcparam;
	int localtx;
	char funcname[100];
}stnode;

stnode stlist;
stnode *Func;
int position(char *id);	//declare position //added by zjr 17.10.27
int tmpaddress=0;	//record address   //added by zjr 17.10.27
char funcname[200];	//record funcname  //added by zjr 17.10.27
char tmpparam[50][50];  //record name of parameters temporarily //added by zjr 17.10.27
int tmptx=0;		//added by zjr 17.10.28

int cx6[10],cx7[10];
int sign_and=0;int sign_or=0;int sign_condition=0;//----add by ywt 2017.10.25
// EOF PL0.h
