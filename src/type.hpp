#pragma once

#include "string.hpp"
#include "vector.hpp"
#include "variant.hpp"

namespace dflat
{

using TypeName = String;

// A "single" type. 
// This may contain arguments representing either
//  free type variables or applied type arguments.
class ValueType
{
    private:
        TypeName _name;
        Vector<ValueType> _tvars;

    public:
        explicit ValueType(TypeName);
        ValueType(TypeName, Vector<ValueType>);
        
        TypeName const& name() const;
        Vector<ValueType> const& tvars() const;
        String toString() const;
        bool operator==(ValueType const&) const;
        bool operator!=(ValueType const&) const;
};

// A method type consisting of a return type and argument types.
class MethodType
{
    private:
        ValueType _ret;
        Vector<ValueType> _args;

    public:
        MethodType(ValueType const&, Vector<ValueType> const&);
        void addArgType(ValueType const&);
       
        ValueType const& ret() const;
        Vector<ValueType> const& args() const;
        String toString() const;
        bool operator==(MethodType const&) const;
        bool operator!=(MethodType const&) const;
};

// Either a ValueType or MethodType.
class Type : private Variant<ValueType, MethodType>
{
    using Base = Variant<ValueType, MethodType>;

    void assertValue() const;
    void assertMethod() const;

    public:
        Type(ValueType const&);
        Type(MethodType const&);

        // SubType selectors. They throw when wrong.
        ValueType const& value() const;
        ValueType& value();
        MethodType const& method() const;
        MethodType& method();

        bool isValue() const;
        bool isMethod() const;
        String toString() const;
        bool operator==(Type const&) const;
        bool operator!=(Type const&) const;
};

// Type that can't be used.
ValueType const undefinedType("undefined");

// Builtin types
ValueType const intType("int");
ValueType const boolType("bool");
ValueType const voidType("void");

bool isBuiltinType(ValueType const&);
char const* translateBuiltinType(ValueType const&);


} // namespace dflat


// std::hash overloads.
namespace std
{

template <>
struct hash<dflat::ValueType>
{
    size_t operator()(dflat::ValueType const& x) const
    {
        return std::hash<dflat::TypeName>{}(x.name());
    }
};

} // namespace std
