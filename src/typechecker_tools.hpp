#ifndef TYPECHECKER_TOOLS_HPP
#define TYPECHECKER_TOOLS_HPP

#include "type.hpp"
#include "string.hpp"
#include "map.hpp"
#include "set.hpp"
#include "optional.hpp"
#include "vector.hpp"
#include "classmeta.hpp"

namespace dflat
{

//Macro that makes a dynamic cast look like less of a mess:
#define cast(ptr,type) dynamic_cast<type*>(ptr.get())

ValueType const intType("int");
ValueType const boolType("bool");
ValueType const voidType("void");

/// Struct used for type checking:
struct TypeEnv
{
    /// Set of instanciable types (user classes, builtin types).
    Set<ValueType> types;
    
    /// rules - Map of valid rules for how types interact with operators:
    ///     Map: String canonical name -> String expressions return type.
    Map<String,Type> rules;

    ///Map of Maps for classes and their relevant variables:
    /// Map: ValueType class ->
    ///     Map: String variable/function name -> Type
    Map<ValueType, Map<String, Type>> variables;
    
    /// The canonical name of the method that is currently being typechecked.
    Optional<String> currentMethod;

    ClassMetaMan classes;
};


} //namespace dflat

#endif // TYPECHECKER_TOOLS_HPP
