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

//// Typecheck a single syntax node, returning its type.
//Type typeCheck(ASNPtr const& asn)
//{
//    TypeEnv env;
//    return asn->typeCheck(env);
//}

} //namespace dflat
