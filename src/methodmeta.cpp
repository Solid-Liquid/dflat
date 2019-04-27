#include "methodmeta.hpp"
#include "asn.hpp"

namespace dflat
{

MethodMeta const* MethodMetaMan::lookupMeta(ASN const* node) const
{
    return lookup(_canonNames, node);
}
        
void MethodMetaMan::setMeta(ASN const* node, MethodMeta meta)
{
    _canonNames.insert({ node, std::move(meta) });
}

} // namespace dflat
