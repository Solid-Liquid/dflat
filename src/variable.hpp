#pragma once

#include "string.hpp"
#include "optional.hpp"
#include "type.hpp"

namespace dflat
{

struct Variable
{
    Optional<String> object;
    String variable;

    Variable(Optional<String> object, String variable);

    String toString() const;
};

struct FormalArg
{
    Type   type;
    String name;
};

bool operator ==(FormalArg const&, FormalArg const&);
bool operator==(Variable const&, Variable const&);
bool operator!=(Variable const&, Variable const&);

} // namespace dflat
