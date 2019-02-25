#include "parser.hpp"

namespace dflat
{

using namespace std;

ParserException::ParserException(String msg) noexcept
{
    message = msg;
}

const char* ParserException::what() const noexcept
{
    return message.c_str();
}

Parser::Parser()
{
    
}

<<<<<<< HEAD
Exp Parser::parseExp() throws ParserException 
{
    const ParseResult<Exp> result = parseExp(0);
    // TODO: Why does result->tokenPos not work?
    if (result->tokenPos == tokens->size()) {
        return result->result;
    } else {
        throw new ParserException("Extra tokens starting at " + result->tokenPos);
    }
}
=======
//OpPtr Parser::getOp(TokenPtr const& tok) const
//{
//
//    switch (tok->getType())
//    {
//        case tokPlus:
//            return make_unique<PlusOp>();
//        case tokMinus:
//            return make_unique<MinusOp>();
//        case tokMult:
//            return make_unique<MultOp>();
//        case tokDiv:
//            return make_unique<DivOp>();
//        default:
//            return nullptr;
//    }
//}
>>>>>>> Parser-Initial

ParseResult::ParseResult(const A result, const int tokenPos) 
{
    this->result = result;
    this->tokenPos = tokenPos;
}

} //namespace dflat
