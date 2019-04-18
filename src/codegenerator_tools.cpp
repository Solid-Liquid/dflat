#include "codegenerator_tools.hpp"
#include "asn.hpp"

namespace dflat
{

static String const prolog = R"(
#define $TYPE(x)   dftype_##x
#define $VAR(x)    dfvar_##x
#define $MEMBER(x) dfmember_##x
#define $PARENT()  dfparent
#define $METHOD(x) dfmethod_##x

)";

static String const typePrefix   = "$TYPE(";
static String const varPrefix    = "$VAR(";
static String const memberPrefix = "$MEMBER(";
static String const methodPrefix = "$METHOD(";
static String const parent       = "$PARENT";
static String const suffix       = ")";


std::stringstream& GenEnv::write()
{
    if (!classes.cur()) 
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

String GenEnv::prolog() const
{
    return dflat::prolog;
}

String GenEnv::concat() const
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
    return typePrefix + x + suffix;
}

String GenEnv::mangleVarName(String const& x)
{
    return varPrefix + x + suffix;
}

String GenEnv::mangleMemberName(String const& x)
{
    return memberPrefix + x + suffix;
}

String GenEnv::mangleMethodName(String const& x)
{
    return methodPrefix + x + suffix;
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

GenEnv& GenEnv::operator<<(CodeMemberName const& x)
{
    write() << mangleMemberName(x.value);
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
    write() << parent;
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
