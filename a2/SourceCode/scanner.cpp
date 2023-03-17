#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;
#define file_path "./Test_cases/"

class Scanner{
    private:
        vector<unordered_map<char, int>> dfa_states_vector;
        unordered_map<int, string> final_state_map;
        ifstream c_file;
        int current_state;
    public:
        void init_digits(unordered_map<char, int> &umap, int target){
            for(int i = 0; i < 10; i++){
                umap[char('0'+i)] = target;
            }
        }
        void init_letters(unordered_map<char, int> &umap, int target){
            for(int i = 0; i < 26; i++){
                umap[char('a'+i)] = target;
                umap[char('A'+i)] = target;
            }
        }
        void init_l_and_d(unordered_map<char, int> &umap, int target){
            init_digits(umap, target);
            init_letters(umap, target);
            umap['_'] = target;
        }

        // Initialization
        Scanner(string file_name){
            
            // Open the file
            c_file.open(file_path+file_name);
            if(!c_file.is_open()){
                // throw "Invalid filename!";
                cout<<"Invalid filename!" <<endl;
                cout<<"Please make sure the test file is under the "<<file_path<<"."<<endl;
                throw;
            }

            // Define the state transfer condition
            dfa_states_vector.resize(100);
            init_letters(dfa_states_vector[1], 30);
            dfa_states_vector[1]['{'] = 2; 
            dfa_states_vector[1]['}'] = 3;
            dfa_states_vector[1]['['] = 4;
            dfa_states_vector[1][']'] = 5;
            dfa_states_vector[1]['('] = 6;
            dfa_states_vector[1][')'] = 7;
            dfa_states_vector[1][';'] = 8;
            dfa_states_vector[1]['+'] = 9;
            dfa_states_vector[1]['-'] = 10;
            dfa_states_vector[1]['*'] = 11;
            dfa_states_vector[1]['/'] = 12;
            dfa_states_vector[1]['&'] = 13;
            dfa_states_vector[1]['|'] = 15;
            dfa_states_vector[1]['!'] = 17;
            dfa_states_vector[1]['='] = 19;
            dfa_states_vector[1]['<'] = 21;
            dfa_states_vector[1]['>'] = 24;
            dfa_states_vector[1][','] = 27;
            dfa_states_vector[13]['&'] = 14;
            dfa_states_vector[15]['|'] = 16;
            dfa_states_vector[17]['='] = 18;
            dfa_states_vector[19]['='] = 20;
            dfa_states_vector[21]['<'] = 22;
            dfa_states_vector[21]['='] = 23;
            dfa_states_vector[24]['>'] = 25;
            dfa_states_vector[24]['='] = 26;
            dfa_states_vector[1][','] = 27;
            init_digits(dfa_states_vector[1], 28);
            init_digits(dfa_states_vector[28], 28);
            dfa_states_vector[1]['\n'] = 29;
            dfa_states_vector[1][' '] = 29;
            dfa_states_vector[1]['\0'] = 29;
            dfa_states_vector[1]['\t'] = 29;
            dfa_states_vector[1]['\r'] = 29;
            dfa_states_vector[29]['\n'] = 29;
            dfa_states_vector[29]['\0'] = 29;
            dfa_states_vector[29]['\t'] = 29;
            dfa_states_vector[29]['\r'] = 29;
            dfa_states_vector[1]['i'] = 31; 
            dfa_states_vector[1]['m'] = 35; 
            dfa_states_vector[1]['v'] = 39;
            dfa_states_vector[1]['e'] = 43;
            dfa_states_vector[1]['d'] = 47;
            dfa_states_vector[1]['b'] = 49;
            dfa_states_vector[1]['w'] = 54;
            dfa_states_vector[1]['s'] = 59;
            dfa_states_vector[1]['p'] = 64;
            dfa_states_vector[1]['r'] = 70;        
            init_l_and_d(dfa_states_vector[30], 30);
            init_l_and_d(dfa_states_vector[31], 30);
            init_l_and_d(dfa_states_vector[32], 30);
            init_l_and_d(dfa_states_vector[33], 30);
            init_l_and_d(dfa_states_vector[34], 30);
            init_l_and_d(dfa_states_vector[35], 30);
            init_l_and_d(dfa_states_vector[36], 30);
            init_l_and_d(dfa_states_vector[37], 30);
            init_l_and_d(dfa_states_vector[38], 30);
            init_l_and_d(dfa_states_vector[39], 30);
            init_l_and_d(dfa_states_vector[40], 30);
            init_l_and_d(dfa_states_vector[41], 30);
            init_l_and_d(dfa_states_vector[42], 30);
            init_l_and_d(dfa_states_vector[43], 30);
            init_l_and_d(dfa_states_vector[44], 30);
            init_l_and_d(dfa_states_vector[45], 30);
            init_l_and_d(dfa_states_vector[46], 30);
            init_l_and_d(dfa_states_vector[47], 30);
            init_l_and_d(dfa_states_vector[48], 30);
            init_l_and_d(dfa_states_vector[49], 30);
            init_l_and_d(dfa_states_vector[50], 30);
            init_l_and_d(dfa_states_vector[51], 30);
            init_l_and_d(dfa_states_vector[52], 30);
            init_l_and_d(dfa_states_vector[53], 30);
            init_l_and_d(dfa_states_vector[54], 30);
            init_l_and_d(dfa_states_vector[55], 30);
            init_l_and_d(dfa_states_vector[56], 30);
            init_l_and_d(dfa_states_vector[57], 30);
            init_l_and_d(dfa_states_vector[58], 30);
            init_l_and_d(dfa_states_vector[59], 30);
            init_l_and_d(dfa_states_vector[60], 30);
            init_l_and_d(dfa_states_vector[61], 30);
            init_l_and_d(dfa_states_vector[62], 30);
            init_l_and_d(dfa_states_vector[63], 30);
            init_l_and_d(dfa_states_vector[64], 30);
            init_l_and_d(dfa_states_vector[65], 30);
            init_l_and_d(dfa_states_vector[66], 30);
            init_l_and_d(dfa_states_vector[67], 30);
            init_l_and_d(dfa_states_vector[68], 30);
            init_l_and_d(dfa_states_vector[69], 30);
            init_l_and_d(dfa_states_vector[70], 30);
            init_l_and_d(dfa_states_vector[71], 30);
            init_l_and_d(dfa_states_vector[72], 30);
            init_l_and_d(dfa_states_vector[73], 30);
            init_l_and_d(dfa_states_vector[74], 30);
            init_l_and_d(dfa_states_vector[75], 30);
            dfa_states_vector[31]['n'] = 32;  
            dfa_states_vector[31]['f'] = 34;
            dfa_states_vector[32]['t'] = 33;
            dfa_states_vector[35]['a'] = 36; 
            dfa_states_vector[36]['i'] = 37; 
            dfa_states_vector[37]['n'] = 38;
            dfa_states_vector[39]['o'] = 40;
            dfa_states_vector[40]['i'] = 41;
            dfa_states_vector[41]['d'] = 42;
            dfa_states_vector[43]['l'] = 44;
            dfa_states_vector[44]['s'] = 45;
            dfa_states_vector[45]['e'] = 46;
            dfa_states_vector[47]['o'] = 48;
            dfa_states_vector[49]['r'] = 50;
            dfa_states_vector[50]['e'] = 51;
            dfa_states_vector[51]['a'] = 52;
            dfa_states_vector[52]['k'] = 53;
            dfa_states_vector[54]['h'] = 55;
            dfa_states_vector[55]['i'] = 56;
            dfa_states_vector[56]['l'] = 57;
            dfa_states_vector[57]['e'] = 58;
            dfa_states_vector[59]['c'] = 60;
            dfa_states_vector[60]['a'] = 61;
            dfa_states_vector[61]['n'] = 62;
            dfa_states_vector[62]['f'] = 63;
            dfa_states_vector[64]['r'] = 65;
            dfa_states_vector[65]['i'] = 66;
            dfa_states_vector[66]['n'] = 67;
            dfa_states_vector[67]['t'] = 68;
            dfa_states_vector[68]['f'] = 69;
            dfa_states_vector[70]['e'] = 71;
            dfa_states_vector[71]['t'] = 72;
            dfa_states_vector[72]['u'] = 73;
            dfa_states_vector[73]['r'] = 74;
            dfa_states_vector[74]['n'] = 75;                                                                  

            // Define the final state condition
            final_state_map[2] = "LBRACE";
            final_state_map[3] = "RBRACE";
            final_state_map[4] = "LSQUARE";
            final_state_map[5] = "RSQUARE";
            final_state_map[6] = "LPAR";
            final_state_map[7] = "RPAR";
            final_state_map[8] = "SEMI";
            final_state_map[9] = "PLUS";
            final_state_map[10] = "MINUS";
            final_state_map[11] = "MUL_OP";
            final_state_map[12] = "DIV_OP";
            final_state_map[13] = "AND_OP";
            final_state_map[14] = "ANDAND";
            final_state_map[15] = "OR_OP";
            final_state_map[16] = "OROR";
            final_state_map[17] = "NOT_OP";
            final_state_map[18] = "NOTEQ";
            final_state_map[19] = "ASSIGN";
            final_state_map[20] = "EQ";
            final_state_map[21] = "LT";
            final_state_map[22] = "SHL_OP";
            final_state_map[23] = "LTEQ";
            final_state_map[24] = "GT";
            final_state_map[25] = "SHR_OP";
            final_state_map[26] = "GTEQ";
            final_state_map[27] = "COMMA";
            final_state_map[28] = "INT_NUM";
            final_state_map[29] = "BLANK";   // Blank
            final_state_map[30] = "ID";
            final_state_map[31] = "ID";
            final_state_map[32] = "ID";
            final_state_map[33] = "INT";
            final_state_map[34] = "IF";
            final_state_map[35] = "ID";
            final_state_map[36] = "ID";
            final_state_map[37] = "ID";
            final_state_map[38] = "MAIN";
            final_state_map[39] = "ID";
            final_state_map[40] = "ID";
            final_state_map[41] = "ID";
            final_state_map[42] = "VOID";
            final_state_map[43] = "ID";
            final_state_map[44] = "ID";
            final_state_map[45] = "ID";
            final_state_map[46] = "ELSE";
            final_state_map[47] = "ID";
            final_state_map[48] = "DO";
            final_state_map[49] = "ID";
            final_state_map[50] = "ID";
            final_state_map[51] = "ID";
            final_state_map[52] = "ID";
            final_state_map[53] = "BREAK";
            final_state_map[54] = "ID";
            final_state_map[55] = "ID";
            final_state_map[56] = "ID";
            final_state_map[57] = "ID";
            final_state_map[58] = "WHILE";
            final_state_map[59] = "ID";
            final_state_map[60] = "ID";
            final_state_map[61] = "ID";
            final_state_map[62] = "ID";
            final_state_map[63] = "READ";
            final_state_map[64] = "ID";
            final_state_map[65] = "ID";
            final_state_map[66] = "ID";
            final_state_map[67] = "ID";
            final_state_map[68] = "ID";
            final_state_map[69] = "WRITE";
            final_state_map[70] = "ID";
            final_state_map[71] = "ID";
            final_state_map[72] = "ID";
            final_state_map[73] = "ID";
            final_state_map[74] = "ID";
            final_state_map[75] = "RETURN";
        }

