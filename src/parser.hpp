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

} //namespace dflat

#endif // PARSER_HPP
