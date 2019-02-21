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
    Parser();
private:

    OpPtr getOp(TokenPtr const&) const;
};

} //namespace dflat

#endif // PARSER_HPP
