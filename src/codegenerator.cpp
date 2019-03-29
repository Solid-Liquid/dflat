#include "codegenerator.hpp"

namespace dflat
{

String generateCode(Vector<ASNPtr> const& program)
{
    GenEnv env;
    for(size_t i = 0; i < program.size(); i++)
    {

          program[i]->codeGenerator(env);

    }
    return genEverything(env);
}


} //namespace dflat
