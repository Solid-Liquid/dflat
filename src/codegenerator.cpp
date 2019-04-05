#include "codegenerator.hpp"

namespace dflat
{

String codeGenerator(Vector<ASNPtr> const& program)
{
    GenEnv env;
    for(size_t i = 0; i < program.size(); i++)
    {

          program[i]->generateCode(env);

    }
    return env.concat();
}

GenEnv initialGenEnv()
{
    //TODO
    GenEnv env;
    return env;
}


} //namespace dflat
