#include "canonname.hpp"
#include <iostream>

namespace dflat
{

CanonName::CanonName(String name)
    : _name(std::move(name))
{}

String CanonName::toString() const
{
    return _name;
}

bool operator==(CanonName const& a, CanonName const& b)
{
    return a.toString() == b.toString();
}

bool operator!=(CanonName const& a, CanonName const& b)
{
    return !(a == b);
}

std::ostream& operator<<(std::ostream& s, CanonName const& x)
{
    return s << x.toString();
}

} // namespace dflat
