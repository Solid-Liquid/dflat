#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"
#include "map.hpp"

namespace dflat
{

struct GenEnv
{
    //TODO figure out what needs to be here
    int tempDeleteLater;

    String structDecl = "";
    String funcDecl = "";
    String main = "";
};

}

#endif // CODEGENERATOR_TOOLS_HPP
