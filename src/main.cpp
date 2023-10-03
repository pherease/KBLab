#include "include.h"

int main(){
    gErrorIgnoreLevel = 6001;
    while(true){
        string arg;
        string PATH;
        string folderPath = "/Users/efemyuksel/Desktop/KB_OBJ/data";
        cout << "------------------------------------------------------------------------------------|" << endl;
        cout << "Write out an individual fit-graph ->" << RED << " 'wg'" << RESET << endl  <<  "Read - analyze - write out a file ->" << RED << " 'af'" << RESET << endl <<  "Read - analyze - write out whole data folder ->" << RED << " 'ad'" << RESET << endl << "Type" << RED << " 'x'" << RESET <<  " to exit." << endl << YELLOW << "Enter the desired argument: " << RESET << endl;
        cin >> arg;
        
        if(arg == "wg"){
            int i;
            cout << "Enter the PATH of the data file: " << endl;
            cin >> PATH;
            
            cout << "Enter the index of the desired fit: " << endl;
            cin >> i;
            WriteOutFitGraph(PATH, i);
            cout << GREEN << "Done." << RESET << endl;
        }

        else if (arg == "af")
        {
            cout << "Enter the PATH of the data file: " << endl;
            cin >> PATH;
            ReadAnalyzeFile(PATH);
            cout << GREEN << "Done." << RESET << endl;
        }
        
        else if (arg == "ad"){
            ReadAnalyzeFolder(folderPath);
            cout << GREEN << "Done." << RESET << endl;
        }

        else if (arg == "x"){
            break;
        }
    }
    return 0;
}