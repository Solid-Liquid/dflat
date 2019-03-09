#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "asn.hpp"

namespace dflat
{

TypeEnv typeCheck(Vector<ASNPtr> const&);  //runner function
Type typeCheck(ASNPtr const&);  //runner function

class TypeCheckerException : virtual std::exception
{
    public:
        TypeCheckerException(String msg) noexcept;
        const char* what() const noexcept;
    private:
        String message;
};

//class TypeChecker
//{
//    private:
//        Vector<ASNPtr> const& program;
//        TypeEnv typeEnv;  //TypeEnv struct defined in asn.hpp
//
//    public:
//        TypeChecker(Vector<ASNPtr> const&);
//};

Type lookupType(TypeEnv const&, String const&);
void pushTypeScope(TypeEnv&);
void popTypeScope(TypeEnv&);
void assertTypeIs(Type const&, Type const&);
String funcCanonicalName(String const&, Vector<Type> const&);
String unopCanonicalName(OpType, Type const&);
String binopCanonicalName(OpType, Type const&, Type const&);

} //namespace dflat

#endif // TYPECHECKER_HPP
