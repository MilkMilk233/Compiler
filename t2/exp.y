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
program : exp {printf("program : exp \n"); ;printf("Result: %d\n", $1); }
        | program ';' exp {printf("program : program ';' exp \n"); printf("Result: %d\n", $3); }
        ;

exp : exp '+' exp {printf("exp : exp '+' exp  \n"); $$ = $1 + $3; }
    | exp '-' exp {printf("exp : exp '-' exp \n"); $$ = $1 - $3; }
    | exp '*' exp {printf("exp : exp '*' exp \n"); $$ = $1 * $3; }
    | exp '^' exp {printf("exp : exp '^' exp \n"); $$ = pow($1, $3); }
    | exp '/' exp {printf("exp : exp '/' exp \n"); $$ = $1 / $3; }
    | '-' exp %prec OP_UMINUS {printf("exp : '-' exp prec OP_UMINUS \n"); $$ = -$2; }
    | NUMBER {printf("exp : NUMBER \n"); $$ = $1; }
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
