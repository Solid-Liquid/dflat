#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

static CodeVarName    codeVar(String x)     { return { std::move(x) }; }
static CodeMemberName codeMember(String x)  { return { std::move(x) }; }
static CodeMethodName codeMethod(String x)  { return { std::move(x) }; }
static CodeTypeName   codeType(String x)    { return { std::move(x) }; }
static CodeLiteral    codeLiteral(String x) { return { std::move(x) }; }
static CodeNumber     codeNumber(int x)     { return {x}; }

static CodeParent const codeParent;
static CodeTabs const   codeTabs;
static CodeTabIn const  codeTabIn;
static CodeTabOut const codeTabOut;

void scope_push(GenEnv& env)
{
    env.scopes.push_back({});
}

void scope_pop(GenEnv& env)
{
    env.scopes.pop_back();
}

void scope_decl(GenEnv& env, String s, Decl d)
{
    env.scopes.back().insert({s, d});
}

void scope_decl_method(GenEnv& env, String s, Type t)
{
    Decl d{ DeclType::method, t };
    scope_decl(env, s, d);
}

void scope_decl_local(GenEnv& env, String s, Type t)
{
    std::cout << "scope_decl_local(" << s << ", " << t.toString() << ")\n";
    Decl d{ DeclType::local, t };
    scope_decl(env, s, d);
}

void scope_print(GenEnv& env)
{
    std::cout << "scope_print\n";
    unsigned tab = 1;
    for (auto&& s : env.scopes)
    {
        for (auto&& [k,v] : s)
        {
            std::cout << String(tab*2, ' ') << k << "\n";
        }
        ++tab;
    }
}

Optional<Decl> scope_lookup(GenEnv& env, String s)
{
    auto it = env.scopes.rbegin();
    auto const end = env.scopes.rend();

    while (it != end)
    {
        if (Decl const* d = lookup(*it, s))
        {
            return *d;
        }

        ++it;
    }

    return nullopt;
}

static void emitMember(GenEnv& env, ValueType objectType, String member)
{
    int depth = env.classes.classHasMember(objectType, member);

    if (depth == 0)
    {
        throw std::logic_error("no member '" + member 
                + "' in '" + objectType.name() + "'");
    }

    while (depth > 1)
    {
        env << codeLiteral("->")
            << codeParent;
    }

    env << codeLiteral("->")
        << codeMember(member);
}

void emitObject(GenEnv& env, String object, String member)
{
    Optional<Decl> decl = scope_lookup(env, object);

    if (!decl)
    {
        throw std::logic_error("no object '" + object + "' in scope");
    }

    env << codeVar(object);
    emitMember(env, decl->type.value(), member);
}

void VariableExp::generateCode(GenEnv & env)
{
    //TODO referencing members of parent classes
    if (object)
    {
        emitObject(env, *object, name);
    }
    else
    {
        Optional<Decl> decl = scope_lookup(env, name);

        if (decl && decl->declType == DeclType::local)
        {
            // Local.
            env << codeVar(name);
        }
        else
        {
            // Must be using implicit this.
            emitObject(env, "this", name);
        }
    }
}

void NumberExp::generateCode(GenEnv & env)
{
    env << codeNumber(value);
}

void BoolExp::generateCode(GenEnv & env)
{
    env << codeNumber(value ? 1 : 0);
}

void BinopExp::generateCode(GenEnv & env)
{
    env << codeLiteral("(")
        << lhs
        << codeLiteral(opString(op))
        << rhs
        << codeLiteral(")");
}

void UnopExp::generateCode(GenEnv & env)
{
    env << codeLiteral("(") 
        << codeLiteral(opString(op))
        << rhs
        << codeLiteral(")");
}

void Block::generateCode(GenEnv & env)
{
    scope_push(env);

    env << codeTabs
        << codeLiteral("{\n")
        << codeTabIn;

    for (ASNPtr const& stmt : statements)
    {
        env << stmt;
    }

    env << codeTabOut
        << codeTabs
        << codeLiteral("}\n");

    scope_pop(env);
}

