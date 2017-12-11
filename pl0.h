#include <stdio.h>

#define NRW        20     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       22     // maximum number of symbols in array ssym and csym //ZJR 12.8 #Z1
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     67     // maximum number of symbols    //ZJR 12.8 #Z1  

#define STACKSIZE  4096   // maximum storage

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
	SYM_LBRACE,
	SYM_RBRACE,
	//=================added by lijiquan
	SYM_AND,
	SYM_OR,
	SYM_LOGIEQU,

	SYM_BITXOR, 	// ^
	SYM_MOD,			// %

	//Dong Shi, 11.23, add SYM_INC, SYM_DEC
	SYM_INC,
	SYM_DEC,

	//Dong Shi, 12.1, add SYM_FORMAT and SYM_PRINTF
	//Dong Shi, 12.1, add SYM_RANDOM
	SYM_FORMAT,
	SYM_PRINTF,
	SYM_RANDOM,

	//Dong Shi, 12.3, add SYM_INPUT
	SYM_INPUT,

	//ZJR 12.8 ADD '?' AND ':',WHICH IS SYM_QUES AND SYM_COLON #Z1
	SYM_QUES,
	SYM_COLON,

	//ZJR 12.8 ADD '<<' AND '>>' //#Z5
	SYM_LSHIFT,
	SYM_RSHIFT,

	//ZJR 12.8 ADD SEVERAL ASSIGNMENT OPERATORS //#Z11
	SYM_MULAS,
	SYM_DIVAS,
	SYM_MODAS,
	SYM_ADDAS,
	SYM_SUBAS,
	SYM_LAS,
	SYM_RAS,
	SYM_ANDAS,
	SYM_XORAS,
	SYM_ORAS, 

	//LJQ 12.9 ADD CALST for callstack
	SYM_CALST
};

//Add ID_POINTER //zjr 17.11.2 
//Add array //ljq
//Add ID_PARRAY for array arguments passing //zjr //11.7 //#Z4
enum idtype
{
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE, ID_POINTER, ID_ARRAY,ID_PARRAY
};

//add PAS: for parameter pass //modified by zjr 17.10.27
//Dong Shi, 10.28, Add RET op
//add APOP: get stack[top] to stack[/pbase+1] and recover top to base+1 //zjr 17.11.2
//add ASTO:store top to /pbase //zjr 11.2
//add LODA: load argument from stack //zjr 11.2
//delete PAS	//zjr 11.2
//Dong Shi, 12.1, Add OUTS op
//Dong Shi, 12.3, Add IN op
//ZJR add LODST POP, and EXC 12.9 #Z15
enum opcode
{
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC,JLEZ,JGZ,RET,APOP,ASTO,LODA,LEA, LODAR, STOAR, OUTS, IN,
	LODST,POP,EXC, CALST
};

//Dong Shi, 11.22, Add op OPR_INC and OPR_DEC
enum oprcode
{
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_LOGIEQU,
	OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,
	OPR_GEQ, 
	//================added by lijiquan
	OPR_AND, OPR_OR, OPR_ANTI,
	
	OPR_BITAND,OPR_BITOR,OPR_BITXOR,OPR_MOD,

	OPR_INC, OPR_DEC,

	//ADDED BY ZJR 12.8 #Z5
	OPR_LSH,OPR_RSH
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
/* 26 */    "Right ++ or -- must followed by a variable.", //Dong Shi, 11.22, Add Error "++"/"--" + VAR 
/* 27 */    "Reserved word call is not supported anymore. ",
/* 28 */    "Missing the dimension width of the array",
/* 29 */    "Missing ']'",
/* 30 */    "Missing '[' or dimension",
/* 31 */    "incompatible type!",	//added by zjr //11.7 //#Z9
/* 32 */    "There are too many levels.",
/* 33 */    "Procedure not found!",	//added by zjr //11.7 //#Z9
/* 34 */    "Has been declared!"	//zjr //11.21
//Dong Shi, 12.1, Add some error about printf
/* 35 */	"( expected.",
/* 36 */	"String format expected.",
/* 37 */	", expected.",
//Dong Shi, 12.3, Add error about input
/* 38 */	"Input destination should be a variable.",
//Dong Shi, 12.3, Add more error about ++/--
/* 39 */	"Opreation increase or decrease must be operated on a variable.",
//zjr 12.8 incorrect ?: expression //#Z3
/* 40 */    ": expected in ?: expression."
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
int dx;		//modified by zjr //11.7

char line[512];	//zjr 11.27

//Dong Shi, 12.1, Add IOStack
char IOStack[32][128];
char tmpStack[128];
int IOStackNum = 0;

//Dong Shi, 12.3, Add tmp var record
int activeLevel;
int activeAddress;

//Dong Shi, 12.3, Add list assign record
int AssignStackLeft[64];
int AssignStackTop;

instruction code[CXMAX];
int funcparam=0;

//关键字集
//Dong Shi, 12.1, Add printf
//Dong Shi, 12.1, Add random
//Dong Shi, 12.3, Add input
char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while","else","else if","exit","return","for", "printf", "random", "input", 
	"callstack"
};

