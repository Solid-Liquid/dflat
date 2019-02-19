#ifndef LEXER_HPP
#define LEXER_HPP

#include "lexercore.hpp"
#include "vector.hpp"
#include "token.hpp"

namespace dflat
{

Vector<TokenPtr> tokenize(String const&);

} //namespace dflat

#endif // LEXER_HPP
