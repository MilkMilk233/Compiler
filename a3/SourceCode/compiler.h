#ifndef _COMPILER_H
#define _COMPILER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#define file_path "./TestCases/"

class Scanner{
    private:
        vector<unordered_map<char, int>> dfa_states_vector;
        unordered_map<int, string> final_state_map;
        ifstream c_file;
        int current_state;
        vector<string> tokens;
        int print_index;
    public:
        void init_digits(unordered_map<char, int> &umap, int target);
        void init_letters(unordered_map<char, int> &umap, int target);
        void init_l_and_d(unordered_map<char, int> &umap, int target);
        Scanner(string file_name);
        string get_token();
        void scan();
        ~Scanner();
};

#endif