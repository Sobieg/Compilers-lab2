%{
	#include <iostream>
	#include <fstream>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <cstring>
	#include "2_ex.hpp"

	static void print_token_value(FILE* file, int type, YYSTYPE value);
	//#define YYPRINT(file, type, value) print_token_value(file, type, value)
	//#define YYPRINT(file, type, value) fprintf(file, "%d", value);
	extern int yy_flex_debug;

	typedef enum {prefix, postfix} sequenceEnum;

	nodeType *opr(int oper, int nops, ...); //оператор
	nodeType *uopr(int oper, int nops, ...);
	nodeType *id(char* id); //id
	nodeType *mid(char* id, int op, sequenceEnum seq);
	nodeType *con(int value); //константа (число)
	void freeNode(nodeType* p);
	int ex(nodeType *p); // выполнение ноды?

	idVar sym[SYMSIZE]; //таблица символов //TODO: сделать очистку
	int firstFree = 0; // первый свободный индекс в массиве sym

	extern int yylineno;
	extern int yylex();
	extern register_struct cpu[NUM_OF_REGISTERS];
	//extern register_list* lu;

	int lbl;

	void yyerror(char *s) {
		std::cerr << s << ", line " << yylineno << std::endl;
		exit(1);
	}

%}
/* %language "c++" */



%union {
	int iValue;			/*целочисленное значение*/
	idVar id;		    /*идентификатор*/ 
	nodeType* nPtr;		/*указатель на ноду*/
}


%start program

%token IF ELSE WHILE
%token INC DEC PLUS_ASS MIN_ASS MULT_ASS DIV_ASS MOD_ASS
%token MORE_EQ LESS_EQ EQ NOTEQ
%token <iValue> NUM
%token <id> VAR
%token RETURN PRINT



%type <nPtr> varaible stmt stmt_list stmt1 stmt2 expr expr1 expr2 term var /*надо перечислить все, где будет opr, id или con*/
 
%%

program: 	function			                    {exit(0);}
			;

function: 
			function stmt 							{ex($2); freeNode($2);}
			|
			;

stmt_list:	stmt 									{$$ = $1;}
			| stmt_list stmt 						{$$ = opr(';', 2, $1, $2);}
			;
				

stmt1: 		'{' stmt_list '}' 						{$$ = $2;}
			| expr ';'								{$$ = $1;}
			| IF '(' expr ')' stmt1 ELSE stmt1		{$$ = opr(IF, 3, $3, $5, $7);}
			| IF '(' expr ')' stmt1 ELSE ';'		{$$ = opr(IF, 2, $3, $5);}
			| IF '(' expr ')' stmt1 ';'				{$$ = opr(IF, 2, $3, $5);}
			| WHILE '(' expr ')' stmt1				{$$ = opr(WHILE, 2, $3, $5);}
			| WHILE '(' expr ')' ';'				{$$ = opr(WHILE, 1, $3);}
			| PRINT '(' expr ')' ';'				{$$ = opr(PRINT, 1, $3);}
			| RETURN ';'							{$$ = opr(RETURN, 0);}
			| RETURN expr ';'						{$$ = opr(RETURN, 1, $2);}
			;

stmt2:		IF '(' expr ')' stmt1  					{$$ = opr(IF, 2, $3, $5);}
			| IF '(' expr ')' ';'					{$$ = opr(IF, 1, $3);}
			| IF '(' expr ')' ELSE ';'				{$$ = opr(IF, 1, $3);}
			| IF '(' expr ')' ELSE stmt 			{$$ = opr(IF, 4, $3, $6);} /*тут вторым аргументом 4 для того чтобы отличать от другого случая с 2 аргументами. Я не придумал, как сделать без такого вот...*/
			| IF '(' expr ')' stmt1 ELSE stmt2		{$$ = opr(IF, 3, $3, $5, $7);}
			| WHILE '(' expr ')' stmt2				{$$ = opr(WHILE, 2, $3, $5);}
			;

stmt: 		stmt1 									{$$ = $1;}
			| stmt2									{$$ = $1;}
			;

varaible:	VAR 									{
													$$ = id($1.id.name);
													}
			| VAR INC                  				{$$ = mid($1.id.name, INC, postfix);}
			| VAR DEC								{$$ = mid($1.id.name, DEC, postfix);}
			| INC VAR                               {$$ = mid($2.id.name, INC, prefix);}
			| DEC VAR 								{$$ = mid($2.id.name, DEC, prefix);}
			;	

expr:		
			expr1 									{$$ = $1;/*ex($1); freeNode($1);*/}
			| varaible '=' expr2					{$$ = opr('=', 2, $1, $3);}
			| varaible PLUS_ASS expr2				{$$ = opr(PLUS_ASS, 2, $1, $3);}
			| varaible MIN_ASS expr2				{$$ = opr(MIN_ASS, 2, $1, $3);}
			| varaible MULT_ASS expr2				{$$ = opr(MULT_ASS, 2, $1, $3);}
			| varaible DIV_ASS expr2				{$$ = opr(DIV_ASS, 2, $1, $3);}
			| varaible MOD_ASS expr2				{$$ = opr(MOD_ASS, 2, $1, $3);}
			;

