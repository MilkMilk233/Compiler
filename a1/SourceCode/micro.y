%{
#include <stdio.h>
#include <math.h>
#include <string.h>

void yyerror(const char *);
int yylex();
extern FILE *yyin;
extern char *yytext;

// Official name for MIPS code generation
char sr_name[8][5] = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7"};
char tr_name[8][5] = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7"};

// 占用情况
int sr_regs_status = 0;
int tr_regs_status[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// 和SR相匹配的Token name，用于搜索
char sr_id_name[100][32];

// TODO: Extra space in memory

%}

%code requires {

  struct id_list_struct{
    int capacity;
    char id_l_struct[50][32];
  };
  
  struct exp_struct{
    char first_reg_name[10];
    char sec_reg_name[10];
    int first_int_value;
    int sign;   // 0 for add, 1 for sub
    int reg_capacity;
    int int_capacity;
  };

  struct exp_list_struct{
    int capacity;
    struct exp_struct exp_l_struct[50];
  };

  struct primary_struct{
    int is_int;
    int int_value;
    char reg_name[10];
  };
  
  void clean_tr();
  int find_id(char* name);
  void id_to_pri(char* id, struct primary_struct *primary_str);
  void int_to_pri(int value, struct primary_struct *primary_str);
  void assign(struct exp_struct *e_str, char* dst);
  void read_id_list(struct id_list_struct *id_l_struct);
  void write_exp_list(struct exp_list_struct *exp_l_struct);
  void first_add_to_id_list(struct id_list_struct *id_l_struct, char* name);
  void first_add_to_exp_list(struct exp_list_struct *exp_l_struct, struct exp_struct *exp_str);
  void add_to_id_list(struct id_list_struct *id_l_struct, char* name);
  void add_to_exp_list(struct exp_list_struct *exp_l_struct, int value);
  void pri_to_exp(struct primary_struct *primary_str, exp_struct *exp_str);
  void neg_pri_to_exp(struct primary_struct *primary_str, exp_struct *exp_str);
  void exp_to_pri(struct exp_struct *exp_str, struct primary_struct *primary_str);
  void exp_add_pri(struct exp_struct *target, struct exp_struct *source1, struct primary_struct *source2, int applied_sign);

}

%union {
  char id[32];
  int value;
  struct exp_list_struct exp_list_str;
  struct id_list_struct id_list_str;
  struct exp_struct exp_str;
  struct primary_struct primary_str;
}

%token LEX_ERR

%token BEGIN_ END READ WRITE 
%token <value> INTLITERAL
%token <id> ID
%token LPAREN RPAREN COMMA 
%token ASSIGNOP PLUOP MINUSOP SCEANOF 

%type <primary_str> primary
%type <exp_str> exp 
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

statement : ID ASSIGNOP exp ';' {assign(&($3), &($1));}
          | READ LPAREN id_list RPAREN ';' {read_id_list(&($3));}
          | WRITE LPAREN exp_list RPAREN ';' {write_exp_list(&($3));}
          ;

id_list : ID {first_add_to_id_list(&($$), $1);}
        | id_list COMMA ID {add_to_id_list(&($1), $3);}
        ;

exp_list : exp {first_add_to_exp_list(&($$), $1); }
         | exp_list COMMA exp {add_to_exp_list(&($1), $3); }
         ;

exp : primary {pri_to_exp(&($1), &($$)); }
    | exp PLUOP primary {exp_add_pri(&($$), &($1), &($3), 0); }
    | exp MINUSOP primary {exp_add_pri(&($$), &($1), &($3), 1); }
    | MINUSOP primary {neg_pri_to_exp(&($2), &($$)); }
    ;

primary : LPAREN exp RPAREN {exp_to_pri(&($2), &($$));}     // If there are two elements in the exp, compress it into one(with the use of TR), put it into primary_struct
        | ID {id_to_pri(&($1), &($$));}
        | INTLITERAL {int_to_pri($1, &($$)); }
        ;
%%


// Assist functions
void clean_tr(){
  tr_regs_status = {0,0,0,0,0,0,0,0};
}

int find_id(char* name){
  // Check if the name has preserved in some SR. If yes, return the id of the SR.
  int i;
  for(i = 0; i < 100; i++){
    if(strcmp(name, sr_id_name[i]) == 0){
      return i;
    }
  }

  // If not, bind it to an new SR and return the name of the SR.
  sr_id_name[sr_regs_status] = name;
  sr_regs_status++;
  return sr_regs_status - 1;
}

void id_to_pri(char* id, struct primary_struct *primary_str){
  primary_str->is_int = 0;
  primary_str->reg_name = id;
}

void int_to_pri(int value, struct primary_struct *primary_str){
  primary_str->is_int = 1;
  primary_str->int_value = value;
}

// Member functions
void assign(struct exp_struct *e_str, char* dst){
  char[15] s_name, target, c1, c2;

  // Find the SR id according to the token name.
  int SR_id = find_id(dst);
  if(SR_id < 8){
    target = sr_name[SR_id];
  }
  // Todo: Memory expansion 
  
  if(e_str->reg_capacity == 2){
    if(e_str->sign){
      s_name = "sub";
    }
    else{
      s_name = "add";
    }
    c1 = e_str->first_reg_name;
    c2 = e_str->sec_reg_name;
  }
  else if(e_str->reg_capacity == 1 && e_str->int_capacity == 1){
    if(e_str->sign){
      s_name = "subi";
    }
    else{
      s_name = "addi";
    }
    c1 = e_str->first_reg_name;
    atoi(e_str->first_int_value, c2, 10);
  }
  else if(e_str->reg_capacity == 1){
    s_name = "add";
    c1 = e_str->first_reg_name;
    c2 = "$zero";
  }
  else if(e_str->reg_capacity == 1){
    s_name = "addi";
    atoi(e_str->first_int_value, c1, 10);
    c2 = "$zero";
  }

  // Print out the info.
  printf("%s, %s, %s, %s\n", s_name, target, c1, c2);
}

void read_id_list(struct id_list_struct *id_l_struct){
  int i;
  for(i = 0; i < id_l_struct->capacity; i++){
    if(i < 8){
      printf("addi $v0, $zero, 5\n");
      printf("syscall\n");
      assign(&(id_l_struct->id_l_struct[i]), "$s0");
    }
    // TODO: Memory expansion
  }
}

void write_exp_list(struct exp_list_struct *exp_l_struct){
  int i;
  for(i = 0; i < exp_l_struct->capacity; i++){
    if(i < 8){
      printf("addi $v0, $zero, 1\n");
      assign(&(id_l_struct->id_l_struct[i]), "$s0");
      printf("syscall\n");
    }
    // TODO: Memory expansion
  }

}
void first_add_to_id_list(struct id_list_struct *id_l_struct, char* name){
  id_l_struct->capacity = 1;
  id_l_struct->id_l_struct[0] = name;
}
void first_add_to_exp_list(struct exp_list_struct *exp_l_struct, struct exp_struct *exp_str){
  exp_l_struct->capacity = 1;
  exp_l_struct->exp_l_struct[0] = exp_str;
  
}
void add_to_id_list(struct id_list_struct *id_l_struct, char* name){
  id_l_struct->id_l_struct[id_l_struct->capacity] = name;
  id_l_struct->capacity++;
}
void add_to_exp_list(struct exp_list_struct *exp_l_struct, int value){
  exp_l_struct->exp_l_struct[exp_l_struct->capacity] = exp_str;
  exp_l_struct->capacity++;
}

void pri_to_exp(struct primary_struct *primary_str, exp_struct *exp_str){
  // Step 1, bind to an unoccupied TR
  int i;
  for(i = 0; i < 8; i++){
    if(tr_regs_status[i] == 0) break;
  }
  // Step 2, initialize the exp struct
  exp_str->first_reg_name = tr_name[i];
  exp_str->sign = 0;
  exp_str->reg_capacity = 1;
  exp_str->int_capacity = 0;
}

void neg_pri_to_exp(struct primary_struct *primary_str, exp_struct *exp_str){
  struct exp_struct new_exp_str;
  new_exp_str->reg_capacity = 2;
  new_exp_str->int_capacity = 0;
  new_exp_str->sign = 1;
  new_exp_str->first_reg_name = "$zero";
  new_exp_str->sec_reg_name = primary_str->reg_name;
  int i;
  for(i = 0; i < 8; i++){
    if(tr_regs_status[i] == 0) break;
  }
  tr_regs_status[i] = 1;
  assign(&new_exp_str, tr_name[i]);
  exp_str->first_reg_name = tr_name[i];
  exp_str->int_capacity = 0;
  exp_str->reg_capacity = 1;
}

void exp_to_pri(struct exp_struct *exp_str, struct primary_struct *primary_str){
  // Only valid when exp_str has two register loads.
  // assert(exp_str->reg_capacity == 2);

  // Step 1: Find possible TR
  int i, is_first_TR, is_second_TR;
  is_first_TR = is_second_TR = 0;
  for(i = 0; i < 8; i++){
    if(strcmp(tr_name[i], exp_str->first_reg_name) == 0){
      is_first_TR = i+1;
    }
    if(strcmp(tr_name[i], exp_str->first_reg_name) == 0){
      is_second_TR = i+1;
    }
  }
  // Step 2: If == 0, allocate new one; if == 2, eliminate one.
  if(is_first_TR && is_second_TR){
    // Eliminate one
    tr_regs_status[is_second_TR-1] = 0;
    primary_str->reg_name = tr_name[is_first_TR-1];
  }
  else if(!is_first_TR && is_second_TR){
    // allocate new one
    for(i = 0; i < 8; i++){
      if(tr_regs_status[i] == 0){
        tr_regs_status[i] = 1;
        break;
      }
    }
    primary_str->reg_name = tr_name[i];
  }
  else{
    // Migrate name in exp to primary. 
    if(is_first_TR){
      primary_str->reg_name = tr_name[is_first_TR-1];
    }
    else if(is_second_TR){
      primary_str->reg_name = tr_name[is_second_TR-1];
    }
  }
  // Step 3: Print out the MIPS code.
  assign(&exp_str, &(primary_str->reg_name));
  primary_str->is_int = 0;
}

void exp_add_pri(struct exp_struct *target, struct exp_struct *source1, struct primary_struct *source2, int applied_sign){
  if(source1->reg_capacity == 2){
    struct primary_struct new_prime;
    exp_to_pri(&(source1), &(new_prime));
    target->first_reg_name = new_prime->reg_name;
    target->sign = applied_sign;
    if(source2->is_int){
      target->first_int_value = source2->int_value;
      target->reg_capacity = 1;
      target->int_capacity = 1;
    }
    else{
      target->sec_reg_name = source2->reg_name;
      target->reg_capacity = 2;
      target->int_capacity = 0;
    }
  }
  else if(source1->reg_capacity == 1 && source1->int_capacity == 1){
    if(source2->is_int){
      target->first_int_value += source2->int_value;
    }
    else{
      // Adding two of the regs. 
      int temp = source1->first_int_value;
      int temp_sign = source1->sign;
      source1->sec_reg_name = source2->reg_name;
      source1->sign = applied_sign;
      source1->reg_capacity = 2;
      source2->int_capacity = 0;
      struct primary_struct new_prime;
      exp_to_pri(&(source1), &(new_prime));
      target->first_reg_name = new_prime->reg_name;
      target->sign = temp_sign;
      target->first_int_value = temp;
      target->int_capacity = 1;
      target->reg_capacity = 1;
    }
  }
  else if(source1->reg_capacity == 1){
    target->first_reg_name = source1->first_reg_name;
    if(source2->is_int){
      target->first_int_value = source2->int_value * pow(-1, applied_sign);
      target->int_capacity = 1;
      target->reg_capacity = 1;
    }
    else{
      target->sec_reg_name = source2->reg_name;
      target->int_capacity = 0;
      target->reg_capacity = 2;
    }
    
  }
  else if(source1->int_capacity == 1){
    if(source2->is_int){
      target->first_int_value += source2->int_value * pow(-1, applied_sign);
      target->int_capacity = 1;
      target->reg_capacity = 0;
    }
    else{
      target->first_reg_name = source2->reg_name;
      target->int_capacity = 1;
      target->reg_capacity = 1;
    }
  }
}

// Bison (YYEX) functions
void yyerror(const char *msg) {
  if (yychar == LEX_ERR)
    fprintf(stderr, "Lexical error: unkown token '%s'\n", yytext);
  else fprintf(stderr, "%s\n", msg);
}

// Main function
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
