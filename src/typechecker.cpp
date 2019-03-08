#include "typechecker.hpp"

namespace dflat
{

void typeCheck(Vector<ASNPtr> const&)
{
    //TODO
}


//TypeChecker Exception:

TypeCheckerException::TypeCheckerException(String msg) noexcept
{
    message = msg;
}

const char* TypeCheckerException::what() const noexcept
{
    return message.c_str();
}


//TypeChecker:

TypeChecker::TypeChecker(Vector<ASNPtr> const& program_)
    : program(program_)
{
}

} //namespace dflat
