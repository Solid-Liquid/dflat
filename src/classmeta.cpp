#include "classmeta.hpp"

#include "string.hpp"
#include "map.hpp"
#include "type.hpp"

namespace dflat
{

void ClassMetaMan::enter(ValueType const& classType)
{
    declare(classType);
    _curClass = classType;
}

void ClassMetaMan::leave()
{
    _curClass = nullopt;
}

void ClassMetaMan::declare(ValueType const& type)
{
    auto it = _classes.find(type);

    if (it == _classes.end())
    {
        auto result = _classes.insert({ type, ClassMeta(type) });
        it = result.first;
    }
}

ClassMeta* ClassMetaMan::lookup(ValueType const& type)
{
    return dflat::lookup(_classes, type);
}

ClassMeta const* ClassMetaMan::lookup(ValueType const& type) const
{
    return dflat::lookup(_classes, type);
}

int ClassMetaMan::classHasMember(ValueType const& type, String const& member) const
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

            if (cm->parent)
            {
                cm = lookup(*cm->parent);
            }
            else
            {
                cm = nullptr;
            }
        }
    }

    return 0; // Does not have member.
}

ClassMeta const* ClassMetaMan::cur() const
{
    if (!_curClass)
    {
        return nullptr;
    }
//    if (!_curClass)
//    {
//        throw std::logic_error("no current class");
//    }

    return lookup(*_curClass);
}

void ClassMetaMan::addMember(String const& memberName, Type const& memberType)
{
    if (!cur())
    {
        throw std::logic_error("adding member when no current class");
    }

    ClassMeta* classMeta = lookup(cur()->type);
    classMeta->members.insert({ memberName, memberType });
}

void ClassMetaMan::setParent(ValueType const& parentType)
{
    if (!cur())
    {
        throw std::logic_error("setting parent when no current class");
    }
    
    ClassMeta* classMeta = lookup(cur()->type);
    classMeta->parent = parentType;
}

} // namespace dflat
