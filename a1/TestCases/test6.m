-- Expected Output: ???
begin
    A := 1;
    B := 2;
    C := 3;
    D := 4;
    E := 5;
    F := 6;
    G := 7;
    H := -A- (-B + (C -D) - (E - F)) - (-G);
    H := B+A+B + (A+B+A + (A+B+C + (A+C+D + (B+C+E + (D+E+F)))));
end