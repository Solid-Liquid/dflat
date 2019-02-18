#ifndef LEXER_HPP
#define LEXER_HPP

#include "lexercore.hpp"

namespace dflat
{

class Lexer : private LexerCore
{
    public:
        Lexer(String const&);
};

} //namespace dflat

#endif // LEXER_HPP
