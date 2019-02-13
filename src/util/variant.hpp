#pragma once

#include "mapbox/variant.hpp"

namespace dflat {

template <typename... Ts>
using Variant = mapbox::util::variant<Ts...>;

template <typename T, typename... Fs>
void match_each(T&& ts, Fs&&... fs) {
    for (auto&& t : ts) {
        t.match(std::forward<Fs>(fs)...);
    }
}

using mapbox::util::apply_visitor;

} //dflat
