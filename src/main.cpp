#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
    std::string fileName;

    if(argc == 2)
    {
        //read in a file name from command line:
        fileName = argv[1];
    }
    else
    {
        //prompt user for filename in terminal:
        std::cout << "Enter the name of a file: ";
        std::getline(std::cin,fileName);
    }

    std::ifstream file(fileName);
    std::stringstream buffer;
    std::string fileContents;

    if(file.is_open())
    {
        buffer << file.rdbuf();
        fileContents = buffer.str();
    }
    else
    {
        std::cout << "Error: File not found!\n";
        return 0;
    }

    ///DO STUFF WITH FILE CONTENTS
    std::cout << fileContents << std::endl;

    return 0;
}
