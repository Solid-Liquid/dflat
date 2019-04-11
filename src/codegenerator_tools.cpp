#include "codegenerator_tools.hpp"
#include "asn.hpp"

namespace dflat
{

static String const dfPrefix     = "";
static String const typePrefix   = "dfType_";
static String const varPrefix    = "dfVar_";
static String const methodPrefix = "dfMethod_";
static String const parentPrefix = "dfParent_";


std::stringstream& GenEnv::write()
{
    if(!curClass) 
    {
        return _main;
    } 
    else if (!curFunc) 
    {
        return _structDef;
    } 
    else 
    {
        return _funcDef;
    }
}

String GenEnv::concat()
{
    String s;
    String const& sStruct = _structDef.str();
    String const& sFunc   = _funcDef.str();
    String const& sMain   = _main.str();

    s += sStruct;

    if (!sStruct.empty())
    {
        s += "\n";
    }

    s += sFunc;

    if (!sFunc.empty())
    {
        s += "\n";
    }

    s += sMain;
    return s;
}

String GenEnv::mangleTypeName(String const& x)
{
    return dfPrefix + typePrefix + x;
}

String GenEnv::mangleVarName(String const& x)
{
    return dfPrefix + varPrefix + x;
}

String GenEnv::mangleMethodName(String const& x)
{
    return dfPrefix + methodPrefix + x;
}

GenEnv& GenEnv::operator<<(CodeTypeName const& x)
{
    write() << mangleTypeName(x.value);
    return *this;
}

GenEnv& GenEnv::operator<<(CodeVarName const& x)
{
    write() << mangleVarName(x.value);
    return *this;
}

GenEnv& GenEnv::operator<<(CodeMethodName const& x)
{
    write() << mangleMethodName(x.value);
    return *this;
}

GenEnv& GenEnv::operator<<(CodeLiteral const& x)
{
    write() << x.value;
    return *this;
}

GenEnv& GenEnv::operator<<(CodeNumber const& x)
{
    write() << std::to_string(x.value);
    return *this;
}

GenEnv& GenEnv::operator<<(CodeParent const&)
{
    write() << dfPrefix << parentPrefix;
    return *this;
}

GenEnv& GenEnv::operator<<(CodeTabs const&)
{
    write() << String(_tabs, '\t');
    return *this;
}

GenEnv& GenEnv::operator<<(CodeTabIn const&)
{
    ++_tabs;
    return *this;
}

GenEnv& GenEnv::operator<<(CodeTabOut const&)
{
    if (_tabs == 0)
    {
        throw std::logic_error("tabbed below zero");
    }

    --_tabs;
    return *this;
}

GenEnv& GenEnv::operator<<(ASNPtr const& x)
{
    x->generateCode(*this);
    return *this;
}


} // namespace dflat
