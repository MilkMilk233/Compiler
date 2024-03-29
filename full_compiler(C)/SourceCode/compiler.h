#ifndef _COMPILER_H
#define _COMPILER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream> 
#define file_path "./TestCases/"

class token{
    public:
        string type_name;
        string token_value;
        int tr_value;
        int end_label;
        stringstream ss;
        token(string t_name, string t_value, int tr_val) : type_name(t_name), token_value(t_value), tr_value(tr_val), end_label(-1) {
            ss.str("");
        }
        token(string t_name, string t_value, int tr_val, int end_l, const stringstream& _ss) : type_name(t_name), token_value(t_value), tr_value(tr_val), end_label(end_l) {
            ss.str("");
            ss << _ss.str();
        }
        token(const token& other) noexcept {
            type_name = other.type_name;
            token_value = other.token_value;
            tr_value = other.tr_value;
            end_label = other.end_label;
            ss << other.ss.str();
        }
        token& operator=(const token& other) {
            if (this == &other) {
                return *this;
            }
            type_name = other.type_name;
            token_value = other.token_value;
            tr_value = other.tr_value;
            end_label = other.end_label;
            ss << other.ss.str();
        }
        void print_ss(){
            cout << ss.str();
        }
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