//关键字代号集，与关键字一一对应
//Dong Shi, 12.1, Add SYM_PRINTF
//Dong Shi, 12.1, Add SYM_RANDOM
//Dong Shi, 12.3, Add SYM_INPUT
int wsym[NRW + 1] =
{
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE,
	SYM_ELSE,SYM_ELSE_IF,SYM_EXIT,SYM_RETURN,SYM_FOR, SYM_PRINTF,
	SYM_RANDOM, SYM_INPUT, SYM_CALST
};

//ADD SYM_QUES AND SYM_COLON //ZJR 12.8 //#Z1
//符号代号集，与符号一一对应
int ssym[NSYM + 1] =
{
	SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
	SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON,SYM_BITAND,SYM_BITOR,SYM_ANTI,SYM_LSBRAC,SYM_RSBRAC,
	SYM_BITXOR,SYM_MOD,SYM_LBRACE,SYM_RBRACE,
	SYM_QUES,SYM_COLON
};

//ADD '?' AND ':' //ZJR 12.8 #Z1
//符号集
char csym[NSYM + 1] =
{
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';','&','|','!','[',']','^','%','{','}','?',':'
};

//汇编指令集
//Dong Shi, 10.29, Add RET
//zjr , 11.2 ,Add APOP,ASTO,LODA. Delete PAS
//Dong Shi, 12.1, Add OUTS
//Dong Shi, 12.3, Add IN
//ZJR 12.9 ADD LODST AND POP AND EXC #Z15
#define MAXINS   23
char* mnemonic[MAXINS] =
{
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC","JLEZ","JGZ", "RET","APOP","ASTO","LODA", "LEA", "LODAR", "STOAR", "OUTS", "IN",
	"LODST","POP","EXC", "CALST"
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
//add paralist 	//zjr 11.7 //#Z9
typedef struct stnode
{
	comtab *stable;
	struct stnode *next;
	int level;
	int funcparam;
	int localtx;
	char funcname[100];
	int paralist[50];
	//ljq added 12.10 for callstack
	int address;
	char paraname[10][100];
	int paranum;
}stnode;

stnode stlist;
stnode *Func=&stlist;
int position(char *id);	//declare position //added by zjr 17.10.27
int tmpaddress=0;	//record address   //added by zjr 17.10.27
char funcname[200];	//record funcname  //added by zjr 17.10.27
//char tmpparam[50][50];  //record name of parameters temporarily //added by zjr 17.10.27
comtab tmpparam[50];	//record name of parameters temporarily //modified by zjr 11.17
int tmptx=0;		//added by zjr 17.10.28

/*added by ljq 17.12.10 for callstack
char paranameset[1000][100];
int currparanum = 0;
//===========for callstack*/
char tmpparaname[10][100];
int tmpparanum;
char numstring[10];

int cx6[5][10],cx7[5][10];
int sign_and[5]={0,0,0,0,0};
int sign_or[5]={0,0,0,0,0};
int lev=0;
int sign_condition=0;//----add by ywt 2017.10.25

//ljq
typedef struct 
{
	char name[MAXIDLEN + 1];
	int kind;
	short level;
	short address;
} array;

typedef struct 
{
	char name[MAXIDLEN + 1];
	int kind;
	int depth;
} dimensionHead;

typedef struct 
{
	char name[MAXIDLEN + 1];
	int kind;
	int width;
} dimension;

//zjr 11.7 
//#define MAX_INT ((unsigned)(-1)>>1)

int isarrayparam=0;//zjr 11.7 //#Z5
int* nodeplist(char *name);
int nodeparam(char *name);

// EOF PL0.h
