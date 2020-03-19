%{
#include "compilador.tab.h"
#include<string.h>
#define MAXTOKENLEN 100
extern YYSTYPE yylval;
void abrirArq();
char currentToken[MAXTOKENLEN+1];

%}

%option yylineno

spaco " "
DIGIT [0-9]
CHAR [a-zA-Z]
COMMO \/\*
COMMC \*\/

%%
[\t\n\r]+ {}
[0-9]+	{strncpy(currentToken,yytext,MAXTOKENLEN); return NUM; }
"int" { return INT; }
"void" { return VOID; }
"if" { return IF; }
"else" { return ELSE; }
"while" { return WHILE; }
"return" { return RETURN; }
{CHAR}+ {strncpy(currentToken,yytext,MAXTOKENLEN); return ID;}
[0-9]+{CHAR}+ return ERR;
{CHAR}+[0-9]+ return ERR;
{CHAR}+[0-9]+{CHAR}+ return ERR;
\+	{ return ADD; }
\-	{ return SUB; }
\*	{ return MUL; }
"/" { return DIV; }
"<" { return LT; }
"<=" { return LTE; }
">" { return GT; }
">=" { return GTE; }
"==" { return EQ; }
"!=" { return NEQ; }
\(	{ return PRTO; }
\)	{ return PRTC; }
"=" { return ATR; }
";" { return SCL; }
"," { return COM; }
"[" { return SBTO; }
"]" { return SBTC; }
"{" { return KEYO; }
"}" { return KEYC; }
{spaco} {}
{COMMO}(.)*{COMMC} {}
.       {return ERR;}
%%

void abrirArq()
{
  yyin = fopen("entrada.txt", "r");
}
