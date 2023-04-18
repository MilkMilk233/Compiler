main:
    # Var declaration
        li     $t0, 3
        move   $s0, $t0
    # 1st if statement
        li     $t0, 3
        sgt    $t1, $s0, $t0
        li     $t2, 0
        bne    $t1, $t2, .B1
        beq    $t1, $t2, .B2
.B1

.B2
