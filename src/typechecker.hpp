#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "asn.hpp"
#include "typechecker_tools.hpp"

namespace dflat
{

//Runner Functions:
TypeEnv typeCheck(Vector<ASNPtr> const&);
Type typeCheck(ASNPtr const&);

//Helper Functions:
/**
 * Look up the rule for an operator based on cannonical name
 * Throw if not found.
 */ 
Type lookupRuleType(TypeEnv const&, String const&);
/**
 * Lookup the return type/arg type(s) of a method by name in current class.
 * Throw if method does not exist
 */ 
Vector<Type> lookupMethodType(TypeEnv const&, String const&);
/**
 * Same as lookupMethodType, but with a specified class.
 */ 
Vector<Type> lookupMethodTypeByClass(TypeEnv const&, String const&, String const&);
/**
 * Lookup the return type/arg type(s) of a variable by name in current class.
 * Throw if method does not exist
 */ 
Type lookupVarType(TypeEnv const&, String const&);
/**
 * Same as lookupVarType, but with a specified class.
 */ 
Type lookupVarTypeByClass(TypeEnv const&, String const&, String const&);
/**
 * Check if "type" is a valid type within the set of types.
 * Return true if valid, throw if not valid.
 */ 
bool validType(TypeEnv const&, String const&);
/**
 * Throw if two types aren't equal.
 */ 
void assertTypeIs(Type const&, Type const&);
/**
 * This is how a comment should look
 * <summary>
 * Make a canonical name for functions (methods, operators).
 * This name can be used to look up a function's return type.
 * </summary>
 * <param name="name">
 * <param name="argTypes">
 * <returns>String</returns>
 **/
String funcCanonicalName(String const&, Vector<Type> const&);
String unopCanonicalName(OpType, Type const&);
String binopCanonicalName(OpType, Type const&, Type const&);
void mapNameToType(TypeEnv&, String const&, Vector<Type> const&);
TypeEnv initialTypeEnv();

//TypeChecker Exceptions class:
class TypeCheckerException : public std::exception
{
    public:
        /** TypeChecker Exception: */ 
        TypeCheckerException(String msg) noexcept;
        const char* what() const noexcept;
    private:
        String message;
};

} //namespace dflat

#endif // TYPECHECKER_HPP
