#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "lexer.hpp"
#include "parser.hpp"
#include "typechecker.hpp"
#include "config.hpp"

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

    try
    {
        //Take file contents and run compiler:

        //Run Lexer:
        Vector<TokenPtr> tokens = tokenize(fileContents);
//        for (TokenPtr const& token : tokens)
//        {
//            cout << token->toString() << endl;
//        }

        //Run Parser:
        Vector<ASNPtr> program = parse(tokens);

//        for (ASNPtr const& decl : program)
//        {
//            cout << decl->toString() << endl << endl;
//        }

        //Run TypeChecker:
        config::traceTypeCheck = true;
        TypeEnv environment = typeCheck(program);
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
