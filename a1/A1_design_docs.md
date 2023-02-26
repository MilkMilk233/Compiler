# CSC4180 Assignment 1 Design Docs

Chen Zhixin, 120090222

[TOC]

## 1. Flex(Lex) & Bison (YACC): Introduction

> flex文件是定义pattern（哪是黄豆，哪是绿豆...），通过flex处理（词法分析）将输出切分成一段一段的token（将输入的豆子一个个摘出来），从而执行不同的action（黄豆就磨豆浆（action），绿豆去做绿豆糕(action)）...
> flex 生成的[tokens](https://www.zhihu.com/search?q=tokens&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"article"%2C"sourceId"%3A"120812270"})可以喂给Bison处理（更简便易调试），当然也可以不喂给bison而直接自己处理就得了（如下例）。但是使用bison可以更方便的处理复杂的逻辑，编写简单，调试方便。
>
> -- By 扬眉 from [Flex(scanner)/Bison(parser)词法语法分析工作原理 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/120812270)



### Flex

#### Matching Rules

```
yacc
```



### Bison

#### Token & Types

#### Union

#### Stacking & Regression



## 2. Implementation



### Registers



### Translations into MIPS code

We mainly uses **simulation** method, i.e., **to generate MIPS code simultaneously when generating AST syntax tree on Bison.**

Most importantly, we should focus on the `statement` part.

```
statement : ID ASSIGNOP exp ';'
          | READ LPAREN id_list RPAREN ';' 
          | WRITE LPAREN exp_list RPAREN ';' 
```

**REMARK**: Different from above, during MIPS translations, **we don't need to know exactly any of the value.** What we do is simply print AST orders in an MIPS way. And the key information (token name) is passed among the token attributes.

#### Assignment

For the statement: `ID ASSIGNOP exp ';'`, we should notice that: **SR only appears in this session**. (TR and relative calculation appears in `exp` and `primary` layer)

#### Read & Write

So far, once the SR is allocated, it can never be discarded. With this property, we can just set up a counter, see how many SR has been allocated.

##### SW / LW Operations

If the index of SR exceeds 8, we have to fetch it from the memory. The only difference between the normal fetch and the memory fetch is, the code below is needed:

```assembly
sw $t8 0($sp)
lw $t8 -4($sp)
...
```

##### Memory fetch mechanism

There would be possibilities that, 8 TRs and (or) 8 SRs are all running out. In this case, we need to push the late comer into the memory, also fetch them from the memory on demand. 

In MIPS design, `$t8` and `$t9` are two temporary registers that reserved for memory fetch mechanism. Assume a case, where 

```assembly
addi $s10 $s11 $s12
```

In real case, there won't be `$s10` or so, since there are only 8 SRs implemented. Here `$s10` represents the 10th saved registers allocated, where `$s8` and later are actually stored in the memory. When the system recognize the name of the register is abnormal (i.e., the index of the register is >= 8), it will simulate the memory fetching process, like what MIPS compiler really do.

*Remark: since the current Flex / Bison parsing mode cannot look back and see what's going on later, there may be some cases like "immediately fetch after store", this may not be the optimized one, but it's still correct.* 

### Optimization

#### Registers management



### Topology of MICRO compiler



## 3. Appendix

### Reference

[Flex(scanner)/Bison(parser)词法语法分析工作原理 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/120812270)

[手把手教程-lex与yacc/flex与bison入门（一）（使用windows环境）_D-Cierra的博客-CSDN博客_bison教程 [Archive time: 2023/2/23]](https://blog.csdn.net/weixin_44007632/article/details/108666375)

[c - Include struct in the %union def with Bison/Yacc - Stack Overflow](https://stackoverflow.com/questions/1430390/include-struct-in-the-union-def-with-bison-yacc)

[Course Tutorial 1-4]



### API Table

| Function Name       | Input                 | Output      | Remark                                                       |
| ------------------- | --------------------- | ----------- | ------------------------------------------------------------ |
| assign()            | expression, dst (raw) | void        | Basis of expression printing like "dst = $A + $B"            |
| assign_and_search() | expression, id_SR     | void        | Dev based on assign(), translate token name to SR name, then print it out as dst(raw) |
| find_id()           | token name            | index of SR | *Check if the token name has already been preserved in some SR. If yes, return the index of the SR. If not, bind it to an new SR and return the index of the SR.* |
| clean_tr()          | NULL                  | void        | Clean all TR registers                                       |
|                     |                       |             |                                                              |
|                     |                       |             |                                                              |
|                     |                       |             |                                                              |
|                     |                       |             |                                                              |
|                     |                       |             |                                                              |



### Q&A

1. How to handle negative numbers in registers ?  -> Don't worry, MIPS will help you with that.
2. How do I know it's `add` or `addi`? -> Add extra marks on `struct exp_struct` to inform. 
3. How to deal with expr like `$s1 = 3 - $s0`? -> Separate it into an half, `$t0 = $zero - $s0`, `$s1 = 3 - $t0`. 



*P.S.*

*Using objective-C just like in constipation, without the help of modern data structure (like <vector>, <unordered_map> in STL), just feel like implementing customized embedding-system codes, so fxxking awesome ~.~ (Believe it or not!)*