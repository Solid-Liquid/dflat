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

    struct Members {
        String type;
        String name;
    };

    // ClassName -> [Members]
    Map<String, Vector<Members>> classMembers;

    std::stringstream& write(){
        if(!curClass) {
            return main;
        } else if(!curFunc) {
            return structDef;
        } else {
            return funcDef;
        }
    }
};

}

#endif // CODEGENERATOR_TOOLS_HPP
