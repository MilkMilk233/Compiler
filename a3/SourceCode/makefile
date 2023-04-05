all : scanner

scanner : scanner.cpp parser.cpp shift_map.cpp reduce_map.cpp
	g++ scanner.cpp parser.cpp reduce_map.cpp shift_map.cpp -std=c++11 -o scanner

clean : 
	rm scanner