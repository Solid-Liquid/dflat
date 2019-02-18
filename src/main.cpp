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
    Token* tk = new NumberToken(); //store a number token in a base class token pointer
    if(tk->getType() == tokNum) //check what type the token is (compare to tokNum enumeration for token number)
    {
        dynamic_cast<NumberToken*>(tk)->num = 5;  //use dynamic cast to change the NumberToken stored number
        cout << dynamic_cast<NumberToken*>(tk)->num << endl; //use dynamic cast to print the stored number
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
