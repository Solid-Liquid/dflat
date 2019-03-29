#include "codegenerator.hpp"

namespace dflat
{

String codeGenerator(Vector<ASNPtr> const& program)
{
    String ret = "";
    GenEnv env;
    for(size_t i = 0; i < program.size(); i++)
    {

          ret += program[i]->generateCode(env);

    }
    return ret;
}


} //namespace dflat
