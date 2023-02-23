%{
#include <ctype.h>
%}

%option noyywrap

%%

[^\] { printf("%d\n",*yytext); }

%%

int main(int argc, char* argv[]) {
  return yylex();
}