void IfStm::generateCode(GenEnv & env)
{
    env << codeTabs
        << codeLiteral("if (")
        << logicExp
        << codeLiteral(")\n")
        << trueStatements;

    if (hasFalse)
    {
        env << codeTabs
            << codeLiteral("else\n")
            << falseStatements;
    }
}

void WhileStm::generateCode(GenEnv & env)
{
    env << codeTabs
        << codeLiteral("while (")
        << logicExp
        << codeLiteral(")\n")
        << statements;
}

void MethodDef::generateCode(GenEnv & env)
{
    env.curFunc = name;
    ValueType curClass = env.classes.cur()->type;
    
//    scope_decl_method(name, *asnType);
    scope_push(env);
    
    env << codeTabs
        << codeType(retTypeName)
        << codeLiteral(" ")
        << codeMethod(name)
        << codeLiteral("(");
    
    env << codeType(curClass.name())
        << codeLiteral(" ")
        << codeVar("this");
        
    scope_decl_local(env, "this", curClass);

    for(auto&& ar : args)
    {
        env << codeLiteral(", ")
            << codeType(ar.typeName)
            << codeLiteral(" ")
            << codeVar(ar.name);
        
        scope_decl_local(env, ar.name, ValueType(ar.typeName));
    }

    env << codeLiteral(")\n")
        << statements;

    scope_pop(env);
    env.curFunc = nullopt;
}

void MethodExp::generateCode(GenEnv & env)
{
    // TODO: Scope resolution. Canonical name
    env << method
        << codeLiteral("(");

    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
        {
            env << codeLiteral(", ")
                << ar;
        }
        
        ar->generateCode(env);
        ++track;
    }
   
    env << codeLiteral(")");
    //TODO: Possible set env.curFunc to null
}

void MethodStm::generateCode(GenEnv & env)
{
    env << codeTabs
        << methodExp
        << codeLiteral(";\n");
}

void NewExp::generateCode(GenEnv&)
{
    // TODO: everything.
    //  <type>* <name> = (<type>*)malloc(sizeof(<type>));
    //  theoretically call constructor
    //
//        String typeName;
//        Vector<ASNPtr> args;
}

void AssignStm::generateCode(GenEnv& env)
{
    // TODO: check if this works for every case (i.e. rhs is new stm)
    env << codeTabs
        << lhs
        << codeLiteral(" = ")
        << rhs
        << codeLiteral(";\n");
}

void VarDecStm::generateCode(GenEnv& env)
{
    //TODO: cannonical names
    env << codeLiteral("\t"); // Hardcoding member vars to 1 tab.

    //typename could be cannonical name of class
    if (typeName == "bool")
    {
        env << codeType("int");
    }
    else
    {
        env << codeType(typeName);
    }
    
    env << codeLiteral(" ")
        << codeVar(name)
        << codeLiteral(" = ")
        << value
        << codeLiteral(";\n");

    if (env.curFunc)
    {
        scope_decl_local(env, name, ValueType(typeName)); 
    }
    else
    {
        env.classes.cur()->members.insert({ name, ValueType(typeName) });
    }
}

void RetStm::generateCode(GenEnv& env)
{
    env << codeTabs
        << codeLiteral("return ")
        << value
        << codeLiteral(";\n");
}

void ClassDecl::generateCode(GenEnv& env)
{
    env.classes.enter(ValueType(name));
                       
    env << codeLiteral("struct ")
        << codeType(name)
        << codeLiteral("\n{\n");

    if(parent) 
    {
        env.classes.cur()->parent = 
            env.classes.lookup(ValueType(parent->name));

        env << codeLiteral("\tstruct ") // Hardcoded to 1 tab
            << codeType(parent->name)
            << codeLiteral(" ")
            << codeParent
            << codeLiteral(";\n");
    }

    for (ASNPtr& member : members)
    {
        env << member;
    }

    env << codeLiteral("};\n");
    env.classes.leave();
}

} //namespace dflat
