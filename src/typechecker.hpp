#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "asn.hpp"
#include "typechecker_tools.hpp"

namespace dflat
{

#pragma region RunnerFunctions

TypeEnv typeCheck(Vector<ASNPtr> const&);
Type typeCheck(ASNPtr const&);

#pragma endregion

#pragma region HelperFunctions

/** Throw an exception if two types aren't equal. */
void assertTypeIs(Type const&, Type const&);

/// <summary>Make a canonical name for functions (methods, operators).
/// This name can be used to look up a function's return type.</summary>
/// <param name="name">The original function name</param>
/// <param name="argTypes">The original function's parameter types</param>
/// <returns>name(arg_1, arg_2, ..., arg_n)</returns>
String funcCanonicalName(String const&, Vector<Type> const& args);

/// <summary>Turns an operator into a function of the form op(rhsType)</summary>
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

TypeEnv initialTypeEnv();

#pragma endregion

} //namespace dflat

#endif // TYPECHECKER_HPP
