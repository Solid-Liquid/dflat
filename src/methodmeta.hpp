#pragma once

#include "string.hpp"
#include "map.hpp"
#include "canonname.hpp"

namespace dflat
{

struct MethodExp;

struct MethodMeta
{
    CanonName name;
};

class MethodMetaMan
{
    public:
        CanonName const* lookupCanonName(MethodExp const*) const;
        void setCanonName(MethodExp const*, CanonName const&);

    private:
        Map<void const*, CanonName> _canonNames;
};

} // namespace dflat
