%{
#include "micro.parser.h"
#include <stdio.h>
int end = 0;
%}

DIGIT [0-9]
LETTER [a-zA-Z]

%option noyywrap
%x COMMENT

%%
<INITIAL>[ \t\r\n]+   {;}
<INITIAL>"begin" {/*printf("begin\n");*/ return BEGIN_; }
<INITIAL>"end" {/*printf("end\n"); */ return END; }
<INITIAL>"read" {return READ; }
<INITIAL>"write" {/*printf("write\n");*/ return WRITE; }
<INITIAL>{DIGIT}+ { yylval.value = atoi(yytext); return INTLITERAL; }
<INITIAL>{LETTER}({DIGIT}|{LETTER})* {sscanf(yytext, "%s", yylval.id); return ID; }
<INITIAL>"(" {return LPAREN; }
<INITIAL>")" {return RPAREN; }
<INITIAL>";" {return ';'; }
<INITIAL>"," {return COMMA; }
<INITIAL>":=" {return ASSIGNOP; }
<INITIAL>"+" {return PLUOP; }
<INITIAL>"--" {/*printf("Get into comment mode!\n");*/ BEGIN(COMMENT); }
<INITIAL>"-" {return MINUSOP; }
<<EOF>> {if(!end) {end = 1; return SCEANOF;} yyterminate(); }
<COMMENT>[^\n]+ {;}
<COMMENT>\n {/*printf("Quitting comment mode!\n");*/ BEGIN(INITIAL); }
%%