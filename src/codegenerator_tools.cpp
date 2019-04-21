#include "codegenerator_tools.hpp"
#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

String const codeProlog = R"(
#define PLACEHOLDER false
)";

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
    ValueType typeName(x);

    if (isBuiltinType(typeName))
    {
        return translateBuiltinType(typeName);
    }
    else
    {
        return "struct df_" + x + "*";
    }
}

String GenEnv::mangleClassDecl(String const& x)
{
    return "df_" + x;
}

String GenEnv::mangleVarName(String const& x)
{
    return "df_" + x;
}

String GenEnv::mangleMemberName(String const& x)
{
    return "df_" + x;
}

String GenEnv::mangleMethodName(String const& x)
{
    return "df_" + x;
}

GenEnv& GenEnv::operator<<(CodeTypeName const& x)
{
    write() << mangleTypeName(x.value);
    return *this;
}

GenEnv& GenEnv::operator<<(CodeClassDecl const& x)
{
    write() << mangleClassDecl(x.value);
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
    write() << "dfparent";
    return *this;
}

GenEnv& GenEnv::operator<<(CodeClassTabs const&)
{
    write() << '\t';
    return *this;
}

GenEnv& GenEnv::operator<<(CodeMethodTabs const&)
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

void GenEnv::emitMember(ValueType const& objectType, String const& memberName)
{
    int depth = classes.classHasMember(objectType, memberName);

    if (depth == 0)
    {
        throw std::logic_error("no member '" + memberName 
                + "' in '" + objectType.name() + "'");
    }

    while (depth > 1)
    {
        *this << CodeLiteral{"->"}
              << CodeParent{};
    }

    *this << CodeLiteral{"->"}
          << CodeMemberName{memberName};
}

void GenEnv::emitObject(String const& objectName, String const& memberName)
{
    Optional<Decl> decl = scopes.lookup(objectName);

    if (!decl)
    {
        throw std::logic_error("no object '" + objectName + "' in scope");
    }

    *this << CodeVarName{objectName};
    emitMember(decl->type.value(), memberName);
}

} // namespace dflat
