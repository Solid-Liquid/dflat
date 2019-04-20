#pragma once

#include "string.hpp"
#include "map.hpp"
#include "type.hpp"
#include "optional.hpp"

namespace dflat
{

struct ClassMeta
{
    ValueType type;
    Optional<ValueType> parent;
    Map<String, Type> members;
    
    ClassMeta(ValueType const& _type)
        : type(_type)
    {}
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
        int classHasMember(ValueType const& classType, 
                String const& memberName) const;
        void addMember(String const& memberName, Type const& memberType);
        void setParent(ValueType const& parentType);
        ClassMeta const* cur() const;

    private:
        ClassMeta* lookup(ValueType const& classType);
};

} // namespace dflat
