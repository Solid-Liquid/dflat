#include "codegenerator.hpp"

namespace dflat
{

String generateCode(Vector<ASNPtr> const& program, TypeEnv const& typeEnv)
{
    GenEnv env(typeEnv);

    for (ASNPtr const& node : program)
    {
        node->generateCode(env);
    }

    return env.prolog()
         + env.concat()
         + env.epilog();
}

} //namespace dflat
