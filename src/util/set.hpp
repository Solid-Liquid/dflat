#pragma once

#include "optional.hpp"
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

    if (it == set.end())
    {
        return nullopt;
    }
    else
    {
        return *it;
    }
}

} //dflat
