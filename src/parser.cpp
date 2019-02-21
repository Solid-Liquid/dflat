#include "parser.hpp"

namespace dflat
{

using namespace std;

Parser::Parser()
{
    
}

OpPtr Parser::getOp(TokenPtr const& tok) const
{

    switch (tok->getType())
    {
        case tokPlus:
            return make_unique<PlusOp>();
        case tokMinus:
            return make_unique<MinusOp>();
        case tokMult:
            return make_unique<MultOp>();
        case tokDiv:
            return make_unique<DivOp>();
        default:
            return nullptr;
    }
}

} //namespace dflat
