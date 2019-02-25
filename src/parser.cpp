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

Exp Parser::parseExp()
{
    const ParseResult<Exp> result = parseExp(0);
    // TODO: Why does result->tokenPos not work?
    if (result->tokenPos == tokens->size()) {
        return result->result;
    } else {
        throw new ParserException("Extra tokens starting at " + result->tokenPos);
    }
}

ParseResult(T&& result, std::size_t tokenPos)
    : _result(std::move(result)), _tokenPos(tokenPos)
{}

} //namespace dflat