        void scan(){
            try{
                char next_char;
                current_state = 1;
                while(c_file.get(next_char)){
                    if(dfa_states_vector[current_state].find(next_char) != dfa_states_vector[current_state].end()){
                        current_state = dfa_states_vector[current_state][next_char];
                    }
                    else{
                        if(final_state_map.find(current_state) != final_state_map.end()){
                            if(current_state != 29){
                                cout<<"Token: "<<final_state_map[current_state]<<endl;
                            }
                            if(dfa_states_vector[1].find(next_char) != dfa_states_vector[1].end()){
                                current_state = dfa_states_vector[1][next_char];
                            }
                            else{
                                cout<<"current_state = "<<current_state<<endl;
                                throw next_char;
                            }
                        }
                        else{
                            cout<<"current_state = "<<current_state<<endl;
                            throw next_char;
                        }
                    }
                }
                if(final_state_map.find(current_state) != final_state_map.end()){
                    if(current_state != 29){
                        cout<<"Token: "<<final_state_map[current_state]<<endl;
                    }
                }
                else{
                    cout<<"current_state = "<<current_state<<endl;
                    throw EOF;
                }
            }
            catch(const char msg){
                cout<<"Unable to parse the file, last character = "<<msg<<endl;
            }
        }

        ~Scanner(){
            if(c_file.is_open()){
                c_file.close();
            }
        }
};

int main(int argc,char *argv[]){
    if(argc < 2){
        cout<<"It seems that you forget to indicate the input file."<<endl;
        cout<<"Format (example): ./run_scanner.sh test0.c"<<endl;
        return 0;
    }

    Scanner goal(argv[1]);
    goal.scan();
    
    return 0;
}