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

// Returns the char at _pos + i, or \0 if it's past end.
char LexerCore::peek_ahead(size_t i) const
{
    size_t ahead_pos = _pos + i;

    if (ahead_pos >= _input.size())
    {
        return 0;
    }
    else
    {
        return _input[ahead_pos];
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
