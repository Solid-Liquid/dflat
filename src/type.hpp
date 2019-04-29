#pragma once

#include "string.hpp"
#include "vector.hpp"
#include "variant.hpp"
#include "valueptr.hpp"

namespace dflat
{

class TUndefined;
class TVar;
class TConst;
class TClass;
class TMethod;
class TFunc;

using TType = Variant<TUndefined, TVar, TConst, TClass, TMethod, TFunc>;

class Type;

class TUndefined
{
    public:
        TUndefined() = default;
        String toString() const;
        bool isConcrete() const;
};

class TVar
{
    public:
        TVar(String);
        String toString() const;
        bool isConcrete() const;
        String const& name() const;

    private:
        String _name;
};

bool operator==(TVar const&, TVar const&);
bool operator!=(TVar const&, TVar const&);

class TConst
{
    public:
        TConst(String);
        String toString() const;
        bool isConcrete() const;
        String const& name() const;

    private:
        String _name;
};

bool operator==(TConst const&, TConst const&);
bool operator!=(TConst const&, TConst const&);

class TClass
{
    public:
        TClass(String, Vector<Type>);
        String toString() const;
        bool isConcrete() const;
        String const& name() const;
        Vector<Type> const& vars() const;

    private:
        String _name;
        Vector<Type> _vars;
};

bool operator==(TClass const&, TClass const&);
bool operator!=(TClass const&, TClass const&);

class TMethod
{
    public:
        TMethod(TClass, Type, Vector<Type>);
        String toString() const;
        bool isConcrete() const;
        TClass const& owner() const;
        Type const& ret() const;
        Vector<Type> const& args() const;

    private:
        TClass _owner;
        ValuePtr<Type> _ret;
        Vector<Type> _args;
};

bool operator==(TMethod const&, TMethod const&);
bool operator!=(TMethod const&, TMethod const&);

class TFunc
{
    public:
        TFunc(Type, Vector<Type>);
        String toString() const;
        bool isConcrete() const;
        Type const& ret() const;
        Vector<Type> const& args() const;

    private:
        ValuePtr<Type> _ret;
        Vector<Type> _args;
};

bool operator==(TFunc const&, TFunc const&);
bool operator!=(TFunc const&, TFunc const&);

class Type : public TType
{
    public:
        using TType::TType;
        String toString() const;
        bool isConcrete() const;

        template <typename T>
        T* as()
        {
            if (!TType::is<T>())
            {
                return nullptr;
            }
            else
            {
                return &TType::get_unchecked<T>();
            }
        }
};

bool operator==(Type const&, Type const&);
bool operator!=(Type const&, Type const&);

// Builtin types
TConst const intType("int");
TConst const boolType("bool");
TConst const voidType("void");

bool isBuiltinType(Type const&);
char const* translateBuiltinType(Type const&);

} // namespace dflat


// std::hash overloads.
namespace std
{

#define TYPEHASHER(T) template <> struct hash<dflat::T> {\
    size_t operator()(dflat::T const& x) const {\
    return std::hash<dflat::String>{}(x.toString());\
    }}

TYPEHASHER(TUndefined);
TYPEHASHER(TVar);
TYPEHASHER(TConst);
TYPEHASHER(TClass);
TYPEHASHER(TMethod);
TYPEHASHER(Type);

} // namespace std
