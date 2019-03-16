#pragma once

#include "optional.hpp"
#include <unordered_map>
#include <utility>

namespace dflat {

template <typename K, typename V>
using Map = std::unordered_map<K,V>;

template <typename K, typename V>
Optional<V> lookup(Map<K,V> const& map, K const& key) {
    auto it = map.find(key);

    if (it == map.end()) {
        return nullopt;
    }
    else {
        return it->second;
    }
}

} //dflat
