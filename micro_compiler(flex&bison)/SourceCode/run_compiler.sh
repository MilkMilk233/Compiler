make clean
make
./micro ../TestCases/test$1.m > ../TestCases/test$1.txt
echo "test$1 done!!"