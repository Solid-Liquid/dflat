#ifndef PARSER_HPP
#define PARSER_HPP

#include "map.hpp"
#include "string.hpp"
#include "token.hpp"
#include "asn.hpp"

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

    ASNPtr parseExp();
private:
    String message;

    ParseResult parseExp(const int startPos);
};

template <typename T>
class ParseResult {
public:
    T _result;
    std::size_t _tokenPos;
    ParseResult(T&& result, std::size_t tokenPos);
};

} //namespace dflat

#endif // PARSER_HPP
