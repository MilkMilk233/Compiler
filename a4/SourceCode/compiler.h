#ifndef _COMPILER_H
#define _COMPILER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#define file_path "./TestCases/"

class token{
    public:
        string type_name;
        string token_value;
};

class Scanner{
    private:
        vector<unordered_map<char, int>> dfa_states_vector;
        unordered_map<int, string> final_state_map;
        ifstream c_file;
        int current_state;
        vector<token> tokens;
        int print_index;
    public:
        void init_digits(unordered_map<char, int> &umap, int target);
        void init_letters(unordered_map<char, int> &umap, int target);
        void init_l_and_d(unordered_map<char, int> &umap, int target);
        Scanner(string file_name);
        token get_token();
        void scan();
        void display_tokens();
        ~Scanner();
};

void init_shift_map(vector<unordered_map<string, int>> &shift_map);
void init_reduce_map(vector<unordered_map<string, vector<string>>> &reduce_map);

#endif