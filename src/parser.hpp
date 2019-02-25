#ifndef PARSER_HPP
#define PARSER_HPP

#include "map.hpp"
#include "string.hpp"
#include "token.hpp"
#include "op.hpp"

namespace dflat
{

class Parser
{
public:
    const Vector<TokenPtr> tokens;
    Parser();
private:

    OpPtr getOp(TokenPtr const&) const;
};

class ParserException: public std::exception
{
public:
    ParserException(String msg) noexcept;
    const char* what() const noexcept;

    Exp parseExp() throws ParserException;
private:
    String message;

    ParseResult<Exp> parseExp(const int startPos) throws ParserException;
};

class ParseResult<A> {
    public:
        const A result;
        const int tokenPos;
        ParseResult(const A result, const int tokenPos);
} // ParseResult

} //namespace dflat

#endif // PARSER_HPP
