%{
#include <ctype.h>
%}

%option noyywrap
UPPER [A-Z]
F "F"
LOWER [a-z]
ANY [^a-z]
%%
{LOWER} {printf(" ");}
{UPPER} { printf("%c", tolower(*yytext)); }
{F} {printf("*");}
{ANY} {printf("%d", *yytext); }
%%

int main(int argc, char* argv[]) {
  return yylex();

}
