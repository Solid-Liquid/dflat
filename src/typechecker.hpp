#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "asn.hpp"
#include "typechecker_tools.hpp"

namespace dflat
{
TypeEnv typeCheck(Vector<ASNPtr> const&);
Type typeCheck(ASNPtr const&);

CanonName canonizeOp(OpType, Vector<Type> const&);
CanonName canonizeMethod(String const&, TMethod const&);
CanonName canonizeCons(TMethod const&);
CanonName canonizeLocal(String const&);
CanonName canonizeMember(String const&, TClass const&);

} //namespace dflat

#endif // TYPECHECKER_HPP
