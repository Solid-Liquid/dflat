#ifndef TYPECHECKER_TOOLS_HPP
#define TYPECHECKER_TOOLS_HPP

#include "type.hpp"
#include "string.hpp"
#include "map.hpp"
#include "set.hpp"
#include "optional.hpp"
#include "vector.hpp"
#include "meta.hpp"
#include "classmeta.hpp"
#include "scopemeta.hpp"
#include "methodmeta.hpp"

namespace dflat
{

struct ASN;
struct ClassDecl;

class TypeEnv
{
    public:
        TypeEnv();

        Type getType(CanonName const&) const;
        TClass getClassType(CanonName const&) const;
        TMethod getMethodType(CanonName const&) const;
        TFunc getFuncType(CanonName const&) const;
        Type getReturnType(CanonName const&) const;
        bool inMethod() const;
        
        ClassMeta getCurClass() const;
        MethodMeta getCurMethod() const;

        void assertTypeIs(Type const&, Type const&) const;
        void assertTypeIsOrBase(Type const&, Type const&) const;

        Scope newScope();
        void leaveScope(Scope const&);
       
        void registerClass(TClass const&, ClassDecl*);

        Scope newMethod(CanonName const&, Type const&);
        void leaveMethod(Scope const&);
        void newMethodCall(ASN*, CanonName const&);
      
        void newMember(CanonName const&, Type const&);

        void newLocal(CanonName const&, Type const&);
        
        void realize(Type const&);

    private:
        void initialize();

        Meta _meta;
    
    private:
//        ValueType mappedType(ValueType const&) const;
//
//        // Special map from type variable to concrete type.
//        Map<TypeName, ValueType> _tvars;

        friend class GenEnv;

};

class TypeCheckerException : public std::runtime_error
{
    public:
        TypeCheckerException(String msg) noexcept;
};

} //namespace dflat

#endif // TYPECHECKER_TOOLS_HPP
