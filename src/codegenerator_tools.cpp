#include "codegenerator_tools.hpp"
#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

String GenEnv::prolog() const
{
/*
    NEW(T,R,V,C,...)
        Constructs a T.
        R is the type to cast to (different from T when polymorphic).
        V is the vtable function for T.
        C is the constructor to call.
        ... are the extra arguments to pass to C.

    NEW0(T,R,V,C)
        Same but with no extra arguments to C (default constructor).

    VTABLE(x)
        Gets the vtable for object x.

    FIRST_ARG(x,...)
        Returns the first of variadic arguments.

    CALL(R,f,...)
        Calls the function f.
        R is the return type.
        ... are the arguments to pass to F, including "this".

    vtablefn
        Typedef for a function pointer to a vtable function.

    vtable
        Contains a df_vtablefn.
        Every root class has this as its first member.
        As a result, any object can be trivially cast to access its vtable.

    dfalloc(size_t, vtablefn)
        Allocates a class object of given size and assigns the given vtable to it.
        It zeroes the object's memory so that unassigned members are at least predictable.

*/
    String s = R"(
#include <stdlib.h>

#define NEW(T,V,C,...) C(dfalloc(sizeof(struct T), (vtablefn)&V), __VA_ARGS__)
#define NEW0(T,V,C)    C(dfalloc(sizeof(struct T), (vtablefn)&V))
#define VTABLE(x)        (((struct vtable*)x)->vt)
#define FIRST_ARG(x,...) x
#define CALL(R,f,...)    ( ( (R(*)(void*)) ( (*VTABLE(FIRST_ARG(__VA_ARGS__))) (f) ) ) (__VA_ARGS__) )

enum Methods
{
)";

    for (CanonName const& method : getAllMethods())
    {
        s += "\t" + mangleVTableMethodName(method) + ",\n";
    }

    s += "};\n";

s += R"(
typedef void* (*vtablefn)(enum Methods);

struct vtable
{
    vtablefn vt;
};

void* dfalloc(size_t size, vtablefn vt)
{
    void* p = calloc(1, size);
    VTABLE(p) = vt;
    return p;
}

)";

    // Emit vtable headers.
    for (auto [classType, meta] : _classes.allClasses())
    {
        (void)meta; // unused

        s = s
          + "void* "
          + mangleVTableName(classType)
          + "(enum Methods);\n";
    }

    s += "\n";
    return s;
}

static
String vtable(GenEnv const& env, ValueType const& classType)
{
    String s;
    
    s = s
      + "void* "
      + mangleVTableName(classType)
      + "(enum Methods m)\n"
      + "{\n"
      + "\tswitch (m)\n"
      + "\t{\n";

    for (CanonName const& methodName : env.getClassMethods(classType))
    {
        s = s
          + "\t\tcase "
          + mangleVTableMethodName(methodName)
          + ": return &"
          + mangleMethodName(classType, methodName)
          + ";\n";
    }
    
    s = s
      + "\t\tdefault: abort();\n"
      + "\t}\n"
      + "}\n\n";

    return s;
}

String GenEnv::epilog() const
{
    String s;

    for (auto [type, meta] : _classes.allClasses())
    {
        (void)meta; // unused
        s += vtable(*this, type);
    }

    ValueType mainClassType("Main");
    String mainClassName("main");

    MethodType mainMethodType(voidType, {});
    CanonName mainMethodName("main", mainMethodType);
    MethodType mainConsType(mainClassType, {});
    CanonName mainConsName(config::consName, mainConsType); 

    s = s
      + "int main()\n"
      + "{\n"
      + "\t" 
      + mangleTypeName(mainClassType) 
      + " "
      + mangleVarName(mainClassName)
      + " = NEW0("
      + mangleClassDecl(mainClassType)
      + ", "
      + mangleVTableName(mainClassType)
      + ", "
      + mangleConsName(mainConsName)
      + ");\n"
      + "\t"
      + mangleMethodName(mainClassType, mainMethodName)
      + "("
      + mangleVarName(mainClassName)
      + ");\n"
      + "}\n";

    return s;
}

String mangleTypeName(ValueType const& typeName)
{
    if (isBuiltinType(typeName))
    {
        return translateBuiltinType(typeName);
    }
    else
    {
        return "struct df_" + typeName.toString() + "*";
    }
}

String mangleClassDecl(ValueType const& x)
{
    return "df_" + x.toString();
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

String mangleVTableName(ValueType const& classType)
{
    return "dfv_" + classType.toString();
}

String mangleVTableMethodName(CanonName const& methodName)
{
    String s = "dfvm_" + methodName.baseName();

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

    if (_classTabs != 0)
    {
        throw std::logic_error("Incorrect tabs on leaving class");
    }
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
    
    if (_methodTabs != 0)
    {
        throw std::logic_error("Incorrect tabs on leaving method");
    }
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

Set<CanonName> GenEnv::getClassMethods(ValueType const& classType) const
{
    Set<CanonName> methods;
    ClassMeta const* meta = _classes.lookup(classType);

    if (!meta)
    {
        throw std::logic_error("No class of type '" + classType.toString() + "'");
    }

    while (true)
    {
        for (CanonName const& methodName : meta->methods)
        {
            methods.insert(methodName);
        }

        if (!meta->parent)
        {
            break;
        }

        meta = _classes.lookup(*meta->parent);

        if (!meta)
        {
            throw std::logic_error("No class of type '" + meta->parent->toString() + "'");
        }
    }

    return methods;
}

Set<CanonName> GenEnv::getAllMethods() const
{
    Set<CanonName> methods;

    for (auto [type, meta] : _classes.allClasses())
    {
        (void)type; // unused

        for (CanonName const& methodName : meta.methods)
        {
            methods.insert(methodName);
        }
    }

    return methods;
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
    write() << mangleTypeName(x.type);
    return *this;
}

GenEnv& GenEnv::operator<<(CodeClassDecl const& x)
{
    write() << mangleClassDecl(x.type);
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

GenEnv& GenEnv::operator<<(CodeVTableName const& x)
{
    write() << mangleVTableName(x.classType);
    return *this;
}

GenEnv& GenEnv::operator<<(CodeVTableMethodName const& x)
{
    write() << mangleVTableMethodName(x.methodName);
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
    write() << "parent";
    return *this;
}

GenEnv& GenEnv::operator<<(CodeTabs const&)
{
    if (_curMethod)
    {
        write() << String(_methodTabs, '\t');
    }
    else
    {
        write() << String(_classTabs, '\t');
    }
    
    return *this;
}

GenEnv& GenEnv::operator<<(CodeTabIn const&)
{
    if (_curMethod)
    {
        ++_methodTabs;
    }
    else
    {
        ++_classTabs;
    }
    
    return *this;
}

GenEnv& GenEnv::operator<<(CodeTabOut const&)
{
    if (_curMethod)
    {
        if (_methodTabs == 0)
        {
            throw std::logic_error("tabbed below zero");
        }
    
        --_methodTabs;
    }
    else
    {
        if (_classTabs == 0)
        {
            throw std::logic_error("tabbed below zero");
        }
    
        --_classTabs;
    }

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
