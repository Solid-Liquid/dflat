#pragma once

#include "string.hpp"
#include "vector.hpp"
#include "variant.hpp"

namespace dflat
{

using TypeName = String;

// A simple type consisting only of a name.
// Can be used as a key in a Set or Map.
class ValueType
{
    private:
        TypeName _name;

    public:
        explicit ValueType(TypeName const&);
        
        TypeName const& name() const;
        String toString() const; // Identical to name().
        bool operator==(ValueType const&) const;
        bool operator!=(ValueType const&) const;
        bool operator<(ValueType const&) const;
};

// A complex type consisting of a return type and argument types.
// Its types are themselves ValueTypes.
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

} // namespace dflat


// std::hash overload for ValueType.
namespace std
{

template <>
struct hash<dflat::ValueType>
{
    size_t operator()(dflat::ValueType const& t) const
    {
        return std::hash<dflat::TypeName>{}(t.name());
    }
};

} // namespace std
