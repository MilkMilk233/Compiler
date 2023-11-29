%{
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define TR_STORAGE 10000
#define SR_STORAGE 10000
#define MEM_STORAGE 20000

void yyerror(const char *);
int yylex();
extern FILE *yyin;
extern char *yytext;

// Official name for MIPS code generation
char sr_name[8][5] = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7"};
char tr_name[8][5] = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7"};

// 占用情况
int sr_regs_status = 0;
int tr_regs_status[TR_STORAGE] = {0};

int memory_access[MEM_STORAGE];   // 0 for unoccupied, 1 for SR_occupied, 2 for TR_occupied
int memory_index[MEM_STORAGE]; // The actual index of each registers.
int tr_in_memory = 0;

// 和SR相匹配的Token name，用于搜索
char sr_id_name[SR_STORAGE][32];   // 3.2kb * 4 = 13.2kb，应该不会爆栈

%}

%code requires {

  struct exp_struct{
    char first_reg_name[10];
    char sec_reg_name[10];
    int first_int_value;
    int sign;   // 0 for add, 1 for sub
    int reg_capacity;
    int int_capacity;
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
  void assign_and_search(struct exp_struct *e_str, char* id_sr);
  void add_to_id_list(char* name);
  void add_to_exp_list(struct exp_struct *exp_str);
  void pri_to_exp(struct primary_struct *primary_str, struct exp_struct *exp_str);
  void neg_pri_to_exp(struct primary_struct *primary_str, struct exp_struct *exp_str);
  void exp_to_pri(struct exp_struct *exp_str, struct primary_struct *primary_str);
  void exp_add_pri(struct exp_struct *target, struct exp_struct *source1, struct primary_struct *source2, int applied_sign);
  void check_index(char* printed_reg_name, int* type, int* index);   // To check the index of the printed register

}

%union {
  char id[32];
  int value;
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

%start system_goal
%%
system_goal : program SCEANOF {;}
            ;

program : BEGIN_ statement_list END {;}
        ;

statement_list : statement {;}
               | statement_list statement {;}
               ;

statement : ID ASSIGNOP exp ';' {/*printf("statement : ID ASSIGNOP exp ';' \n");*/ assign_and_search(&($3), $1);}
          | READ LPAREN id_list RPAREN ';' {/*printf("statement : READ LPAREN id_list RPAREN ';' \n");*/ clean_tr();}
          | WRITE LPAREN exp_list RPAREN ';' {/*printf("statement : WRITE LPAREN exp_list RPAREN\n");*/ clean_tr();}
          ;

id_list : ID {/*printf("id_list : ID\n");*/ add_to_id_list($1); clean_tr();}
        | id_list COMMA ID {/*printf("id_list : id_list COMMA ID\n");*/ add_to_id_list($3); clean_tr();}
        ;

exp_list : exp {/*printf("exp_list : exp\n");*/ add_to_exp_list(&($1)); clean_tr();}
         | exp_list COMMA exp {/*printf("exp_list : exp_list COMMA exp\n");*/ add_to_exp_list(&($3)); clean_tr();}
         ;

exp : primary {/*printf("exp : primary \n");*/ pri_to_exp(&($1), &($$)); }
    | exp PLUOP primary {/*printf("exp : exp PLUOP primary\n");*/ exp_add_pri(&($$), &($1), &($3), 0); }
    | exp MINUSOP primary {/*printf("exp : exp MINUSOP primary\n");*/ exp_add_pri(&($$), &($1), &($3), 1); }
    | MINUSOP primary {/*printf("exp : MINUSOP primary\n");*/ neg_pri_to_exp(&($2), &($$)); }
    ;

primary : LPAREN exp RPAREN {/*printf("primary : LPAREN exp RPAREN\n");*/ exp_to_pri(&($2), &($$));}     // If there are two elements in the exp, compress it into one(with the use of TR), put it into primary_struct
        | ID {/*printf("primary : ID\n");*/ id_to_pri($1, &($$));}
        | INTLITERAL {/*printf("primary : INTLITERAL\n");*/ int_to_pri($1, &($$)); }
        ;
%%

// Clean all the TRs.
void clean_tr(){
  int i;
  for(i = 0; i < TR_STORAGE; i++){
    tr_regs_status[i] = 0;
  }
  // Memory expansion
  for(i = 0; (i < TR_STORAGE) && (tr_in_memory > 0); i++){
    if(memory_access[i] == 2){
      memory_access[i] = 0;
      tr_in_memory--;
    }
  }
}

// Retrieve type and the index of register from string. -> Type: 0 for SR, 1 for TR.
void check_index(char* printed_reg_name, int* type, int* index){
  int i, p;
  p = 0;
  char index_str[32];
  if(printed_reg_name[1] == 't') *type = 1;
  else *type = 0;
  for(i = 2; printed_reg_name[i] != '\0'; i++){
    index_str[p] = printed_reg_name[i];
    p++;
  }
  index_str[p] = '\0';
  *index = atoi(index_str);
}

// Find the index of token name (SR)
int find_id(char* name){
  // Check if the token name has preserved in some SR. If yes, return the id of the SR.
  int i;
  for(i = 0; i < sr_regs_status; i++){
    if(strcmp(name, sr_id_name[i]) == 0){
      return i;
    }
  }
  // If not, bind it to an new SR and return the name of the SR.
  strcpy(sr_id_name[sr_regs_status], name);
  // Memory expansion: Store info on Memory
  if(sr_regs_status >= 8){
    for(i = 0; i < MEM_STORAGE; i++){
      if(memory_access[i] == 0){
        memory_access[i] = 1;
        memory_index[i] = sr_regs_status;
        break;
      }
    }
  }
  sr_regs_status++;
  return sr_regs_status - 1;
}

void id_to_pri(char* id, struct primary_struct *primary_str){
  int index = find_id(id);
  char name[32];
  sprintf(name, "$s%d", index);
  strcpy(primary_str->reg_name, name);
  primary_str->is_int = 0;
}

void int_to_pri(int value, struct primary_struct *primary_str){
  primary_str->is_int = 1;
  primary_str->int_value = value;
}

// Find the shift amount, by the register type and index.
// For example, find the shift amount of $t11
// Caution: the target register must already exists in memory.
// Type: 0 for SR, 1 for TR.
int find_sa(int type, int index){
  int sa;
  for(sa = 0; sa < MEM_STORAGE; sa++){
    if(memory_access[sa] == type + 1 && memory_index[sa] == index){
      break;
    }
  }
  return (-4)*sa;
}

// Member functions
// assign: print out the assign info, but not modify it.
void assign(struct exp_struct *e_str, char* dst){
  char s_name[32];
  char c1[32];
  char c2[32];
  char destination[32];
  strcpy(destination, dst);

  if(e_str->reg_capacity == 2){
    if(e_str->sign){
      strcpy(s_name, "sub");
    }
    else{
      strcpy(s_name, "add");
    }
    strcpy(c1, e_str->first_reg_name);
    strcpy(c2, e_str->sec_reg_name);
  }
  else if(e_str->reg_capacity == 1 && e_str->int_capacity == 1){
    if(e_str->sign){
      strcpy(s_name, "subi");
    }
    else{
      strcpy(s_name, "addi");
    }
    strcpy(c1, e_str->first_reg_name);
    sprintf(c2, "%d", e_str->first_int_value );
  }
  else if(e_str->reg_capacity == 1){
    strcpy(s_name, "add");
    strcpy(c1, e_str->first_reg_name);
    strcpy(c2, "$zero");
  }
  else if(e_str->int_capacity == 1){
    strcpy(s_name, "addi");
    sprintf(c2, "%d", e_str->first_int_value );
    strcpy(c1, "$zero");
  }

  // Print out the info.
  // Memory expansion: check if c1 and c2 are in the memory.
  int c1_index, c2_index, c1_type, c2_type, dst_type, dst_index;
  check_index(c1, &(c1_type), &(c1_index));
  check_index(c2, &(c2_type), &(c2_index));
  check_index(destination, &(dst_type), &(dst_index));

  int sa;
  if(c1_index >= 8){
    strcpy(c1, "$t8");
    printf("lw $t8 %d($sp)\n", find_sa(c1_type, c1_index));
  }
  if(c2_index >= 8){
    strcpy(c2, "$t9");
    printf("lw $t9 %d($sp)\n", find_sa(c2_type, c2_index));
  }
  if(dst_index >= 8){
    strcpy(destination, "$t8");
  }

  printf("%s %s, %s, %s\n", s_name, destination, c1, c2);

  if(dst_index >= 8){
    printf("sw $t8 %d($sp)\n", find_sa(dst_type, dst_index));
  }

}

void assign_and_search(struct exp_struct *e_str, char* id_sr){

  char target[32];
  // Find the SR id according to the token name.
  int SR_id = find_id(id_sr);
  sprintf(target, "$s%d", SR_id);
  assign(e_str, target);

  // Clean the TR as statement ends
  clean_tr();
}

void add_to_id_list(char* name){
  int i = find_id(name);
  printf("addi $v0, $zero, 5\n");
  printf("syscall\n");
  if(i >= 8){
    printf("%s %s, %s, %s\n", "add", "$t8", "$v0", "$zero");
    printf("sw $t8, %d($sp)\n", find_sa(0, i));
  }
  else{
    printf("%s %s%d, %s, %s\n", "add", "$s",i , "$v0", "$zero");
  }
}

void add_to_exp_list(struct exp_struct *exp_str){
  printf("addi $v0, $zero, 1\n");
  assign(exp_str, "$a0");
  printf("syscall\n");
}

void pri_to_exp(struct primary_struct *primary_str, struct exp_struct *exp_str){
  // Step 1: Check the type of the primary
  if(primary_str->is_int){
    exp_str->first_int_value = primary_str->int_value;
    exp_str->int_capacity = 1;
    exp_str->reg_capacity = 0;
  }
  else{
    strcpy(exp_str->first_reg_name, primary_str->reg_name);
    exp_str->reg_capacity = 1;
    exp_str->int_capacity = 0;
  }
}

// Allocate a new TR, and return its index.
int allocate_tr(){
  int i, index;
  for(index = 0; index < TR_STORAGE; index++){
    if(tr_regs_status[index] == 0){
      tr_regs_status[index] = 1;
      break;
    }
  }
  if(index < 8) return index;
  tr_in_memory++;
  for(i = 0; i < MEM_STORAGE; i++){
    if(memory_access[i] == 0){
      memory_access[i] = 2;
      memory_index[i] = index;
      break;
    }
  }
  return index;
}

// Free TR with indicated index.
void free_tr(int index){
  tr_regs_status[index] = 0;
  int i;
  if(index >= 8){
    for(i = 0; i < MEM_STORAGE; i++){
      if(memory_access[i] == 2 && memory_index[i] == index){
        memory_access[i] = 0;
        break;
      }
    }
    tr_in_memory--;
  }
}

void neg_pri_to_exp(struct primary_struct *primary_str, struct exp_struct *exp_str){
  if(primary_str->is_int){
    exp_str->int_capacity = 1;
    exp_str->first_int_value = -1 * primary_str->int_value;
    exp_str->reg_capacity = 0;
    return;
  }
  struct exp_struct new_exp_str;
  new_exp_str.reg_capacity = 2;
  new_exp_str.int_capacity = 0;
  new_exp_str.sign = 1;
  strcpy(new_exp_str.first_reg_name, "$zero");
  strcpy(new_exp_str.sec_reg_name, primary_str->reg_name);
  int index = allocate_tr();
  char name[32];
  sprintf(name, "$t%d", index);
  assign(&new_exp_str, name);
  strcpy(exp_str->first_reg_name, name);
  exp_str->int_capacity = 0;
  exp_str->reg_capacity = 1;
}

void exp_to_pri(struct exp_struct *exp_str, struct primary_struct *primary_str){
  // Only valid when exp_str has two register loads.
  // assert(exp_str->reg_capacity == 2);

  // Step 1: Find possible TR
  int i, is_first_TR, is_second_TR, type, index;
  is_first_TR = is_second_TR = 0;
  if(exp_str->first_reg_name[1] == 't'){
    is_first_TR = 1;
  }
  if(exp_str->sec_reg_name[1] == 't'){
    is_second_TR = 1;
  }

  // Step 2: If == 0, allocate new one; if == 2, eliminate one.
  if(is_first_TR && is_second_TR){
    // Eliminate one
    check_index(exp_str->sec_reg_name, &type, &index);
    strcpy(primary_str->reg_name, exp_str->first_reg_name);
  }
  else if(!is_first_TR && !is_second_TR){
    // allocate new one
    index = allocate_tr();
    char name[32];
    sprintf(name, "$t%d", index);
    strcpy(primary_str->reg_name, name);
  }
  else{
    // Migrate name in exp to primary. 
    if(is_first_TR){
      strcpy(primary_str->reg_name, exp_str->first_reg_name);
    }
    else if(is_second_TR){
      strcpy(primary_str->reg_name, exp_str->sec_reg_name);
    }
  }
  // Step 3: Print out the MIPS code.
  assign(exp_str, primary_str->reg_name);
  primary_str->is_int = 0;

  // Step 4: If two of them are both TR, eliminate one.
  if(is_first_TR && is_second_TR){
    free_tr(index);
  }
      
}

void exp_add_pri(struct exp_struct *target, struct exp_struct *source1, struct primary_struct *source2, int applied_sign){
  if(source1->reg_capacity == 2){
    struct primary_struct new_prime;
    exp_to_pri(source1, &(new_prime));
    strcpy(target->first_reg_name, new_prime.reg_name);
    target->sign = applied_sign;
    if(source2->is_int){
      target->first_int_value = source2->int_value;
      target->reg_capacity = 1;
      target->int_capacity = 1;
    }
    else{
      strcpy(target->sec_reg_name, source2->reg_name);
      target->reg_capacity = 2;
      target->int_capacity = 0;
    }
  }
  else if(source1->reg_capacity == 1 && source1->int_capacity == 1){
    if(source2->is_int){
      target->first_int_value += source2->int_value * (applied_sign ? -1 : 1);
    }
    else{
      // Adding two of the regs. 
      int temp = source1->first_int_value;
      int temp_sign = source1->sign;
      strcpy(source1->sec_reg_name, source2->reg_name);
      source1->sign = applied_sign;
      source1->reg_capacity = 2;
      source1->int_capacity = 0;
      struct primary_struct new_prime;
      exp_to_pri(source1, &(new_prime));
      strcpy(target->first_reg_name, new_prime.reg_name);
      target->sign = temp_sign;
      target->first_int_value = temp;
      target->int_capacity = 1;
      target->reg_capacity = 1;
    }
  }
  else if(source1->reg_capacity == 1){
    strcpy(target->first_reg_name, source1->first_reg_name);
    if(source2->is_int){
      target->first_int_value = source2->int_value * (applied_sign ? -1 : 1);
      target->int_capacity = 1;
      target->reg_capacity = 1;
    }
    else{
      strcpy(target->sec_reg_name, source2->reg_name);
      target->sign = applied_sign;
      target->int_capacity = 0;
      target->reg_capacity = 2;
    }
    
  }
  else if(source1->int_capacity == 1){
    if(source2->is_int){
      target->first_int_value += source2->int_value * (applied_sign ? -1 : 1);
      target->int_capacity = 1;
      target->reg_capacity = 0;
    }
    else{
      if(applied_sign){
        // int - reg(primary) == $zero - reg(primary) + int
        struct exp_struct new_str;
        struct primary_struct new_p;
        strcpy(new_str.first_reg_name, "$zero");
        strcpy(new_str.sec_reg_name, source2->reg_name);
        new_str.sign = 1;
        new_str.int_capacity = 0;
        new_str.reg_capacity = 2;
        exp_to_pri(&(new_str), &(new_p));
        strcpy(target->first_reg_name, new_p.reg_name);
        target->first_int_value = source1->first_int_value;
        target->int_capacity = 1;
        target->reg_capacity = 1;
      }
      else{
        // int + reg(primary)
        strcpy(target->first_reg_name, source2->reg_name);
        target->int_capacity = 1;
        target->reg_capacity = 1;
        target->sign = 0;
      }
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
