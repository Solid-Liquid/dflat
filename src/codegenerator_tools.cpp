#include "codegenerator_tools.hpp"
#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

String const codeProlog = R"(
#define PLACEHOLDER false
)";

GenEnv::GenEnv(TypeEnv const& typeEnv)
    : _classes(typeEnv._classes)
{}

std::stringstream& GenEnv::write()
{
    if (!inClass()) 
    {
        return _main;
    } 
    else if (!inMethod()) 
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
        
void GenEnv::enterClass(ValueType const& classType)
{
    _classes.enter(classType);
}

void GenEnv::leaveClass()
{
    _classes.leave();
}

bool GenEnv::inClass() const
{
    return _classes.cur() != nullptr;
}

ClassMeta const& GenEnv::curClass() const
{
    if (!_classes.cur())
    {
        throw std::logic_error("no curClass");
    }

    return *_classes.cur();
}

void GenEnv::enterMethod(CanonName const& methodName)
{
    _curMethod = MethodMeta{methodName};
    _scopes.push(); // Argument scope.
    _scopes.declLocal(config::thisName, curClass().type);
}

void GenEnv::leaveMethod()
{
    _scopes.pop();
    _curMethod = nullopt;
}

bool GenEnv::inMethod() const
{
    return _curMethod != nullopt;
}

MethodMeta const& GenEnv::curMethod() const
{
    if (!_curMethod)
    {
        throw std::logic_error("no curMethod");
    }

    return *_curMethod;
}
        
CanonName const& GenEnv::getCanonName(MethodExp const* methodExp) const
{
    CanonName const* name = _methods.lookupCanonName(methodExp);

    if (!name)
    {
        throw std::logic_error("No canon name!");
    }

    return *name;
}

void GenEnv::enterScope()
{
    _scopes.push();
}

void GenEnv::leaveScope()
{
    _scopes.pop();
}

void GenEnv::declareLocal(String const& name, ValueType const& type)
{
    if (!_curMethod)
    {
        throw std::logic_error("declareLocal with no curMethod");
    }

    _scopes.declLocal(name, type);
}

Optional<Decl> GenEnv::lookupDecl(String const& name) const
{
    return _scopes.lookup(name);
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

String GenEnv::mangleMethodName(CanonName const& name)
{
    String s = "dfm_" + name.baseName();

    for (ValueType const& arg : name.type().args())
    {
        s += "_" + arg.toString();
    }

    return s;
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

GenEnv& GenEnv::operator<<(CodeTabs const&)
{
    if (_curMethod)
    {
        write() << String(_tabs, '\t');
    }
    else
    {
        write() << '\t';
    }
    
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

void GenEnv::emitMemberVar(ValueType const& objectType, String const& memberName)
{
    Optional<MemberMeta> const member = _classes.lookupVar(objectType, memberName);

    if (!member)
    {
        throw std::logic_error("no member var '" + memberName 
                + "' in '" + objectType.name() + "'");
    }
    
    for (int i = 1; i < member->depth; ++i)
    {
        *this << CodeLiteral("->")
              << CodeParent();
    }

    *this << CodeLiteral("->")
          << CodeMemberName{memberName};
}

void GenEnv::emitObject(String const& objectName, String const& memberName)
{
    Optional<Decl> decl = _scopes.lookup(objectName);

    if (!decl)
    {
        throw std::logic_error("no object '" + objectName + "' in scope");
    }

    *this << CodeVarName(objectName);
    emitMemberVar(decl->type.value(), memberName);
}

} // namespace dflat
