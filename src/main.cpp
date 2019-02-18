#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <token.hpp>

using namespace std;
using namespace dflat;

int main(int argc, char* argv[])
{
    //TOKEN USE EXAMPLE
    Token* tk = new NumberToken(3); //store a number token in a base class token pointer

    //try to get as desired type
    if (auto tkNum = tk->as<NumberToken>())
    {
        tkNum->num = 5;
        cout << tkNum->num << endl;
    }
    //END TOKEN USE EXAMPLE

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
    cout << fileContents << endl;

    return 0;
}
