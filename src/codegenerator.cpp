#include "codegenerator.hpp"

namespace dflat
{

String codeGenerator(Vector<ASNPtr> const& program)
{
    String ret = "";
    GenEnv env;
    for(size_t i = 0; i < program.size(); i++)
    {

          program[i]->generateCode(env);

    }
    return ret;
}

GenEnv initialGenEnv()
{
    //TODO
    GenEnv env;
    return env;
}


} //namespace dflat
