# CSC4180 Assignment 1 Design Docs

[TOC]

## 1. Flex(Lex) & Bison (YACC): Introduction

> flex文件是定义pattern（哪是黄豆，哪是绿豆...），通过flex处理（词法分析）将输出切分成一段一段的token（将输入的豆子一个个摘出来），从而执行不同的action（黄豆就磨豆浆（action），绿豆去做绿豆糕(action)）...
> flex 生成的[tokens](https://www.zhihu.com/search?q=tokens&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"article"%2C"sourceId"%3A"120812270"})可以喂给Bison处理（更简便易调试），当然也可以不喂给bison而直接自己处理就得了。但是使用bison可以更方便的处理复杂的逻辑，编写简单，调试方便。
>
> -- By 扬眉 from [Flex(scanner)/Bison(parser)词法语法分析工作原理 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/120812270)



### 1.1 Flex

#### 1.1.1 Matching Rules

Every time the Flex reads an token from left to right. The newly read characters will be stored in a stack. As every time the flex reads new characters to the stack, flex will search if any matching rules are correspond to the current string in stack. If yes, pop the stack, apply the matching rules to the strings popped out, and execute the actions inside the `{}`.

Like what is discussed in tutorials, the lexer (flex) follows these matching rules:

- Longest matching rules: if more than one matching string is found, the rule that generates the longest one is selected.
- First rule: if more than one string with the same length is matched, the rules listed first will be triggered.
- Default action: If no rules are matched, the next character in input is considered matched implicitly and copied to the output string, like what `echo` will do.

After each successful match, the action inside the `{}` will be executed, we can use it to:

1. Process the matched data (The latest matched string is stored in `yytext`)
2. Return the token to the Bison(yacc). 
3. Carry parameters(e.g., the matched data string, some certain value etc. ) along with the token to Bison(yacc). (Store the token into `yyval`)

#### 1.1.2 Comment line elimination

In Micro language, the comment line has the prefix `--`. We can setup multiple scanners to eliminate the comments.

In this case, when the lexer reads `--`, it will turn into `<COMMENT>` mode from `<INITIAL>` mode. In `<COMMENT>` mode, everything will be omitted, until the lexer reads`\n`, the mode turn into `<INITIAL>` again.



### 1.2 Bison

If we say flex is to deal with token-character relationship, then bison is to handle the token-token relationship. 

#### 1.2.1 Stacking & Regression

Similarly, every time the Bison reads an token passed from the flex. The newly imported tokens will be stored in a stack. As every time the bison reads new characters to the stack, bison will search if any matching rules are correspond to the current tokens in stack. If yes, pop the stack, apply the matching rules to the tokens popped out, and execute the actions inside the `{}`.

Remark: the bison will look forward one token for deciding whether to pop it out(regression) of keep it on(stack). This depends on the precedence of the tokens. We can define the precedence by the order of occurrence in the definition session.

Also, we can also defines the associativity. It means that, when cases like "A op B op C", in which op are the same, we can decide whether to regress or stack when processing B. Some of them are left-associativity, like `+`, `-`, `*` and `/`. Some of them are right-associativity, like `^`. There are some non-associativity, like `==`, which means `A == B == C` is illegal.

#### 1.2.2 Token & Types

Since the Flex(lex) has already passed tokens to the Bison, now we can use our pre-defined rules to (possibly) form an syntax tree (AST). Some of the tokens has no parameter, some of the tokens are carrying some parameters. For those tokens which carries parameters, we should declare what type of value they are carrying. Therefore, we should declare "type" in the definition parts, like:

```
%token BEGIN_ END READ WRITE 
%token <value> INTLITERAL
%token <id> ID
```

In the example above, the token `BEGIN_`, `END`, `READ`, `WRITE`, they don't need to carry parameters. Therefore we don't need to declare what data type they are carrying, or even we can ignore the declaration (just simply not to write `%token xxx` in the definition part, it's okay). As for the token `INTLITERAL`, `ID`, they carried some data from lex side, so we should declare what the data type is.

To be mention that, all the data type (like the above `value`, `id`) should also be included in the `union` session.

For some intermediate representations (what we discuss later), if it's also carrying data, we should also declare its data type.

```
%type <primary_str> primary		// <primary_str> is the data type carried by IR "primary"
%type <exp_str> exp 
// If the IR does not carry any data, we can simply ignore it.
```

In later actions (inside the `{}`), we can manipulate the token data, retrieve & analysis it, or pass it to another token. So I'd like to call it "token-token relationship". For example,

```yaml
exp : primary {pri_to_exp(&($1), &($$)); }
```

When the action `pri_to_exp(&($1), &($$));` is triggered, the data stored in `primary` is transformed and passed into the `exp`. And the `primary` and `exp` acts the role of data-container, can be retrieved by some actions.



## 2. Implementation

### 2.1 Translations into MIPS code

We mainly uses **simulation** method, i.e., **to generate MIPS code simultaneously when generating AST syntax tree on Bison.**

In general, we focus on three categories for output:

```
Categoty 1 Assignment: A := B + C + D
Categoty 2 Read: 	   read(A, B)
Category 3 Write:      write(A, A+B)
```

Assume A is in $s0, B is in $s1, C is in $s2. Let's observe what will happen when parsing `A := B + C + D`.  (The comment will be explained later, and the context shows the order of how Bison's parsing works)

```yaml
...
primary : ID	// Reading `B`, put B into the primary structure
exp : primary	// Upgrade `B` into the exp (with one single primary now)
primary : ID	// Reading `C`, put C into the primary structure
exp : exp PLUOP primary		// Insert primary (C) into the exp (now exp have two primary)
primary : ID	// Reading `D`, put D into the primary structure
exp : exp PLUOP primary		// Try to Insert primary (D), but exp now is full
// Compress exp to one primary, then combine the two primary into new exp
statement : ID ASSIGNOP exp ';'	 	// Output the info
...
```

Assume A is in $s0, B is in $s1. Let's observe what will happen when parsing `read(A, B)`. 

```YAML
...
id_list : ID
id_list : id_list COMMA ID
...
```

Assume A is in $s0, B is in $s1. Let's observe what will happen when parsing `write(A, A+B)`.

```yaml
...
primary : ID
exp : primary
exp_list: exp
primary : ID
exp: primary
primary : ID
exp: exp PLUOP primary
exp_list: exp_list COMMA exp
...
```

 With the parsing process, we can generate MIPS code simultaneously with the use of actions in `{}`.

#### 2.1.1 Data structure

First, we need to find (or define) some proper data structure, to preserve some important information, or to pass data from one token to another. 

**REMARK**: Different from calculators, during MIPS translations, **we don't need to know exactly any of the value.** What we do is simply print AST orders in an MIPS way. So we can simply preserve the name of the registers, And the key information (token name) is passed among the token attributes. 

So the tokens that need to preserve data is:

1. primary (intermediate representation)
2. exp (intermediate representation)
3. ID
4. INTLITERAL

(As for the id_list and exp_list, actually we don't need it to store value, current data structure is quite enough to support all the functionalities. The details will be explained later)

And the question is, what type of data do we need to preserve? The answer is:

1. primary -> [is_int?] [int_value] [reg_name]
2. exp -> [reg_name#1] [reg_name#2] [int_value#1] [sign_bit] [int_num] [reg_num]
3. ID -> [token_name]
4. INTLITERAL -> [int_value]

Why we design in this way? Details will be explained later, since data structure is to feed the need of the functionalities.

#### 2.1.2 Assignment

Assignment is the basic expression in MIPS code. It appears in such form:

```
<op> <dst> <source1> <source2>
```

op can be `add`, `addi`, `sub`, `subi`. For the suffix `i`, it indicates that `<source2>` is a immediate (integer).

Now we consider a basic case: `A := B + C`.  In MIPS, it is expressed as:

```assembly
add $s0, $s1, $s2
```

We can observe that, it consist of two source registers, one destination registers, and a sign function.

In parsing process, the corresponding code is:

```yaml
statement : ID ASSIGNOP exp ';'
```

With the design above, we can obtain the data of `ID`, which contains the token name of dst, and `exp`, which is a structure contains 2 primary info that we need. Therefore, with the info we want, we can easily generate the MIPS code we want.

Now based on the simplest case, we further discuss what will happen when the number of ID on the LHS of the assignment is > 2. For example, we have `A := B + C + D`. Referring to the parsing process again, we can find that, 

1. First the parser will process `B+C`, integrate them into a primary structure. (`$t0 = B+c`)
2. Then combine the new primary structure with the `D`, and form another exp structure, which contains two primary structure. (`A = $t0 + D`)

With the data structure we designed, we can easily deal with such calculation above. Only to notice that, we need to be careful on some detailed corner cases. (Like the exp struct, sometimes it may contains 1 immediate + 1 register, or sometimes only 1 immediate (or 1 register), we have to consider all the scenarios. )

#### 2.1.3 Read & Write

Read & Write is another kind of special assignment. For the expression like:

```
read(A);
```

Actually in MIPS code, it's 

```assembly
addi $v0, $zero, 5
syscall
add $s0, $v0, $zero
```

What we do is to simply get what token `A`'s corresponding SR is, then generate the MIPS code. Done.

Similarly, `write()` is almost the same, just to calculate the expression structure, assign it to the special register, generate the MIPS code.

As we can see, the SR are where we store variable's value. and TR are where we store immediate value (to satisfy the need that MIPS calculation can only support up to two sources)

So far, once the SR is allocated, it can never be discarded, but as a statement ends, all the TR will be discarded. With this property, we can just set up a counter, see how many SR has been allocated.

We should maintain a table to record the relationship between the token name and the register name of SR, so that every time we receive some token name, we can first translate it into the register name.

##### 2.1.3.1 SW / LW Operations

How to allocate SR and TR registers? This requires us to maintain some mechanism to record which register are available, while which are occupied. By default, we have 8 register quota for SR and TR, we can maintain an integer array, as the register is occupied we set it to `1`, otherwise we set it to `0`.

If the index of SR exceeds 8, that means all the default registers are running out. We have to fetch it from the memory. The only difference between the normal fetch and the memory fetch is, the code below is needed:

```assembly
sw $t8 0($sp)
lw $t8 -4($sp)
...
```

i.e., we have to put it into the bridge registers (`$t8` and `$t9`), then execute `lw` or `sw` to fetch from or put into the memory.

##### 2.1.3.2 Virtual register mechanism

In practice, we establish a mechanism call "virtual register", i.e., we maintain some registers that does not exists in reality. For example, as the index of register exceeds 8, we still name it `$tx` or `$sx`, although they are in memory actually.

Assume a case, where 

```assembly
addi $s10 $s11 $s12
```

In real case, there won't be `$s10` or so, since there are only 8 SRs implemented. Here `$s10` represents the 10th saved registers allocated, where `$s8` and later are actually stored in the memory. When the system recognize the name of the register is abnormal (i.e., the index of the register is >= 8), it will simulate the memory fetching process, like what MIPS compiler really do.

*Remark: since the current Flex / Bison parsing mode cannot look back and see what's going on later, there may be some cases like "immediately fetch after store", this may not be the optimized one, but it's still correct.* 

### 2.2 Optimization

#### 2.2.1 Registers management

Register management is an important topic in our program. Since both TR and SR has the chance of running out, we have to put all the extra registers into the memory space. Therefore, we use two integer array to solve this problem.

```c
int memory_access[MEM_STORAGE];   // 0 for unoccupied, 1 for SR_occupied, 2 for TR_occupied
int memory_index[MEM_STORAGE]; // The actual index of each registers.
```

The first array, `memory_access` is used to record the occupation status of the register. If marked `0` -> unoccupied, marked `1` -> SR is occupying, marked `2` -> TR is occupying.

The second array, `memory_index` is used to record the actual index of each register, which corresponds to the same position on the first array. For example, virtual register `$t8` is stored in the 1st place in memory, `$s8` is stored 2nd, `$t9` is stored 3rd, `$s9` is stored 4th. Rest of them are unoccupied.  Since `memory_access` preserve the occupation of the registers, so it should be `[2,1,2,1,0,0,...]`.  As for `memory_index`, it will record all the index of the registers, so it should be `[8,8,9,9,...]`. So when we checked 1st place in memory, first we know `memory_access[0] == 2` -> It is occupied by TR; then `memory_index[0] == 8` -> TR#8 is occupying the 1st place in memory.

### 2.3 Topology of MICRO compiler

Here are the CFG grammar of MICRO compiler:

```
1. <program> → BEGIN <statement list> END
2. <statement list > → <statement> {<statement>}
3. <statement> → ID ASSIGNOP <expression>;
4. <statement> → READ LPAREN <id list> RPAREN;
5. <statement> → WRITE LPAREN<exp list> RPAREN;
6. <id list > → ID {COMMA ID}
7. <expr list > → <expression> {COMMA <expression>}
8. <expression> → <primary> {<add op> <primary>}
9. <primary> → LPAREN <expression> RPAREN
10. <primary> → ID
11. <primary> → INTLITERAL
12. <add op> → PLUOP
13. <add op> → MINUSOP
14. <system goal> → <program> SCANEOF
```

Based on the CFG grammar, we implemented our Bison grammar rules:

```yaml
%start system_goal
%%
system_goal : program SCEANOF {;}
            ;

program : BEGIN_ statement_list END {;}
        ;

statement_list : statement {;}
               | statement_list statement {;}
               ;

statement : ID ASSIGNOP exp ';' {assign_and_search(&($3), $1);}
          | READ LPAREN id_list RPAREN ';' {clean_tr();}
          | WRITE LPAREN exp_list RPAREN ';' {clean_tr();}
          ;

id_list : ID {add_to_id_list($1); clean_tr();}
        | id_list COMMA ID {add_to_id_list($3); clean_tr();}
        ;

exp_list : exp {add_to_exp_list(&($1)); clean_tr();}
         | exp_list COMMA exp {add_to_exp_list(&($3)); clean_tr();}
         ;

exp : primary {pri_to_exp(&($1), &($$)); }
    | exp PLUOP primary {exp_add_pri(&($$), &($1), &($3), 0); }
    | exp MINUSOP primary {exp_add_pri(&($$), &($1), &($3), 1); }
    | MINUSOP primary {neg_pri_to_exp(&($2), &($$)); }
    ;

primary : LPAREN exp RPAREN {exp_to_pri(&($2), &($$));}     
        | ID {id_to_pri($1, &($$));}
        | INTLITERAL {int_to_pri($1, &($$)); }
        ;
```

## 3. Appendix

### 3.1 Reference

[Flex(scanner)/Bison(parser)词法语法分析工作原理 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/120812270)

[手把手教程-lex与yacc/flex与bison入门（一）（使用windows环境）_D-Cierra的博客-CSDN博客_bison教程 [Archive time: 2023/2/23]](https://blog.csdn.net/weixin_44007632/article/details/108666375)

[c - Include struct in the %union def with Bison/Yacc - Stack Overflow](https://stackoverflow.com/questions/1430390/include-struct-in-the-union-def-with-bison-yacc)

[Course Tutorial 1-4]

### 3.2 API Table

#### 3.2.1 Base function (low-level)

| Function Name | Input                 | Output      | Remark                                                       |
| ------------- | --------------------- | ----------- | ------------------------------------------------------------ |
| assign()      | expression, dst (raw) | NULL        | print equation (assignment), like "dst = $A + $B"            |
| find_id()     | token name            | index of SR | Check if the token name has already been preserved in some SR. If yes, return the index of the SR. If not, bind it to an new SR and return the index of the SR. |
| clean_tr()    | NULL                  | NULL        | Clean all TR registers                                       |
| check_index() | printed_reg_name      | type, index | Retrieve type and the index of register from string. -> Type: 0 for SR, 1 for TR. |

#### 3.2.2 Advanced function (high-level)

| Function Name       | Input                  | Output               | Remark                                                       |
| ------------------- | ---------------------- | -------------------- | ------------------------------------------------------------ |
| assign_and_search() | expression, id_SR      | NULL                 | Dev based on assign(), translate token name to SR name, then print it out as dst(raw) |
| id_to_pri()         | id                     | primary_str          | translate the token name into register name, then put it into the primary structure |
| int_to_pri()        | int                    | primary_str          | put the integer into the primary structure                   |
| add_to_id_list()    | token name             | NULL                 | translate the token name into register name, then simulate MIPS's input reading process |
| add_to_exp_list()   | expression structure   | NULL                 | simulate MIPS's output printing process, export expression structure |
| pri_to_exp()        | primary_str            | expression structure | put a (positive) primary into a two-primary capable expression structure |
| neg_pri_to_exp()    | primary_str            | expression structure | put a (negative) primary into a two-primary capable expression structure. If the primary_str consists of an ID instead of integer, first calculate `sub $tx, $zero, $sx`, then put `$tx` into the primary structure. |
| exp_to_pri()        | expression structure   | primary_str          | Integrate expression (with 2 or 1 primary) to one primary.   |
| exp_add_pri()       | exp, primary, sign bit | target exp           | First integrate expression (with 2 or 1 primary) to one primary, then add or substract with the new primary, forming the new structure. |



### 3.3 Q&A

1. How to handle negative numbers in registers ?  -> Don't worry, MIPS will help you with that.
2. How do I know it's `add` or `addi`? -> Add extra marks on `struct exp_struct` to inform. 
3. How to deal with expr like `$s1 = 3 - $s0`? -> Separate it into an half, `$t0 = $zero - $s0`, `$s1 = 3 - $t0`. 

### 3.4 File structure

```bash

.
|-- csc4180-a1-120090222.Dockerfile
|-- csc4180-a1-120090222-report.pdf
`-- SourceCode
    |-- makefile
    |-- micro.l
    |-- micro.y
    |-- run_compiler.sh
    |-- test1.m
    |-- test2.m
    |-- test3.m
    |-- test4.m
    |-- test5.m
    |-- test6.m
    `-- test7.m
```

### 3.5 How to execute the program?

1. Place all the test file under `./SourceCode/`

2. Execute the following instructions: 

   **CAUTION: ADD "`--privileged=true` ON `docker run` COMMAND"**

```bash
(base) −> docker build -f csc4180-a1-120090222.Dockerfile -t csc4180-a1-120090222 .
(base) −> docker run -it --privileged=true --mount type=bind,source=/root/Compiler/a1_submission/SourceCode,target=/opt/compiler_src/ csc4180-a1-120090222 bash
−> cd /opt/compiler src
−> make clean
−> make all
−> ./run_compiler.sh test0.m
addi $t0, $s0, -314
add $t0, $t0, $s1
add $s1, $t0, $zero
```


*P.S.*

*Using objective-C just like `****** * ****`, without the help of modern data structure (like <vector>, <unordered_map> in C++ STL), just feel like implementing customized embedding-system codes, so fxxking awesome ~.~ (Believe it or not!)*