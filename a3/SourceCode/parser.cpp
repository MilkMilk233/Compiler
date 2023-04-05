using namespace std;
#include "compiler.h"

int main(int argc,char *argv[]){
    // Check the user has already fill in the file name
    if(argc < 2){
        cout<<"It seems that you forget to indicate the input file."<<endl;
        cout<<"Format (example): ./run_scanner.sh test0.c"<<endl;
        return 0;
    }

    // Use scanner to scan the input file
    Scanner goal(argv[1]);
    vector<unordered_map<string, int>> shift_map;
    vector<unordered_map<string, pair<int, string>> reduce_map;
    // Initialize the shift map
    // init_shift_map(shift_map);

    


    return 0;
}