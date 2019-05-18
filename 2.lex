%{
	#include <iostream>
	#include <stdlib.h>
	#include "2_structs.hpp"
	#include "y.tab.h" //c
	#include <string.h>
	//#include "2_structs.hpp" //c++
	#define YYSTYPE std::string
	void yyerror(char *s);
	void myerr(char *s, char c) {
		std::cerr << s << ", character: " << c << ", line: " << yylineno << std::endl;
		exit(1);
	}
%}

%option yylineno
%option noyywrap

%%

"if" 						{return IF;}
"else" 						{return ELSE;}
"while" 					{return WHILE;}
"++" 						{return INC;}
"--"						{return DEC;}
"+="						{return PLUS_ASS;}
"-="						{return MIN_ASS;}
"*="						{return MULT_ASS;}
"/="						{return DIV_ASS;}
"%="						{return MOD_ASS;}
">="						{return MORE_EQ;}
"<=" 						{return LESS_EQ;}
"!=" 						{return NOTEQ;}
"=="						{return EQ;}
"return"					{return RETURN;}
"print"						{return PRINT;}
[ \t\r\n] 					{;}
[0-9]+						{	
								yylval.iValue = atoi(yytext);
								return NUM;
							}

[a-zA-Z_]+[a-zA-Z0-9_]*		{	
								yylval.id.id.name = strdup(yytext);
								return VAR;
							}

[-{};()=<>+*/!%]			{return *yytext;}
.							{}


%%