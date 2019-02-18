#ifndef LEXERCORE_HPP
#define LEXERCORE_HPP

#include "string.hpp"

namespace dflat
{

struct LexerCore
{
    LexerCore(String const&);

    bool at_end() const;
    char get();
    char peek() const;
    void next();    

    String _input;
    size_t _pos;
};

} //namespace dlfat

#endif // LEXER_HPP
