#pragma once

#include "vector.hpp"
#include "optional.hpp"
#include "string.hpp"
#include "type.hpp"
#include "canonname.hpp"
#include <memory>

namespace dflat
{

struct ASN;
struct ClassDecl;


// Scope
struct Scope_;

using Scope = std::shared_ptr<Scope_>;

struct Scope_
{

};


// ClassMeta
struct ClassMeta
{
    TClass type;
};


// MethodMeta
struct MethodMeta
{
    CanonName name;
    TMethod type;
};


// Meta (manager)
class Meta
{
    public:
        Type const* lookupType(CanonName const&) const;
        bool isTypeBaseOf(Type const&, Type const&) const;
        
        Optional<MethodMeta> curMethod() const;
        Optional<ClassMeta> curClass() const;
        
        void registerClass(TClass, ClassDecl*);
        Scope newScope();
        void leaveScope(Scope const&);
        Scope newMethod(CanonName const&, Type const&);
        void leaveMethod(Scope const&);
        void newMethodCall(ASN*, CanonName const&);
        void newMember(CanonName const&, Type const&);
        void newLocal(CanonName const&, Type const&);
        void newFunc(CanonName const&, Type const&);
};

} // namespace dflat
