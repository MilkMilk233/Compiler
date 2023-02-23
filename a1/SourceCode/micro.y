%{
#include <stdio.h>
#include <math.h>

void yyerror(const char *);
int yylex();
extern FILE *yyin;
extern char *yytext;

%}

%code requires {

  struct exp_list_struct{
    int capacity;
    int value[50];
  };
  struct id_list_struct{
    int capacity;
    int value[50];
  };
  void write_exp_list(struct exp_list_struct *exp_list_struct);
  void add_to_id_list(struct id_list_struct *id_l_struct, int value);
  void add_to_exp_list(struct exp_list_struct *exp_l_struct, int value);

}

%union {
  int value;
  struct exp_list_struct exp_list_str;
  struct id_list_struct id_list_str;
}

%token LEX_ERR

%token BEGIN_ END READ WRITE 
%token <value> INTLITERAL
%token <value> ID
%token LPAREN RPAREN COMMA 
%token ASSIGNOP PLUOP MINUSOP SCEANOF 

%type <value> exp primary 
%type <exp_list_str> exp_list
%type <id_list_str> id_list

%start system_goal
%%
system_goal : program SCEANOF {;}
            ;

program : BEGIN_ statement_list END {;}
        ;

statement_list : statement {;}
               | statement_list statement {;}
               ;

statement : ID ASSIGNOP exp ';' {$1 = $3; printf("%d == %d\n",$1,$3);}
          | READ LPAREN id_list RPAREN ';' {printf("READING STATEMENT\n");}
          | WRITE LPAREN exp_list RPAREN ';' {write_exp_list(&($3));}
          ;

id_list : ID {add_to_id_list(&($$), $1);}
        | id_list COMMA ID {add_to_id_list(&($1), $3);}
        ;

exp_list : exp {add_to_exp_list(&($$), $1); }
         | exp_list COMMA exp {add_to_exp_list(&($1), $3); }
         ;

exp : primary {$$ = $1; }
    | exp PLUOP primary {$$ = $1 + $3; }
    | exp MINUSOP primary {$$ = $1 - $3; }
    ;

primary : LPAREN exp RPAREN {$$ = $2; }
        | ID {printf("Reading ID = %d\n", $1); $$ = $1; }
        | INTLITERAL {$$ = $1; }
        ;
%%

void write_exp_list(struct exp_list_struct *exp_l_struct){
  printf("---PRINTING---\n");
  int i = 0;
  for(i = 0; i < exp_l_struct->capacity; i++){
    printf("%d\n",exp_l_struct->value[i]);
  }
  printf("---PRINTING DONE---\n");
}
void add_to_id_list(struct id_list_struct *id_l_struct, int value){
  id_l_struct->value[id_l_struct->capacity] = value;
  id_l_struct->capacity++;
}
void add_to_exp_list(struct exp_list_struct *exp_l_struct, int value){
  exp_l_struct->value[exp_l_struct->capacity] = value;
  exp_l_struct->capacity++;
}

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
