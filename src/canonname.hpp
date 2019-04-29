#pragma once

#include "string.hpp"
#include "type.hpp"
#include <iosfwd>

namespace dflat
{

class CanonName
{
    public:
        String toString() const;

    private:
        CanonName(String);

        String _name;

    friend CanonName canonize(String);
};

bool operator==(CanonName const&, CanonName const&);
bool operator!=(CanonName const&, CanonName const&);
std::ostream& operator<<(std::ostream&, CanonName const&);

} // namespace dflat

namespace std
{

template <>
struct hash<dflat::CanonName>
{
    size_t operator()(dflat::CanonName const& x) const
    {
        return std::hash<dflat::String>{}(x.toString());
    }
};

} // namespace std
