#ifndef PARSER_HPP
#define PARSER_HPP

#include "map.hpp"
#include "string.hpp"
#include "token.hpp"

namespace dflat
{

class Parser
{
public:
    const Vector<TokenPtr> tokens;
    Parser();
private:

};

class ParserException: public std::exception
{
public:
    ParserException(String msg) noexcept;
    const char* what() const noexcept;

    Node parseExp();
private:
    String message;

    ParseResult<Node> parseExp(const int startPos);
};

class ParseResult<A> {
    public:
        const A result;
        const int tokenPos;
        ParseResult(const A result, const int tokenPos);
} // ParseResult

} //namespace dflat

#endif // PARSER_HPP
