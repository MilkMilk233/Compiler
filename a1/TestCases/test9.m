begin
    A := 3 + (5 - 7);
    B := -3 + A - A;
    C := -B - A + (B + (-1));
    D := 3 - (-A + (-B));
    E := (A - B - C);
    F := (A + B + (1 - D));
    G := A + (B - C);
    H := 3 + B - 8 - (2 + 3);
    I := A + B - C + D + E + F;
    write(A);   -- 1
    write(B);   -- -3
    write(C);   -- -2
    write(D);   -- 1
    write(E);   -- 6
    write(F);   -- -2
    write(G);   -- 0
    write(H);   -- -13
    write(I);   -- 5
    write(A,B,C,D,E,F,G,H,I);
    write(A+B,B+C,C+(D-C),H-I);
end