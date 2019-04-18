#pragma once

#include "string.hpp"
#include "map.hpp"
#include "type.hpp"

namespace dflat
{

struct ClassMeta
{
    ValueType type;
    ClassMeta const* parent;
    Map<String, Type> members;
    
    ClassMeta(ValueType const& _type)
        : type(_type)
        , parent(nullptr)
    {}
};

class ClassMetaMan
{
    Map<ValueType, ClassMeta> _classes;
    ClassMeta* _curClass = nullptr;

    public:
        void enter(ValueType const& type)
        {
            _curClass = declare(type);
        }

        void leave()
        {
            _curClass = nullptr;
        }

        ClassMeta* declare(ValueType const& type)
        {
            auto it = _classes.find(type);

            if (it == _classes.end())
            {
                auto result = _classes.insert({ type, ClassMeta(type) });
                it = result.first;
            }

            return &it->second;
        }

        ClassMeta* lookup(ValueType const& type)
        {
            return dflat::lookup(_classes, type);
        }

        int classHasMember(ValueType const& type, String const& member) const
        {
            int depth = 1;
            ClassMeta const* cm = dflat::lookup(_classes, type);

            while (cm)
            {
                if (cm->members.count(member))
                {
                    return depth;
                }
                else
                {
                    ++depth;
                    cm = cm->parent;
                }
            }

            return 0; // Does not have member.
        }

        ClassMeta* cur()
        {
            return _curClass;
        }
        
        ClassMeta const* cur() const
        {
            return _curClass;
        }
};

} // namespace dflat
