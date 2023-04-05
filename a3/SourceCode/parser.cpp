using namespace std;
#include "compiler.h"

int main(int argc,char *argv[]){
    // Check the user has already fill in the file name
    if(argc < 2){
        cout<<"It seems that you forget to indicate the input file."<<endl;
        cout<<"Format (example): ./run_scanner.sh test0.c"<<endl;
        return 0;
    }

    // Create an object
    Scanner goal(argv[1]);
    string www;
    while((www = goal.get_token()) != "EOF"){
        cout << www << " ";
    }
    cout << goal.get_token() << " ";
    cout << goal.get_token() << " ";
    cout << goal.get_token() << " ";
    return 0;
}