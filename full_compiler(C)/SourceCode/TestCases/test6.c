int a = 1;
int b = 1;
int c = 1;
int d = 1;
int e = 1;
int f = 888;

if (a == 3){
    printf(f);
}
else if(a == 2){
    printf(f);
}
else{
    if(b == 2){
        printf(f);
    }
    else if(b == 1){
        if(c == 1){
            printf(666);
        }
        if(d == 2){
            printf(f);
        }
        else{
            printf(666);
        }
    }
    else{
        printf(f);
    }
}