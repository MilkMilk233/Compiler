%{
#include <ctype.h>
%}

%option noyywrap
UPPER [A-Z]
F "F"
LOWER [a-z]
%%
{LOWER} {printf(" ");}
{UPPER} { printf("%c", tolower(*yytext)); }
{F} {printf("*");}
%%

int main(int argc, char* argv[]) {
  return yylex();

}
