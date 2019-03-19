#ifndef TYPECHECKER_TOOLS_HPP
#define TYPECHECKER_TOOLS_HPP

#include "string.hpp"
#include "map.hpp"
#include "set.hpp"
#include "optional.hpp"
#include "vector.hpp"

namespace dflat
{

//Macro that makes a dynamic cast look like less of a mess:
#define cast(ptr,type) dynamic_cast<type*>(ptr.get())

//Convenient aliases for known types:
using Type = String;
Type const intType = "int";
Type const boolType = "bool";
Type const voidType = "void";

/// Struct used for type checking:
struct TypeEnv
{

    /// Set of valid type names (including classes).
    Set<String> types;
    
    /// rules - Map of valid rules for how types interact with operators:
    ///     Map: String cannonical name -> String expressions type.
    Map<String,String> rules;

    ///Map of Maps for classes and their relevant variables:
    /// Map: String class name ->
    ///     Map: String variable/function name -> Vector of types
    ///         0 is var type or return type, 1+ are types for method args.
    Map<String,Map<String,Vector<String>>> variables;
    /// The name of the class that is currently being typechecked.
    Optional<String> currentClass;
    /// The canonical name of the method that is currently being typechecked.
    Optional<String> currentMethod;
};


} //namespace dflat

#endif // TYPECHECKER_TOOLS_HPP
