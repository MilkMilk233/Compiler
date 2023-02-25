/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_MICRO_PARSER_H_INCLUDED
# define YY_YY_MICRO_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 26 "micro.y" /* yacc.c:1909  */


  struct id_list_struct{
    int capacity;
    int id_l_struct[50];    // Store index of SR
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
  void add_to_exp_list(struct exp_list_struct *exp_l_struct, struct exp_struct *exp_str);
  void pri_to_exp(struct primary_struct *primary_str, struct exp_struct *exp_str);
  void neg_pri_to_exp(struct primary_struct *primary_str, struct exp_struct *exp_str);
  void exp_to_pri(struct exp_struct *exp_str, struct primary_struct *primary_str);
  void exp_add_pri(struct exp_struct *target, struct exp_struct *source1, struct primary_struct *source2, int applied_sign);


#line 89 "micro.parser.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LEX_ERR = 258,
    BEGIN_ = 259,
    END = 260,
    READ = 261,
    WRITE = 262,
    INTLITERAL = 263,
    ID = 264,
    LPAREN = 265,
    RPAREN = 266,
    COMMA = 267,
    ASSIGNOP = 268,
    PLUOP = 269,
    MINUSOP = 270,
    SCEANOF = 271
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 71 "micro.y" /* yacc.c:1909  */

  char id[32];
  int value;
  struct exp_list_struct exp_list_str;
  struct id_list_struct id_list_str;
  struct exp_struct exp_str;
  struct primary_struct primary_str;

#line 127 "micro.parser.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MICRO_PARSER_H_INCLUDED  */
