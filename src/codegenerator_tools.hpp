#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"

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

String genEverything(GenEnv env)
{
    return env.structDecl + env.funcDecl + env.main;
}

}

#endif // CODEGENERATOR_TOOLS_HPP
