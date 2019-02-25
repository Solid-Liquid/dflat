#pragma once

#include "token.hpp"
#include "vector.hpp"

// Convenience function for making Token vectors to test against.
template <typename... Ts>
dflat::Vector<dflat::TokenPtr> tokens(Ts&&... in)
{
    dflat::Vector<dflat::TokenPtr> out;
    (out.push_back(std::make_unique<Ts>(in)), ...);
    return out;
}
