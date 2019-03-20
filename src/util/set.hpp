#pragma once

#include <unordered_set>
#include <utility>

namespace dflat
{

template <typename K>
using Set = std::unordered_set<K>;

template <typename K>
K const* lookup(Set<K> const& set, K const& key)
{
    auto it = set.find(key);

    if (it == set.end())
    {
        return nullptr;
    }
    else
    {
        return &*it;
    }
}

} //dflat
