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

// Here is the only way to construct a CanonName.
CanonName canonize(String s)
{
    return CanonName(std::move(s));
}

CanonName canonizeOp(OpType op, Vector<Type> const& args)
{
    String s = opString(op)
             + "(";
    bool first = true;

    for (Type const& a : args)
    {
        if (first) { first = false; }
        else       { s += ","; }

        s += a.toString();
    }

    s += ")";
    return canonize(std::move(s));
}

CanonName canonizeMethod(String const& name, TMethod const& type)
{
    // Does not include return type.
    String s = type.owner().toString()
             + "::"
             + name
             + "(";
    bool first = true;
    
    for (Type const& a : type.args())
    {
        if (first) { first = false; }
        else       { s += ","; }

        s += a.toString();
    }

    s += ")";
    return canonize(std::move(s));
}

CanonName canonizeCons(TMethod const& type)
{
    return canonizeMethod(config::consName, type);
}

CanonName canonizeLocal(String const& name)
{
    return canonize(name);
}

CanonName canonizeMember(String const& name, TClass const& classType)
{
    // Does not include return type.
    String s = classType.toString()
             + "::"
             + name;
    return canonize(std::move(s));
}

} //namespace dflat
