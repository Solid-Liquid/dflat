#include "classmeta.hpp"
#include "asn.hpp"
#include "typechecker.hpp"
#include <iostream>

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

    // Already declared OK. Handle in user.
}

ClassMeta* ClassMetaMan::_lookup(ValueType const& type)
{
    return dflat::lookup(_classes, type);
}

ClassMeta const* ClassMetaMan::lookup(ValueType const& type) const
{
    return dflat::lookup(_classes, type);
}

Optional<MemberMeta> ClassMetaMan::lookupVar(ValueType const& classType,
        String const& memberName) const
{
    int depth = 1;
    ClassMeta const* cm = dflat::lookup(_classes, classType);

    while (cm)
    {
        Type const* memberType = dflat::lookup(cm->members, memberName);

        if (memberType)
        {
            return MemberMeta{ depth, *memberType };
        }
        else
        {
            ++depth;

            if (cm->parent)
            {
                cm = lookup(cm->parent.value());
            }
            else
            {
                cm = nullptr;
            }
        }
    }

    return nullopt;
}

Optional<MemberMeta> ClassMetaMan::lookupMethod(ValueType const& classType,
        CanonName const& methodName) const
{
    return lookupVar(classType, methodName.canonName());
}

ClassMeta const* ClassMetaMan::cur() const
{
    if (!_curClass)
    {
        return nullptr;
    }

    return lookup(*_curClass);
}

void ClassMetaMan::addVar(String const& name, ValueType const& type)
{
    if (!cur())
    {
        throw std::logic_error("adding member when no current class");
    }

    ClassMeta* classMeta = _lookup(cur()->type);
    classMeta->members.insert({ name, type });
}

void ClassMetaMan::addMethod(CanonName const& methodName)
{
    if (!cur())
    {
        throw std::logic_error("adding member when no current class");
    }

    ClassMeta* classMeta = _lookup(cur()->type);
    classMeta->members.insert({ methodName.canonName(), methodName.type() });
}

void ClassMetaMan::setParent(ValueType const& parentType)
{
    if (!cur())
    {
        throw std::logic_error("setting parent when no current class");
    }
    
    ClassMeta* classMeta = _lookup(cur()->type);
    classMeta->parent = parentType;
}

void ClassMetaMan::print() const
{
    std::cout << "ClassMeta:";
    for (auto [ck,cv] : _classes)
    {
        std::cout << "\n" << ck.toString();

        if (cv.parent)
        {
            std::cout << " extends " << cv.parent->toString();
        }

        if (!cv.members.empty())
        {
            for (auto [mk,mv] : cv.members)
            {
                std::cout << "\n\t" << mk;
            }
        }
    }
    if (_curClass)
    {
        std::cout << "\n(cur: " << _curClass->toString() << ")";
    }
    std::cout << "\n";
}

} // namespace dflat
