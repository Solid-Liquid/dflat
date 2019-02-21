#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "lexer.hpp"

using namespace std;
using namespace dflat;

int main(int argc, char* argv[])
{
    string fileName;

    if(argc == 2)
    {
        //read in a file name from command line:
        fileName = argv[1];
    }
    else
    {
        //prompt user for filename in terminal:
        cout << "Enter the name of a file: ";
        getline(cin,fileName);
    }

    ifstream file(fileName);
    stringstream buffer;
    string fileContents;

    if(file.is_open())
    {
        buffer << file.rdbuf();
        fileContents = buffer.str();
    }
    else
    {
        cout << "Error: File not found!\n";
        return 0;
    }

    ///DO STUFF WITH FILE CONTENTS
    Vector<TokenPtr> tokens = tokenize(fileContents); //run lexer
    for(unsigned long long i=0; i<tokens.size(); ++i)
    {
        cout << tokens[i]->toString() << endl;
    }

    return 0;
}
