#pragma once

#include "token.hpp"
#include "vector.hpp"
#include "asn.hpp"
#include "parser.hpp"

using namespace dflat;

// Convenience function for making Token vectors to test against.
template <typename... Ts>
dflat::Vector<dflat::TokenPtr> tokens(Ts&&... in)
{
    dflat::Vector<dflat::TokenPtr> out;
    (out.push_back(std::make_unique<Ts>(in)), ...);
    return out;
}

// Convenience function for making ASN trees to test against.
template <typename T>
ASNPtr operator~(T&& t)
{
    return std::make_unique<T>(std::forward<T>(t));
}

template <typename... Ts>
Vector<ASNPtr> asns(Ts&&... in)
{
    Vector<ASNPtr> out;
    (out.push_back(std::make_unique<Ts>(in)), ...);
    return out;
}

