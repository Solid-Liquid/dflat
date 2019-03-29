#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"

namespace dflat
{

struct GenEnv
{
    //TODO figure out what needs to be here
    int tempDeleteLater;

    String funcDecl = "";
    String structDecl = "";
    String main = "";
};

}

#endif // CODEGENERATOR_TOOLS_HPP
