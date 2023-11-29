# Specification - Parser

### My LR(1) Grammar

> To be specific, it is not standard LR(1) design in grammar, since there exists shift-reduce conflict in `if-else statemets`.  I deal with shift-reduce conflicts by manually controling what to behave when encountering conflicts.
> 

**What’s the difference compared with example CFG?**

1. `declaration -> ID ASSIGN exp | ID LSQUARE exp RSQUARE | ID.` I change “NUMBER” to “exp”, to better deal with number with positive / negative sign.
2. `program -> var_declarations statements | statements`, Rewrite the structure to make `var_declarations` optional.
3. Rewrite the `exp` session, adding priority levels, make it under LR(1) grammar. *(For priority level in C language, I referred to* [this site](https://www.zhihu.com/question/482207875/answer/2087439250)*)*

Below are my LR(1) grammar:

```bash
program -> var_declarations statements | statements
var_declarations -> var_declarations var_declaration | var_declaration
var_declaration -> INT declaration_list SEMI
declaration_list -> declaration_list COMMA declaration | declaration
declaration -> ID ASSIGN exp | ID LSQUARE exp RSQUARE | ID
code_block -> statement | LBRACE statements RBRACE
statements -> statements statement | statement
statement -> assign_statement SEMI | control_statement 
						| read_write_statement SEMI | SEMI
control_statement -> if_statement | while_statement | do_while_statement SEMI
										 | return_statement SEMI
read_write_statement -> read_statement | write_statement
assign_statement -> ID LSQUARE exp RSQUARE ASSIGN exp | ID ASSIGN exp
if_statement -> if_stmt | if_stmt ELSE code_block
if_stmt -> IF LPAR exp RPAR code_block
while_statement -> WHILE LPAR exp RPAR code_block
do_while_statement -> DO code_block WHILE LPAR exp RPAR
return_statement -> RETURN
read_statement -> READ LPAR ID RPAR
write_statement -> WRITE LPAR exp RPAR

exp -> exp OROR exp1
exp -> exp1
exp1 -> exp1 ANDAND exp2
exp1 -> exp2
exp2 -> exp2 OR_OP exp3
exp2 -> exp3
exp3 -> exp3 AND_OP exp4
exp3 -> exp4
exp4 -> exp4 EQ exp5
exp4 -> exp4 NOTEQ exp5
exp4 -> exp5
exp5 -> exp5 LT exp6
exp5 -> exp5 GT exp6
exp5 -> exp5 LTEQ exp6
exp5 -> exp5 GTEQ exp6
exp5 -> exp6
exp6 -> exp6 SHL_OP exp7
exp6 -> exp6 SHR_OP exp7
exp6 -> exp7
exp7 -> exp7 PLUS exp8
exp7 -> exp7 MINUS exp8
exp7 -> exp8
exp8 -> exp8 MUL_OP exp9
exp8 -> exp8 DIV_OP exp9
exp8 -> exp9
exp9 -> MINUS exp9
exp9 -> PLUS exp9
exp9 -> NOT_OP exp9
exp9 -> exp10
exp10 -> INT_NUM
exp10 -> ID
exp10 -> ID LSQUARE exp RSQUARE
exp10 -> LPAR exp RPAR
```

### LR(1) Parsing table design

I designed a LR(1) parsing table with 431 states:

![Untitled](Compiler%20Project%203%20-%20Scanner%204aea326e4c664360bf68b5414af22b07/Untitled.png)

To access the full parsing table, please refer to:

[https://ly-blog.oss-cn-shenzhen.aliyuncs.com/static/CSC4180/LR(1)_parsing_table.xlsx](https://ly-blog.oss-cn-shenzhen.aliyuncs.com/static/CSC4180/LR(1)_parsing_table.xlsx) *(Expires until 2024/3)*

### Parsing with Parsing table

I maintained two data structures to preserve the LR(1) parsing table info:

```cpp
vector<unordered_map<string, int>> shift_map(431);
vector<unordered_map<string, vector<string>>> reduce_map(431);

/* Example of shift map: */

// Means in state 0, next token = "INT", shift to state 6
shift_map[0]["INT"] = 6;   
// Means in state 2, next token = "control_statement", shift to state 8
shift_map[2]["control_statement"] = 8;  
...

/* Example of reduce map: */

// Means in state 4, next token = "EOF", reduce(program -> statements)
// Production = "program", states to pop = 1
reduce_map[3]["EOF"] = {"1", "program", "program>statements"};
...
```

**Parsing algorithm:**

- Shift(n): Advance input one token; push n on stack.
- Reduce(k):
    - Pop stack as many times as the number of symbols on the right-hand
    side of rule k;
    - Let X be the left-hand-side symbol of rule k;
    - In the state now on top of stack, look up X to get "goto n";
    - Push n on top of stack.
- Accept: Stop parsing, report success.
- Error: Stop parsing, report failure.

### How to run the program:

Programming language: C++ => `g++` is needed.

Dockerfile content:

```
FROM ubuntu:18.04

RUN apt-get clean && apt-get update && apt-get install make g++ -y

```

File structure:

```powershell
.
|-- parser.Dockerfile
|-- parser-report.pdf
`-- SourceCode
    |-- compiler.h
    |-- makefile
    |-- parser.cpp
    |-- reduce_map.cpp
    |-- scanner.cpp
    |-- shift_map.cpp
    `-- TestCases
        |-- test1.c
        |-- test2.c
        |-- test3.c
        |-- test4.c
        `-- test5.c

```

⭐︎**IMPORTANT: please put test files under`"TestCases"` folder**.

It will take a little bit longer to compiler (~15s). Relax!

```bash
(base) −> docker build -f parser.Dockerfile -t parser .
(base) −> docker run -it --privileged=true --mount type=bind,source=/root/Compiler/parser/SourceCode,target=/opt/parser_src/ parser bash
−> cd /opt/parser_src
−> make clean
−> make
−> ./parser test1.c
Scanned Tokens:
INT ID SEMI INT ID SEMI ID ASSIGN ID PLUS INT_NUM SEMI RETURN SEMI EOF

Parsing Process:
state: 0        next type: INT          shift to state 6
current situation: INT |

state: 6        next type: ID           shift to state 30
current situation: INT ID |
...
```