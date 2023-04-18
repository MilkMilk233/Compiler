using namespace std;
#include "compiler.h"
#include <assert.h>

int main(int argc,char *argv[]){
    // Check the user has already fill in the file name
    if(argc < 2){
        cout<<"It seems that you forget to indicate the input file."<<endl;
        cout<<"Format (example): ./run_scanner.sh test0.c"<<endl;
        return 0;
    }

    // Use scanner to scan the input file
    Scanner goal(argv[1]);
    vector<unordered_map<string, int>> shift_map(431);
    vector<unordered_map<string, vector<string>>> reduce_map(431);
    // Initialize the shift operations in phasing table
    init_shift_map(shift_map);
    // Initialize the reduce ops in phasing table
    init_reduce_map(reduce_map);

    int current_state = 0;
    vector<int> path_num;
    vector<token> path_name;

    // Display all the tokens
    cout<<"Scanned Tokens:"<<endl;
    goal.display_tokens();
    cout<<"EOF"<<endl<<endl;
    cout <<"Parsing Process:" << endl;

    token next_type = goal.get_token();
    path_num.push_back(0);

    while(true){
        // Accepted
        if(shift_map[current_state].count(next_type.type_name) && shift_map[current_state][next_type.type_name] == -1){
            cout << "Accepted!"<<endl;
            break;
        }
        // There exists a shift operation in the phasing table
        else if(shift_map[current_state].count(next_type.type_name)){
            cout<<"state: "<<current_state;
            cout<<"	next type: "<<next_type.type_name;
            cout <<"		shift to state "<<shift_map[current_state][next_type.type_name]<<endl;
            path_name.push_back(next_type);
            path_num.push_back(shift_map[current_state][next_type.type_name]);
            current_state = shift_map[current_state][next_type.type_name];
            cout << "current situation:";
            for(int i = 0; i < path_name.size(); i++){
                cout<<" "<<path_name[i].type_name;
            }
            cout <<" |" << endl << endl;
            next_type = goal.get_token();
        }
        // There exists a reduce operation in the phasing table (Muted)
        else if(reduce_map[current_state].count(next_type.type_name)){
            //  Announcement #1
            cout<<"state: "<<current_state;
            cout<<"	next type: "<<next_type.type_name;
            cout <<"		reduce by grammar: "<<reduce_map[current_state][next_type.type_name][2]<<endl;
            for(int i = 0; i < atoi(reduce_map[current_state][next_type.type_name][0].c_str()); i++){
                path_name.pop_back();
                path_num.pop_back();
            }
            cout << "current situation:";
            for(int i = 0; i < path_name.size(); i++){
                cout<<" "<<path_name[i].type_name;
            }
            cout << " | " << reduce_map[current_state][next_type.type_name][1] << endl<< endl;

            //  Announcement #2
            int old_current_state = path_num.back();
            token old_next_type = {reduce_map[current_state][next_type.type_name][1], ""};
            cout << "state: "<< old_current_state << "	next type: "<< old_next_type.type_name;
            cout << "		shift to state "<<shift_map[old_current_state][old_next_type.type_name] << endl;
            path_name.push_back(old_next_type);
            path_num.push_back(shift_map[old_current_state][old_next_type.type_name]);
            current_state = shift_map[old_current_state][old_next_type.type_name];
            cout << "current situation:";
            for(int i = 0; i < path_name.size(); i++){
                cout<<" "<<path_name[i].type_name;
            }
            cout <<" |" << endl << endl;
        }
        // There exists neither shift ops or reduce ops in the phasing table
        else{
            cout << "declined!"<<endl;
            break;
        }

    }

    return 0;
}