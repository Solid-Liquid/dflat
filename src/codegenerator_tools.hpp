#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"
#include "map.hpp"
#include <sstream>
#include "optional.hpp"
#include "vector.hpp"

namespace dflat
{

struct GenEnv
{
    std::stringstream structDef;
    std::stringstream funcDef;
    std::stringstream main;

    Optional<String> curClass;
    Optional<String> curFunc;

    unsigned tabs = 0;

    std::stringstream& write(){
        if(!curClass) {
            return main;
        } else if(!curFunc) {
            return structDef;
        } else {
            return funcDef;
        }
    }

    std::stringstream& writeTabbed(){
        auto& stream = write();
        stream << String(tabs, '\t');
        return stream;
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
