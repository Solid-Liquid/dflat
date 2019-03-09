#ifndef TYPECHECKER_TOOLS_HPP
#define TYPECHECKER_TOOLS_HPP

#include "string.hpp"
#include "map.hpp"
#include "set.hpp"

namespace dflat
{

//Struct used for type checking:
struct TypeEnv
{
    Set<String> types;              //set of known types and classes
    Map<String,String> variables;   //map for variables
};


//Convenient aliases for known types:
using Type = String;
Type const intType = "int";
Type const boolType = "bool";
Type const voidType = "void";

} //namespace dflat

#endif // TYPECHECKER_TOOLS_HPP
