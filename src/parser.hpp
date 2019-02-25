#ifndef PARSER_HPP
#define PARSER_HPP

#include "map.hpp"
#include "string.hpp"
#include "token.hpp"
//#include "op.hpp"

namespace dflat
{

class Parser
{
public:
    Parser();
private:

//    OpPtr getOp(TokenPtr const&) const;
};

class ParserException: public std::exception
{
public:
    ParserException(String msg) noexcept;
    const char* what() const noexcept;
private:
    String message;
};

} //namespace dflat

#endif // PARSER_HPP
