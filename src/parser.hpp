#ifndef PARSER_HPP
#define PARSER_HPP

#include "asn.hpp"
#include "token.hpp"
#include "vector.hpp"

namespace dflat
{

ASNPtr parse(Vector<TokenPtr> const&);

class ParserException : public std::exception
{
public:
    using std::exception::exception;
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
