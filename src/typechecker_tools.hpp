#ifndef TYPECHECKER_TOOLS_HPP
#define TYPECHECKER_TOOLS_HPP

#include "string.hpp"
#include "map.hpp"
#include "set.hpp"
#include "optional.hpp"
#include "vector.hpp"

namespace dflat
{

//Convenient aliases for known types:
using Type = String;
Type const intType = "int";
Type const boolType = "bool";
Type const voidType = "void";

struct TypeEnv
{
    //Struct used for type checking:
    //types - Set of valid type names (including classes).
    //rules - Map of valid rules for how types interact with operators:
    //  Map: String cannonical name -> String expressions type.
    //variables - Map of Maps for classes and their relevant variables:
    //  Map: String class name ->
    //      Map: String variable/function name -> Vector of types
    //          0 is var type or return type, 1+ are types for method args.
    //currentClass - the name of the class that is currently being typechecked.

    Set<String> types;
    Map<String,String> rules;
    Map<String,Map<String,Vector<String>>> variables;
    Optional<String> currentClass;
    Optional<String> currentMethod;
};


} //namespace dflat

#endif // TYPECHECKER_TOOLS_HPP
