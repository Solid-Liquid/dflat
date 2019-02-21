#ifndef PARSER_HPP
#define PARSER_HPP

#include "map.hpp"
#include "string.hpp"
#include "token.hpp"
#include "op.hpp"

namespace dflat
{

class parser
{
public:
    parser();
private:
    // static Map<TokenPtr, OpPtr> const ADDITIVE_OP_MAP {
    //     { PlusToken(), PlusOp() },
    //     { MinusToken(), MinusOp() },
    // };

    // static Map<TokenPtr, OpPtr> const MULTIPLICATIVE_OP_MAP
    // {
    //     { MultiplyToken(), MultOp() },
    //     { DivisionToken(), DivisionToken() },
    // };
};

} //namespace dflat

#endif // PARSER_HPP
