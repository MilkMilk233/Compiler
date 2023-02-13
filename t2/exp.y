%{
#include <stdio.h>
#include <math.h>

void yyerror(const char *);
int yylex();
extern FILE *yyin;
extern char *yytext;
%}
%union {
  int value;
}


%token LEX_ERR

%type <value> exp


%token <value> NUMBER
%left '+' '-'
%left '*' '/'
%left '^'
%right OP_UMINUS


%start program
%%
program : exp { printf("Result: %d\n", $1); }
        | program ';' exp { printf("Result: %d\n", $3); }
        ;

exp : exp '+' exp { $$ = $1 + $3; }
    | exp '-' exp { $$ = $1 - $3; }
    | exp '*' exp { $$ = $1 * $3; }
    | exp '^' exp { $$ = pow($1, $3); }
    | exp '/' exp { $$ = $1 / $3; }
    | '-' exp %prec OP_UMINUS { $$ = -$2; }
    | NUMBER { $$ = $1; }
    ;
%%
void yyerror(const char *msg) {
  if (yychar == LEX_ERR)
    fprintf(stderr, "Lexical error: unkown token '%s'\n", yytext);
  else fprintf(stderr, "%s\n", msg);
}
int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Missing input file!\n");
  }
  yyin = fopen(argv[1], "r");
  if (yyparse()) {
    fprintf(stderr, "Unable to parse the input!\n");
    return 1;
  }

  fclose(yyin);
  return 0;
}
