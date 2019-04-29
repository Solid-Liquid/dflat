#include "type.hpp"
#include "map.hpp"
#include <stdexcept>

namespace dflat
{

static
bool isConcrete(Vector<Type> const& ts)
{
    for (Type const& t : ts)
    {
        if (!t.isConcrete())
        {
            return false;
        }
    }

    return true;
}


// TUndefined

String TUndefined::toString() const
{
    return "#undefined";
}

bool TUndefined::isConcrete() const
{
    return false;
}


// TVar

TVar::TVar(String name)
    : _name(std::move(name))
{}

String TVar::toString() const
{
    return "`" + name();
}
bool TVar::isConcrete() const
{
    return false;
}

String const& TVar::name() const
{
    return _name;
}

bool operator==(TVar const& a, TVar const& b)
{
    return a.name() == b.name();
}

bool operator!=(TVar const& a, TVar const& b)
{
    return !(a == b);
}


// TConst

TConst::TConst(String name)
    : _name(std::move(name))
{}

String TConst::toString() const
{
    return name();
}
bool TConst::isConcrete() const
{
    return true;
}

String const& TConst::name() const
{
    return _name;
}

bool operator==(TConst const& a, TConst const& b)
{
    return a.name() == b.name();
}

bool operator!=(TConst const& a, TConst const& b)
{
    return !(a == b);
}


// TClass

TClass::TClass(String name, Vector<Type> vars)
    : _name(std::move(name))
    , _vars(std::move(vars))
{}

String TClass::toString() const
{
    String s = _name + "[";
    bool first = true;

    for (Type const& v : _vars)
    {
        if (first) { first = false; }
        else       { s += ","; }

        s += v.toString();
    }

    s += "]";
    return s;
}

bool TClass::isConcrete() const
{
    return dflat::isConcrete(_vars);
}

String const& TClass::name() const
{
    return _name;
}

Vector<Type> const& TClass::vars() const
{
    return _vars;
}

bool operator==(TClass const& a, TClass const& b)
{
    return a.name() == b.name()
        && a.vars() == b.vars();
}

bool operator!=(TClass const& a, TClass const& b)
{
    return !(a == b);
}


// TMethod

TMethod::TMethod(TClass owner, Type ret, Vector<Type> args)
    : _owner(std::move(owner))
    , _ret(std::move(ret))
    , _args(std::move(args))
{}

String TMethod::toString() const
{
    String s = _owner.toString() 
             + "::" 
             + _ret->toString() 
             + "(";
    bool first = true;

    for (Type const& a : _args)
    {
        if (first) { first = false; }
        else       { s += ","; }

        s += a.toString();
    }

    s += ")";
    return s;
}

bool TMethod::isConcrete() const
{
    return _owner.isConcrete()
        && _ret->isConcrete()
        && dflat::isConcrete(_args);
}

TClass const& TMethod::owner() const
{
    return _owner;
}

Type const& TMethod::ret() const
{
    return _ret;
}

Vector<Type> const& TMethod::args() const
{
    return _args;
}

bool operator==(TMethod const& a, TMethod const& b)
{
    return a.owner() == b.owner()
        && a.ret()   == b.ret()
        && a.args()  == b.args();
}

bool operator!=(TMethod const& a, TMethod const& b)
{
    return !(a == b);
}


// TFunc

TFunc::TFunc(Type ret, Vector<Type> args)
    : _ret(std::move(ret))
    , _args(std::move(args))
{}

String TFunc::toString() const
{
    String s = _ret->toString() 
             + "(";
    bool first = true;

    for (Type const& a : _args)
    {
        if (first) { first = false; }
        else       { s += ","; }

        s += a.toString();
    }

    s += ")";
    return s;
}

bool TFunc::isConcrete() const
{
    return _ret->isConcrete()
        && dflat::isConcrete(_args);
}

Type const& TFunc::ret() const
{
    return _ret;
}

Vector<Type> const& TFunc::args() const
{
    return _args;
}

bool operator==(TFunc const& a, TFunc const& b)
{
    return a.ret()   == b.ret()
        && a.args()  == b.args();
}

bool operator!=(TFunc const& a, TFunc const& b)
{
    return !(a == b);
}


// Type

String Type::toString() const
{
    return this->match([](auto const& t)
    {
        return t.toString();
    });
}

bool Type::isConcrete() const
{
    return this->match([](auto const& t)
    {
        return t.isConcrete();
    });
}

bool operator==(Type const& a, Type const& b)
{
    return static_cast<TType const&>(a)
        == static_cast<TType const&>(b);
}

bool operator!=(Type const& a, Type const& b)
{
    return !(a == b);
}


// Builtin types

namespace 
{
    struct BuiltinType
    {
        char const* translation;
    };

    const Map<TConst, BuiltinType> s_builtinTypes
    {
        { intType, {"int"} },
        { boolType, {"int"} },
        { voidType, {"void"} },
    };
}

bool isBuiltinType(TConst const& type)
{
    return s_builtinTypes.count(type) != 0;
}

char const* translateBuiltinType(TConst const& type)
{
    BuiltinType const* bt = lookup(s_builtinTypes, type);

    if (!bt)
    {
        throw std::logic_error("No builtin type '" + type.toString() + "'");
    }
    
    return bt->translation;
}

} // namespace dflat