expr1: 		expr2									{$$ = $1;/*ex($1); freeNode($1);*/}
			| expr1 EQ expr2						{$$ = opr(EQ, 2, $1, $3);}
			| expr1 LESS_EQ expr2					{$$ = opr(LESS_EQ, 2, $1, $3);}
			| expr1 MORE_EQ expr2					{$$ = opr(MORE_EQ, 2, $1, $3);}
			| expr1 NOTEQ expr2						{$$ = opr(NOTEQ, 2, $1, $3);}
			| expr1 '<' expr2						{$$ = opr('<', 2, $1, $3);}
			| expr1 '>' expr2						{$$ = opr('>', 2, $1, $3);}
			;

expr2:		term									{$$ = $1;}
			| expr2 '+' term						{$$ = opr('+', 2, $1, $3);}
			| expr2 '-' term						{$$ = opr('-', 2, $1, $3);}
			;

term:		var 									{$$ = $1;}
			| term '*' var 							{$$ = opr('*', 2, $1, $3);}
			| term '/' var 							{$$ = opr('/', 2, $1, $3);}
			| term '%' var 							{$$ = opr('%', 2, $1, $3);}
			;

var:		NUM										{$$ = con($1);}
			| '-' var 								{$$ = opr('-', 1, $2);} 
			| '!' var 								{$$ = opr('!', 1, $2);}
			| '(' expr ')'							{$$ = $2;}
			| varaible 								{
														$$ = $1;
														/*ex($1); freeNode($1);*/
													}
			;


%%

int bsearch(int l, int r, char* idName) {
	if (r >= 1) {
		int midle = 1 + (r-1)/2;
		int res = strcmp(sym[midle].id.name, idName);

		if (res == 0) {
			return midle;
		}
		else if (res > 0){
			return bsearch(l, midle-1, idName);
		}
		
		return bsearch(midle+1, r, idName);
	}
	return -1;
}

void freeNode(nodeType* p) {
	int i;
	if (!p) return;
	if (p->type == typeOpr) {
		for (i = 0; i < p->opr.nops; i++) {
			freeNode(p->opr.op[i]);
		}
	}
	free(p);
}

nodeType *opr(int oper, int nops, ...) {
	//std::cout << "oper: " << oper << " nops: " << nops << "<<<<<<<" <<std::endl;
	va_list ap;
	nodeType *p;
	int i;

	if ((p = (nodeType*) malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType*))) == NULL) {
		yyerror("out of memory");
	}
	p->type = typeOpr;
	p->opr.oper = oper;
	p->opr.nops = nops;
	va_start(ap, nops);
	for (i = 0; i < nops; i++){
		p->opr.op[i] = va_arg(ap, nodeType*);
	}
	va_end(ap);
	return p;
}

nodeType *id(char* id) {

	//std::cout << id << "<<<<<<<" <<std::endl;

	nodeType *p;

	if((p = (nodeType*) malloc(sizeof(nodeType))) == NULL){
		yyerror("out of memory");
	}

	p->type = typeId;
	p->id.id.name = id;
	p->id.ch = no;

	//std::cout << p->id.id.name << "<<<<<<<" <<std::endl;
	return p;
}
nodeType *mid(char* id, int op, sequenceEnum seq) {

	//std::cout << id << "<<<<<<<" <<std::endl;

	nodeType *p;

	if((p = (nodeType*) malloc(sizeof(nodeType))) == NULL){
		yyerror("out of memory");
	}

	p->type = typeId;
	p->id.id.name = id;
	if (seq == prefix) {
		if (op == INC) {
			p->id.ch = preinc;
		}
		else {
			p->id.ch = predec;
		}
	}
	else {
		if (op == INC) {
			p->id.ch = postinc;
		}
		else {
			p->id.ch = postdec;
		}
	}

	//std::cout << p->id.id.name << "<<<<<<<" <<std::endl;
	return p;
}
nodeType *con(int value){
	nodeType *p;
	if ((p = (nodeType*) malloc(sizeof(nodeType))) == NULL) {
		yyerror("out of memory");
	}
	p->type = typeCon;
	p->con.value = value;

	return p;
}

static void print_token_value(FILE* file, int type, YYSTYPE value) {
	switch(type){
		case VAR: fprintf(file, "Name: %s", value); break; //значения нет, хз как прописывать пока что
		case NUM: fprintf(file, "Value: %d", value); break;
		//case varaible: fprintf(file, "Name: %s", value.nPtr->id.id.name); break;
		default: fprintf(file, "%c", value);
	}
}

void init_regs() {
	memset(&cpu, 0, NUM_OF_REGISTERS * sizeof(register_struct));
	for (int i = 0; i<NUM_OF_REGISTERS; i++) {
		cpu[i].is_free = true;
	}
	cpu[0].name = "eax";
	cpu[1].name = "ebx";
	cpu[2].name = "ecx";
	cpu[3].name = "edx";
	for (int i = 4; i<NUM_OF_REGISTERS; i++) {
		std::string name = "r";
		name += std::to_string(i-4);
		cpu[i].name = name.c_str();
	}
	//lu->lastUsed = nullptr;
	//lu->preLastUsed = nullptr;
}

int main() {
	lbl = 0;
	init_regs();
	system("rm -f 2_asm.txt");
	yydebug = 0;
	yy_flex_debug = 0;
	return yyparse();
}



