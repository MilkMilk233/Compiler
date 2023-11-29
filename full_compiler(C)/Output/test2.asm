Example code #1

    if(a != 0){
        a = a + 1;
    }

Parsing order:
1. if_stmt -> IF LPAR exp RPAR code_block   
2. if_statement -> if_stmt

Example code #2

    if(a != 0){
        a = a + 1;
    }
    else if(b != 0){
        b = b + 1;
    }
    else{
        c = c + 1;
    }

Parsing order:
1. if_stmt -> IF LPAR exp RPAR code_block   (First if)
2. if_stmt -> IF LPAR exp RPAR code_block   (Second if, i.e., else if)
3. if_statement -> if_stmt ELSE code_block. (End of the second if)
4. if_statement -> if_stmt ELSE code_block. (End of the first if)

Example code #3

    if(a != 0){
        a = a + 1;
    }
    else{
        c = c + 1;
    }

1. if_stmt -> IF LPAR exp RPAR code_block
2. if_statement -> if_stmt ELSE code_block.