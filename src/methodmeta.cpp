#include "methodmeta.hpp"
#include "asn.hpp"

namespace dflat
{

MethodMeta const* MethodMetaMan::lookupMeta(MethodExp const* exp) const
{
    return lookup(_canonNames, static_cast<void const*>(exp));
}
        
void MethodMetaMan::setMeta(MethodExp const* exp, MethodMeta meta)
{
    _canonNames.insert({ static_cast<void const*>(exp), std::move(meta) });
}

} // namespace dflat
