#include "lexercore.hpp"

namespace dflat
{

LexerCore::LexerCore(String const& input)
    : _input(input)
    , _pos(0)
{
}
    
// Returns true iff no input left.
bool LexerCore::at_end() const
{
    return _pos >= _input.size();
}

// Returns current char and advances, or \0 if no input left.
char LexerCore::get()
{
    if (at_end()) 
    {
        return 0;
    }
    else
    {
        char c = _input[_pos];
        ++_pos;
        return c;
    }
}

// Returns current char, or \0 if no input left.
char LexerCore::peek() const
{
    if (at_end())
    {
        return 0;
    }
    else
    {
        return _input[_pos];
    }
}

// Advances unless no input left.
void LexerCore::next()
{
    if (!at_end())
    {
        ++_pos;
    }
}

} //namespace dflat
