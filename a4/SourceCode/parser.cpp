using namespace std;
#include "compiler.h"
#include <assert.h>

#define DEBUG_MODE false

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
    // Initialize the mapping between the saved registers and the name of the variables
    int sr_count = 0;
    unordered_map<string, int> sr_map;
    // Initialize the occupation status of the temp registers
    unordered_set<int> tr_map;
    // Initialize the memory occupation map
    unordered_map<string, int> mem_map;
    unordered_map<int, string> mem_map_rev;

    int current_state = 0;
    vector<int> path_num;
    vector<token> path_name;
    int label_count = 0;

    token next_type = goal.get_token();
    path_num.push_back(0);

    // Allocate a temp register + allocate memory (optional)
    auto allocate_tr = [&tr_map, &mem_map, &mem_map_rev]() -> int {
        int index = 0;
        int result;
        // Find the first unused tr index
        while(true){
            if(!tr_map.count(index)){
                tr_map.insert(index);
                result = index;
                break;
            }
            ++index;
        }
        // Allocation a chunk of memory
        if(index > 7){
            string tr_name = "$t"+to_string(index);
            int space_index = 0;
            while(true){
                if(!mem_map_rev.count(space_index)){
                    mem_map_rev[space_index] = tr_name;
                    mem_map[tr_name] = space_index;
                    break;
                }
                ++space_index;
            }
        }
        return index;
    };

    // Allocate memory for saved reg (optional)
    auto allocate_sr = [&sr_map, &sr_count, &mem_map, &mem_map_rev]() -> int {
        // Allocation a chunk of memory
        if(sr_count > 7){
            string sr_name = "$s"+to_string(sr_count);
            int space_index = 0;
            while(true){
                if(!mem_map_rev.count(space_index)){
                    mem_map_rev[space_index] = sr_name;
                    mem_map[sr_name] = space_index;
                    break;
                }
                ++space_index;
            }
        }
        return sr_count++;
    };

    // Discard the indicated temp register with index = n
    auto discard_tr = [&tr_map, &mem_map, &mem_map_rev](int n) {
        if(tr_map.count(n)){
            tr_map.erase(n);
        }
        if(n > 7){
            string tr_name = "$t"+ to_string(n);
            int tr_location = mem_map[tr_name];
            mem_map.erase(tr_name);
            mem_map_rev.erase(tr_location);
        }
    };

    auto tr_save = [&mem_map](int id){
        string name = "$t"+to_string(id);
        int mem_addr = mem_map[name]*4;
        cout << "sw $t8, "<<mem_addr<<"($sp)"<<endl;
    };

    auto sr_save = [&mem_map](int id){
        string name = "$s"+to_string(id);
        int mem_addr = mem_map[name]*4;
        cout << "sw $t8, "<<mem_addr<<"($sp)"<<endl;
    };

    auto tr_load = [&mem_map](int id){
        string name = "$t"+to_string(id);
        int mem_addr = mem_map[name]*4;
        cout << "lw $t9, "<<mem_addr<<"($sp)"<<endl;
    };

    auto sr_load = [&mem_map](int id){
        string name = "$s"+to_string(id);
        int mem_addr = mem_map[name]*4;
        cout << "lw $t9, "<<mem_addr<<"($sp)"<<endl;
    };

    // Parsing & translation process
    while(true){
        if(DEBUG_MODE) cout << "Next type = "<<next_type.type_name<<endl;
        // Accepted
        if(shift_map[current_state].count(next_type.type_name) && shift_map[current_state][next_type.type_name] == -1){
            cout << "Accepted!"<<endl;
            break;
        }
        // There exists a shift operation in the phasing table
        else if(shift_map[current_state].count(next_type.type_name)){
            if(DEBUG_MODE){
                cout<<"state: "<<current_state;
                cout<<"	next type: "<<next_type.type_name;
                cout <<"		shift to state "<<shift_map[current_state][next_type.type_name]<<endl;
            }

            path_name.push_back(next_type);
            path_num.push_back(shift_map[current_state][next_type.type_name]);
            current_state = shift_map[current_state][next_type.type_name];

            if(DEBUG_MODE){
                cout << "current situation:";
                for(int i = 0; i < path_name.size(); i++){
                    cout<<" "<<path_name[i].type_name;
                }
                cout <<" |" << endl << endl;
            }

            next_type = goal.get_token();
        }
        // There exists a reduce operation in the phasing table (Muted)
        else if(reduce_map[current_state].count(next_type.type_name)){
            int output_tr = -1;
            int name_tail = path_name.size()-1;
            int num_tail = path_num.size()-1;
            vector<string> *ptr = &reduce_map[current_state][next_type.type_name];

            // Discuss the reduction case by case
            if(ptr->at(2) == "exp10>ID"){
                int sr_id = sr_map[path_name[name_tail].token_value];
                string sr_name = "$s"+to_string(sr_id);
                int mem_addr;
                if(sr_id > 7){
                    sr_load(sr_id);
                    sr_name = "$t9";
                }
                int tr_id = allocate_tr();
                if(tr_id > 7){
                    cout << "move $t8, "<<sr_name<<endl;
                    tr_save(tr_id);
                }
                else{
                    cout << "move $t" << tr_id <<", "<<sr_name<<endl;
                }
                output_tr = tr_id;
            }
            else if(ptr->at(2) == "exp10>ID LSQUARE INT_NUM RSQUARE"){
                int array_location = stoi(path_name[name_tail-1].token_value);
                string t_name = path_name[name_tail-3].token_value + "[" + to_string(array_location) + "]";
                int sr_id = sr_map[t_name];
                string sr_name = "$s"+to_string(sr_id);
                int mem_addr;
                if(sr_id > 7){
                    sr_load(sr_id);
                    sr_name = "$t9";
                }
                int tr_id = allocate_tr();
                if(tr_id > 7){
                    cout << "move $t8, "<<sr_name<<endl;
                    tr_save(tr_id);
                }
                else{
                    cout << "move $t" << tr_id <<", "<<sr_name<<endl;
                }
                output_tr = tr_id;
            }
            else if(ptr->at(2) == "declaration>ID LSQUARE INT_NUM RSQUARE"){
                int array_len = stoi(path_name[name_tail-1].token_value);
                string sr_name;
                for(int i = 0; i < array_len; i++){
                    sr_name = path_name[name_tail-3].token_value + "[" + to_string(i) + "]";
                    sr_map[sr_name] = sr_count;
                    allocate_sr();
                }
            }
            else if(ptr->at(2) == "exp10>INT_NUM"){
                int tr_id = allocate_tr();
                if(tr_id > 7){
                    cout << "li $t8, "<<path_name[name_tail].token_value<<endl;
                    tr_save(tr_id);
                }
                else{
                    cout << "li $t" << tr_id <<", "<<path_name[name_tail].token_value<<endl;
                }
                output_tr = tr_id;
            }
            else if(ptr->at(2) == "declaration>ID"){
                string id_name = path_name[name_tail].token_value;
                int sr_id;
                if(sr_map.count(id_name)){
                    sr_id = sr_map[id_name];
                }
                else{
                    sr_map[id_name] = sr_count;
                    sr_id = allocate_sr();
                }
            }
            else if(ptr->at(2) == "exp9>exp10"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp8>exp9"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp7>exp8"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp6>exp7"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp5>exp6"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp4>exp5"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp3>exp4"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp2>exp3"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp1>exp2"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp>exp1"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp10>LPAR exp RPAR"){
                output_tr = path_name[name_tail-1].tr_value;
            }
            else if(ptr->at(2) == "exp1>exp1 OROR exp2"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    tr_name1 = "$t9";
                }
                cout << "bne "<<tr_name1<<", $0, $L"<< label_count << endl;
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "bne "<<tr_name2<<"$0, $L"<< label_count << endl;
                cout << "move "<<tr_name1<<", $0"<<endl;
                cout << "b $L"<<label_count+1<<endl;
                cout << "$L"<<label_count++<<":"<<endl;
                cout << "li "<<tr_name1<<", 1"<<endl;
                cout << "$L"<<label_count++<<":"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp1>exp1 ANDAND exp2"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    tr_name1 = "$t9";
                }
                cout << "beq "<<tr_name1<<", $0, $L"<< label_count << endl;
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "beq "<<tr_name2<<", $0, $L"<< label_count << endl;
                cout << "li "<<tr_name1<<", 1"<<endl;
                cout << "b $L"<<label_count+1<<endl;
                cout << "$L"<<label_count++<<":"<<endl;
                cout << "move "<<tr_name1<<", $0"<<endl;
                cout << "$L"<<label_count++<<":"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp2>exp2 OR_OP exp3"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "or "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp3>exp3 AND_OP exp4"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "and "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp4>exp4 NOTEQ exp5"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "xor "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                cout << "sltu "<<tr_name1<<", $0, "<<tr_name1<<endl;
                cout << "andi "<<tr_name1<<", "<<tr_name1<<", 0x00ff"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp4>exp4 EQ exp5"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "xor "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                cout << "sltu "<<tr_name1<<", "<<tr_name1<<", 1"<<endl;
                cout << "andi "<<tr_name1<<", "<<tr_name1<<", 0x00ff"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp5>exp5 LT exp6"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "slt "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                cout << "andi "<<tr_name1<<", "<<tr_name1<<", 0x00ff"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp5>exp5 GT exp6"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "slt "<<tr_name1<<", "<<tr_name2<<", "<<tr_name1<<endl;
                cout << "andi "<<tr_name1<<", "<<tr_name1<<", 0x00ff"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp5>exp5 LTEQ exp6"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "slt "<<tr_name1<<", "<<tr_name2<<", "<<tr_name1<<endl;
                cout << "xori "<<tr_name1<<", "<<tr_name1<<", 0x1"<<endl;
                cout << "andi "<<tr_name1<<", "<<tr_name1<<", 0x00ff"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp5>exp5 GTEQ exp6"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "slt "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                cout << "xori "<<tr_name1<<", "<<tr_name1<<", 0x1"<<endl;
                cout << "andi "<<tr_name1<<", "<<tr_name1<<", 0x00ff"<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp6>exp6 SHL_OP exp7"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "sll "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp6>exp6 SHR_OP exp7"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "sra "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp7>exp7 MINUS exp8"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "subu "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp7>exp7 PLUS exp8"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "addu "<<tr_name1<<", "<<tr_name1<<", "<<tr_name2<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp8>exp8 MUL_OP exp9"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "mult "<<tr_name1<<", "<<tr_name2<<endl;
                cout << "mflo "<<tr_name1<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp8>exp8 DIV_OP exp9"){
                int tr_id1 = path_name[name_tail-2].tr_value;
                string tr_name1 = "$t"+to_string(tr_id1);
                if(tr_id1 > 7){
                    tr_load(tr_id1);
                    cout << "move $t8, $t9"<<endl;
                    tr_name1 = "$t8";
                }
                int tr_id2 = path_name[name_tail].tr_value;
                string tr_name2 = "$t"+to_string(tr_id2);
                if(tr_id2 > 7){
                    tr_load(tr_id2);
                    tr_name2 = "$t9";
                }
                cout << "bne "<<tr_name2<<", "<<"$0"<<", 1f"<<endl;
                cout << "div $0, "<<tr_name1<<", "<<tr_name2<<endl;
                cout << "break 7"<<endl;
                cout << "mfhi "<<tr_name1<<endl;
                cout << "mflo "<<tr_name1<<endl;
                if(tr_id1 > 7){
                    tr_save(tr_id1);
                }
                discard_tr(tr_id2);
                output_tr = tr_id1;
            }
            else if(ptr->at(2) == "exp9>NOT_OP exp9"){
                int tr_id = path_name[name_tail].tr_value;
                string tr_name = "$t"+to_string(tr_id);
                if(tr_id > 7){
                    tr_load(tr_id);
                    tr_name = "$t9";
                }
                cout << "sltu "<<tr_name<<", "<<tr_name<<", 1"<<endl;
                cout << "andi "<<tr_name<<", "<<tr_name<<", 0x00ff"<<endl;
                if(tr_id > 7){
                    cout << "move $t8, $t9"<<endl;
                    tr_save(tr_id);
                }
                output_tr = tr_id;
            }
            else if(ptr->at(2) == "exp9>PLUS exp9"){
                output_tr = path_name[name_tail].tr_value;
            }
            else if(ptr->at(2) == "exp9>MINUS exp9"){
                int tr_id = path_name[name_tail].tr_value;
                string tr_name = "$t"+to_string(tr_id);
                if(tr_id > 7){
                    tr_load(tr_id);
                    tr_name = "$t9";
                }
                cout << "subu "<<tr_name<<", $0, "<<tr_name<<endl;
                if(tr_id > 7){
                    cout << "move $t8, $t9"<<endl;
                    tr_save(tr_id);
                }
                output_tr = tr_id;
            }
            else if(ptr->at(2) == "declaration>ID ASSIGN exp"){
                string id_name = path_name[name_tail-2].token_value;
                int sr_id;
                if(sr_map.count(id_name)){
                    sr_id = sr_map[id_name];
                }
                else{
                    sr_map[id_name] = sr_count;
                    sr_id = allocate_sr();
                }
                int tr_id = path_name[name_tail].tr_value;
                string tr_name = "$t" + to_string(tr_id);
                if(tr_id > 7){
                    tr_name = "$t9";
                    tr_load(tr_id);
                }
                discard_tr(tr_id);
                if(sr_id > 7){
                    cout << "move $t8, "<<tr_name<<endl;
                    sr_load(sr_id);
                }
                else{
                    cout << "move $s"<<sr_id<<", "<<tr_name<<endl;
                }
            }
            else if(ptr->at(2) == "assign_statement>ID ASSIGN exp"){
                string id_name = path_name[name_tail-2].token_value;
                int sr_id = sr_map[id_name];
                int tr_id = path_name[name_tail].tr_value;
                string tr_name = "$t" + to_string(tr_id);
                if(tr_id > 7){
                    tr_name = "$t9";
                    tr_load(tr_id);
                }
                discard_tr(tr_id);
                if(sr_id > 7){
                    cout << "move $t8, "<<tr_name<<endl;
                    sr_load(sr_id);
                }
                else{
                    cout << "move $s"<<sr_id<<", "<<tr_name<<endl;
                }
            }
            else if(ptr->at(2) == "assign_statement>ID LSQUARE INT_NUM RSQUARE ASSIGN exp"){
                string id_name = path_name[name_tail-5].token_value + "[" + path_name[name_tail-3].token_value + "]";
                int sr_id = sr_map[id_name];
                int tr_id = path_name[name_tail].tr_value;
                string tr_name = "$t" + to_string(tr_id);
                if(tr_id > 7){
                    tr_name = "$t9";
                    tr_load(tr_id);
                }
                discard_tr(tr_id);
                if(sr_id > 7){
                    cout << "move $t8, "<<tr_name<<endl;
                    sr_load(sr_id);
                }
                else{
                    cout << "move $s"<<sr_id<<", "<<tr_name<<endl;
                }
            }
            // Discussion ends, pop out the old tokens

            for(int i = 0; i < atoi(reduce_map[current_state][next_type.type_name][0].c_str()); i++){
                path_name.pop_back();
                path_num.pop_back();
            }
            if(DEBUG_MODE){
                cout << "current situation:";
                for(int i = 0; i < path_name.size(); i++){
                    cout<<" "<<path_name[i].type_name;
                }
                cout << " | " << reduce_map[current_state][next_type.type_name][1] << endl<< endl;
            }

            //  Announcement #2
            int old_current_state = path_num.back();
            token old_next_type = {reduce_map[current_state][next_type.type_name][1], "", output_tr};
            path_name.push_back(old_next_type);
            path_num.push_back(shift_map[old_current_state][old_next_type.type_name]);
            current_state = shift_map[old_current_state][old_next_type.type_name];

            if(DEBUG_MODE){
                cout << "current situation:";
                for(int i = 0; i < path_name.size(); i++){
                    cout<<" "<<path_name[i].type_name;
                }
                cout <<" |" << endl << endl;
            }
        }
        // There exists neither shift ops or reduce ops in the phasing table
        else{
            cout << "declined!"<<endl;
            break;
        }

    }

    return 0;
}