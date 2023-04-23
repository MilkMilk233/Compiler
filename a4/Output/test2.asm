    if(a != 0){
        a = a + 1;
    }

if_stmt -> IF LPAR exp RPAR code_block
if_statement -> if_stmt


    if(a != 0){
        a = a + 1;
    }
    else if(b != 0){
        b = b + 1;
    }
    else{
        c = c + 1;
    }

if_stmt -> IF LPAR exp RPAR code_block

if_stmt -> IF LPAR exp RPAR code_block
if_statement -> if_stmt ELSE code_block.

if_statement -> if_stmt ELSE code_block.


    if(a != 0){
        a = a + 1;
    }
    else{
        c = c + 1;
    }

if_stmt -> IF LPAR exp RPAR code_block
if_statement -> if_stmt ELSE code_block.