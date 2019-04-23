#include "codegenerator_tools.hpp"
#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

String codeProlog()
{
    return R"(
#include <stdlib.h>

#define DF_NEW(T) (struct T*)malloc(sizeof(struct T))

)";

}

String codeEpilog()
{
    ValueType mainClassType("Main");
    String mainClassName("main");

    MethodType mainMethodType(voidType, {});
    CanonName mainMethodName("main", mainMethodType);

    return String{}
         + "int main()\n"
         + "{\n"
         + "\t" 
         + mangleTypeName(mainClassType.toString()) 
         + " "
         + mangleVarName(mainClassName)
         + " = DF_NEW("
         + mangleClassDecl(mainClassType.toString())
         + ");\n"
         + "\t"
         + mangleMethodName(mainClassType, mainMethodName)
         + "("
         + mangleVarName(mainClassName)
         + ");\n"
         + "}\n";
}

String mangleTypeName(String const& x)
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

String mangleClassDecl(String const& x)
{
    return "df_" + x;
}

String mangleVarName(String const& x)
{
    return "df_" + x;
}

String mangleMemberName(String const& x)
{
    return "df_" + x;
}

String mangleMethodName(ValueType const& objectName, CanonName const& methodName)
{
    String s = "dfm_" + objectName.toString() + "_" + methodName.baseName();

    for (ValueType const& arg : methodName.type().args())
    {
        s += "_" + arg.toString();
    }

    return s;
}


GenEnv::GenEnv(TypeEnv const& typeEnv)
    : _classes(typeEnv._classes)
    , _methods(typeEnv._methods)
{}

std::stringstream& GenEnv::write()
{
    if (!inMethod()) 
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
    return _structDef.str()
         + "\n"
         + _funcDef.str();
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
    _curMethod = MethodMeta{ curClass().type, methodName };
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
        
MethodMeta const& GenEnv::getMethodMeta(MethodExp const* methodExp) const
{
    MethodMeta const* method = _methods.lookupMeta(methodExp);

    if (!method)
    {
        throw std::logic_error("No method meta for '" + methodExp->toString() + "'");
    }

    return *method;
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

ValueType const& GenEnv::getLocalType(String const& name) const
{
    Decl const* decl = _scopes.lookup(name);

    if (!decl)
    {
        throw std::logic_error("no local with name '" + name + "'");
    }

    if (decl->declType != DeclType::local)
    {
        throw std::logic_error("decl '" + name + "' is not a local");
    }

    if (!decl->type.isValue())
    {
        throw std::logic_error("decl '" + name + "' is not a value type");
    }

    return decl->type.value();
}

ValueType const* GenEnv::lookupLocalType(String const& name) const
{
    Decl const* decl = _scopes.lookup(name);

    if (!decl)
    {
        return nullptr;
    }

    if (decl->declType != DeclType::local)
    {
        return nullptr;
    }

    if (!decl->type.isValue())
    {
        throw std::logic_error("decl '" + name + "' is not a value type");
    }

    return &decl->type.value();
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
    write() << mangleMethodName(x.objectType, x.methodName);
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
    Decl const* decl = _scopes.lookup(objectName);

    if (!decl)
    {
        throw std::logic_error("no object '" + objectName + "' in scope");
    }

    *this << CodeVarName(objectName);
    emitMemberVar(decl->type.value(), memberName);
}

} // namespace dflat
