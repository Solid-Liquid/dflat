#ifndef TYPECHECKER_TOOLS_HPP
#define TYPECHECKER_TOOLS_HPP

#include "type.hpp"
#include "string.hpp"
#include "map.hpp"
#include "set.hpp"
#include "optional.hpp"
#include "vector.hpp"

namespace dflat
{

//Macro that makes a dynamic cast look like less of a mess:
#define cast(ptr,type) dynamic_cast<type*>(ptr.get())

ValueType const intType("int");
ValueType const boolType("bool");
ValueType const voidType("void");

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
    //currentMethod - the canonical name of the method that is currently being typechecked.

    Set<ValueType> types;
    Map<String,Type> rules;
    Map<ValueType, Map<String, Type>> variables;
    Optional<ValueType> currentClass;
    Optional<String> currentMethod;
};


} //namespace dflat

#endif // TYPECHECKER_TOOLS_HPP
