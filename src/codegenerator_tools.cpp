#include "codegenerator_tools.hpp"
#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

String codeProlog()
{
/*
    DF_NEW(T,R,V,C,...)
        Constructs a T.
        R is the type to cast to (different from T when polymorphic).
        V is the vtable function for T.
        C is the constructor to call.
        ... are the extra arguments to pass to C.

    DF_NEW0(T,R,V,C)
        Same but with no extra arguments to C (default constructor).

    DF_VTABLE(x)
        Gets the vtable for object x.

    DF_FIRST_ARG(x,...)
        Returns the first of variadic arguments.

    DF_CALL(R,f,...)
        Calls the function f.
        R is the return type.
        ... are the arguments to pass to F, including "this".

    DF_vtablefn
        Typedef for a function pointer to a vtable function.

    DF_vtable
        Contains a df_vtablefn.
        Every root class has this as its first member.
        As a result, any object can be trivially cast to access its vtable.

    DF_alloc(size_t, DF_vtablefn)
        Allocates a class object of given size and assigns the given vtable to it.
        It zeroes the object's memory so that unassigned members are at least predictable.

*/
    return R"(
#include <stdlib.h>

#define DF_NEW(T,R,V,C,...) C((struct R*)dfalloc(sizeof(struct T), (df_vtablefn)&V), __VA_ARGS__)
#define DF_NEW0(T,R,VC)     C((struct R*)dfalloc(sizeof(struct T), (df_vtablefn)&V))
#define DF_VTABLE(x) (((struct df_vtable*)x)->vt)
#define DF_FIRST_ARG(x,...) x
#define DF_CALL(R,f,...) (( (R(*)(void*)) ((*DF_VTABLE(DF_FIRST_ARG(__VA_ARGS__)))(f))) (__VA_ARGS__))

typedef void* (*DF_vtablefn)(enum Fns);

struct DF_vtable
{
    DF_vtablefn vt;
};

void* DF_alloc(size_t size, DF_vtablefn vt)
{
    void* p = calloc(1, size);
    DF_VTABLE(p) = vt;
    return p;
}
)";

}

String codeEpilog()
{
    ValueType mainClassType("Main");
    String mainClassName("main");

    MethodType mainMethodType(voidType, {});
    CanonName mainMethodName("main", mainMethodType);
    MethodType mainConsType(mainClassType, {});
    CanonName mainConsName(config::consName, mainConsType); 

    return String{}
         + "int main()\n"
         + "{\n"
         + "\t" 
         + mangleTypeName(mainClassType.toString()) 
         + " "
         + mangleVarName(mainClassName)
         + " = DF_NEW0("
         + mangleClassDecl(mainClassType.toString())
         + ", "
         + mangleConsName(mainConsName)
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

String mangleMethodName(ValueType const& objectType, 
        CanonName const& methodName)
{
    String s = "dfm_" + objectType.toString() + "_" + methodName.baseName();

    for (ValueType const& arg : methodName.type().args())
    {
        s += "_" + arg.toString();
    }

    return s;
}

String mangleConsName(CanonName const& consName)
{
    ValueType const objectType = consName.type().ret();
    String s = "dfc_" + objectType.toString();

    for (ValueType const& arg : consName.type().args())
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
        
MethodMeta const& GenEnv::getMethodMeta(ASN const* node) const
{
    MethodMeta const* method = _methods.lookupMeta(node);

    if (!method)
    {
        throw std::logic_error("No method meta for '" + node->toString() + "'");
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

GenEnv& GenEnv::operator<<(CodeConsName const& x)
{
    write() << mangleConsName(x.consName);
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

GenEnv& GenEnv::operator<<(BlockPtr const& x)
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
    
    *this << CodeLiteral("->");

    for (int i = 1; i < member->depth; ++i)
    {
        *this << CodeParent()
              << CodeLiteral(".");
    }

    *this << CodeMemberName{memberName};
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
