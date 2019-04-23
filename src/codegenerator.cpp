#include "codegenerator.hpp"

namespace dflat
{

String generateCode(Vector<ASNPtr> const& program, TypeEnv const& typeEnv)
{
    GenEnv env(typeEnv);
    for(size_t i = 0; i < program.size(); i++)
    {

          program[i]->generateCode(env);

    }

    return env.concat();
}

} //namespace dflat
