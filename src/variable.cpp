#include "variable.hpp"

namespace dflat
{

Variable::Variable(Optional<String> _object, String _variable)
    : object(std::move(_object))
    , variable(std::move(_variable))
{}

String Variable::toString() const
{
    String s;
    
    if (object)
    {
        s = *object + ".";
    }

    s += variable;
    return s;
}

bool operator==(Variable const& a, Variable const& b)
{
    return a.object   == b.object
        && a.variable == b.variable;
}

bool operator!=(Variable const& a, Variable const& b)
{
    return !(a == b);
}

} // namespace dflat
