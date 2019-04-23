#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

#include "asn.hpp"
#include "codegenerator_tools.hpp"

namespace dflat
{

String generateCode(Vector<ASNPtr> const& program, TypeEnv const&);

} // namespace dflat

#endif // CODEGENERATOR_HPP
