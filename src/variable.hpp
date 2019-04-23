#pragma once

#include "string.hpp"
#include "optional.hpp"

namespace dflat
{

struct Variable
{
    Optional<String> object;
    String variable;

    Variable(Optional<String> object, String variable);

    String toString() const;
};

bool operator==(Variable const&, Variable const&);
bool operator!=(Variable const&, Variable const&);

} // namespace dflat
