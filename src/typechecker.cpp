#include "typechecker.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{
// Typecheck entire program, returning final environment.
TypeEnv typeCheck(Vector<ASNPtr> const& program)
{
    TypeEnv env;

    for (ASNPtr const &class_ : program)
    {
        class_->typeCheck(env);
    }

    return env;
}

// Typecheck a single syntax node, returning its type.
Type typeCheck(ASNPtr const& asn)
{
    TypeEnv env;
    return asn->typeCheck(env);
}

void assertTypeIs(Type const &test, Type const &against)
{
    if (test != against)
    {
        throw TypeCheckerException(
            "Type '" + test.toString() + 
            "' must be '" + against.toString() + "'"
            );
    }
}

// Note that this function can produce a canonical name for an illegal type.
// This is for the sake of decent errors. That is, while only ValueTypes
// are allowed as arguments, this takes any Type.
String funcCanonicalName(String const &name, Vector<Type> const &argTypes)
{
    String canonicalName = name + "(";
    bool first = true;

    for (Type const &argType : argTypes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            canonicalName += ",";
        }

        canonicalName += argType.toString();
    }

    canonicalName += ")";
    return canonicalName;
}

String unopCanonicalName(OpType op, Type const &rhsType)
{
    return funcCanonicalName(opString(op), { rhsType });
}

String binopCanonicalName(OpType op, Type const &lhsType, Type const &rhsType)
{
    return funcCanonicalName(opString(op), { lhsType, rhsType });
}

TypeCheckerException::TypeCheckerException(String msg) noexcept
{
    message = "TypeChecker Exception:\n" + msg;
}

const char *TypeCheckerException::what() const noexcept
{
    return message.c_str();
}

} //namespace dflat
