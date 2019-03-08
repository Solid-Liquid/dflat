#pragma once

#include "result.hpp"
#include <unordered_set>
#include <utility>

namespace dflat
{

template <typename K>
using Set = std::unordered_set<K>;

template <typename K>
Optional<K> lookup(Set<K> const& set, K const& key)
{
    auto it = set.find(key);

    // Have to be explicit here. Optional<any> didn't work.
    if (it == set.end())
    {
        return Optional<K>(outcome::in_place_type_t<Fail>());
    }
    else
    {
        return Optional<K>(outcome::in_place_type_t<K>(), it->second);
    }
}

} //dflat
