#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "asn.hpp"
#include "typechecker_tools.hpp"

namespace dflat
{

//Runner Functions:
TypeEnv typeCheck(Vector<ASNPtr> const&);
Type typeCheck(ASNPtr const&);

#pragma region HelperFunctions

/** Look up the rule for an operator based on cannonical name
 * Throw if not found. */ 
Type lookupRuleType(TypeEnv const&, String const&);

/** Lookup the type of a method by name in current class.
 * Throw if method does not exist or is not a MethodType */ 
MethodType lookupMethodType(TypeEnv const&, String const&);

/** Same as lookupMethodType, but with a specified class. */ 
MethodType lookupMethodTypeByClass(TypeEnv const&, String const&, ValueType const&);

/** Lookup the type of a variable by name in current class.
 * Throw if variable does not exist or is not a ValueType */ 
ValueType lookupVarType(TypeEnv const&, String const&);

/** Same as lookupVarType, but with a specified class. */ 
ValueType lookupVarTypeByClass(TypeEnv const&, String const&, ValueType const&);

/** Check if type has been declared.
 * Return true if valid, throw if not valid. */ 
bool validType(TypeEnv const&, ValueType const&);

/** <summary>Throw if two types aren't equal.</summary> */
void assertTypeIs(Type const&, Type const&);

/// <summary>
/// Make a canonical name for functions (methods, operators).
/// This name can be used to look up a function's return type.
/// </summary>
/// <param name="name">The original function name</param>
/// <param name="argTypes">The original function's parameter types</param>
/// <returns>name(arg_1, arg_2, ..., arg_n)</returns>
String funcCanonicalName(String const&, Vector<Type> const& args);

/// <summary>
/// Turns an operator into a function of the form op(rhsType)
/// </summary>
/// <param name="op">The operator (+, -, *, /)</param>
/// <param name="rhsType">Type of operand</param>
/// <returns>op(rhsType)</returns>
String unopCanonicalName(OpType, Type const&);

/// <summary>
/// Turns an operator into a function of the form op(rhsType)
/// </summary>
/// <param name="op">The operator (+, -, *, /)</param>
/// <param name="lhsType">Type of left operand</param>
/// <param name="rhsType">Type of right operand</param>
/// <returns>op({lhsType, rhsType})</returns>
String binopCanonicalName(OpType, Type const&, Type const&);

// Registers a name as having the given type (with the current class).
void mapNameToType(TypeEnv&, String const&, Type const&);

// Declares a new type, which can then be instanciated.
void declareClass(TypeEnv&, ValueType const&);

TypeEnv initialTypeEnv();

#pragma endregion

//TypeChecker Exceptions class:
class TypeCheckerException : public std::exception
{
    public:
        /** TypeChecker Exception */
        TypeCheckerException(String msg) noexcept;
        const char* what() const noexcept;
    private:
        String message;
};

} //namespace dflat

#endif // TYPECHECKER_HPP
