#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "asn.hpp"

namespace dflat
{

void typeCheck(const Vector<ASNPtr> &);  //runner function

class TypeCheckerException : virtual std::exception
{
    public:
        TypeCheckerException(String msg) noexcept;
        const char* what() const noexcept;
    private:
        String message;
};

class TypeChecker
{
    private:
        Vector<ASNPtr> const& program;
        TypeRef typeRef;  //TypeRef struct defined in asn.hpp

    public:
        TypeChecker(Vector<ASNPtr> const&);
};

} //namespace dflat

#endif // TYPECHECKER_HPP
