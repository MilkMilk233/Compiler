all : scanner

scanner : scanner.cpp parser.cpp shift_map.cpp
	g++ scanner.cpp parser.cpp shift_map.cpp -std=c++11 -o scanner

clean : 
	rm scanner