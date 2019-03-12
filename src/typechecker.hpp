#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "asn.hpp"
#include "typechecker_tools.hpp"

namespace dflat
{

#define cast(ptr,type) dynamic_cast<type*>(ptr.get())

//Runner Functions:
TypeEnv typeCheck(Vector<ASNPtr> const&);
Type typeCheck(ASNPtr const&);

//Helper Functions:
Type lookupType(TypeEnv const&, String const&);
void pushTypeScope(TypeEnv&);
void popTypeScope(TypeEnv&);
void assertTypeIs(Type const&, Type const&);
String funcCanonicalName(String const&, Vector<Type> const&);
String unopCanonicalName(OpType, Type const&);
String binopCanonicalName(OpType, Type const&, Type const&);
TypeEnv initialTypeEnv();

//TypeChecker Exceptions class:
class TypeCheckerException : public std::exception
{
    public:
        TypeCheckerException(String msg) noexcept;
        const char* what() const noexcept;
    private:
        String message;
};

} //namespace dflat

#endif // TYPECHECKER_HPP
