#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"
#include "map.hpp"
#include <sstream>
#include "optional.hpp"

namespace dflat
{

struct GenEnv
{
    //TODO figure out what needs to be here
    int tempDeleteLater;

    std::stringstream structDef;
    std::stringstream funcDef;
    std::stringstream main;

    Optional<String> curClass;
    Optional<String> curFunc;

    std::stringstream& write(){
        if(!curClass) {
            return main;
        } else if(!curFunc) {
            return structDef;
        } else {
            return funcDef;
        }
    }

    String concat()
    {
        return structDef.str() 
             + "\n"
             + funcDef.str() 
             + "\n"
             + main.str();
    }
};

}

#endif // CODEGENERATOR_TOOLS_HPP
