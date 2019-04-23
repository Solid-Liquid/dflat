#include "methodmeta.hpp"
#include "asn.hpp"

namespace dflat
{

CanonName const* MethodMetaMan::lookupCanonName(MethodExp const* exp) const
{
    return lookup(_canonNames, static_cast<void const*>(exp));
}
        
void MethodMetaMan::setCanonName(MethodExp const* exp, CanonName const& name)
{
    _canonNames.insert({ static_cast<void const*>(exp), name });
}

} // namespace dflat
