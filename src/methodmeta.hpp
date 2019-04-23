#pragma once

#include "string.hpp"
#include "map.hpp"
#include "canonname.hpp"

namespace dflat
{

// This is metadata for method calls, not methods in general.

struct MethodExp;

struct MethodMeta
{
    ValueType objectType;
    CanonName methodName;
};

class MethodMetaMan
{
    public:
        MethodMeta const* lookupMeta(MethodExp const*) const;
        void setMeta(MethodExp const*, MethodMeta);

    private:
        Map<void const*, MethodMeta> _canonNames;
};

} // namespace dflat
