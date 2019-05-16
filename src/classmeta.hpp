#pragma once

#include "string.hpp"
#include "map.hpp"
#include "set.hpp"
#include "type.hpp"
#include "canonname.hpp"
#include "optional.hpp"

namespace dflat
{

struct MethodExp;

struct ClassMeta
{
    // Class type.
    ValueType type;

    // Parent type.
    Optional<ValueType> parent;

    // Map of all members from "canonical name" to type.
    // It's not an actual canonical name, just a String.
    // This is so it can handle vars too. 
    Map<String, Type> members;

    // Holds all non-constructor method canonical names.
    Set<CanonName> methods;
    
    ClassMeta(ValueType const& _type)
        : type(_type)
    {}
};

struct MemberMeta
{
    int depth;
    Type type;
    ValueType baseClassType;
};

class ClassMetaMan
{
    Map<ValueType, ClassMeta> _classes;
    Optional<ValueType> _curClass;

    public:
        void enter(ValueType const& classType);
        void leave();
        void declare(ValueType const& classType);
        ClassMeta const* lookup(ValueType const& classType) const;
        Optional<MemberMeta> lookupVar(ValueType const& classType, String const&) const;
        Optional<MemberMeta> lookupMethod(ValueType const& classType, CanonName const&) const;
        Map<ValueType, ClassMeta> const& allClasses() const;
        void addVar(String const&, ValueType const&);
        void addMethod(CanonName const&);
        void setParent(ValueType const& parentType);
        ClassMeta const* cur() const;
        void print() const;

    private:
        ClassMeta* _lookup(ValueType const& classType);
};

} // namespace dflat
