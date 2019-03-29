#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

#include "asn.hpp"
#include "codegenerator_tools.hpp"

namespace dflat
{

String codeGenerator(Vector<ASNPtr> const& program);

} // namespace dflat

#endif // CODEGENERATOR_HPP
