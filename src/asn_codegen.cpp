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

void VariableExp::generateCode(GenEnv & env)
{
    //TODO referencing members of parent classes
    if (object)
    {
        env.emitObject(*object, name);
    }
    else
    {
        Optional<Decl> decl = env.scopes.lookup(name);

        if (decl && decl->declType == DeclType::local)
        {
            // Local.
            env << codeVar(name);
        }
        else
        {
            // Must be using implicit this.
            env.emitObject("this", name);
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
    env.scopes.push();

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

    env.scopes.pop();
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
    env.scopes.push();
    
    env << codeTabs
        << codeType(retTypeName)
        << codeLiteral(" ")
        << codeMethod(name)
        << codeLiteral("(");
    
    env << codeType(curClass.name())
        << codeLiteral(" ")
        << codeVar("this");
        
    env.scopes.declLocal("this", curClass);

    for(auto&& ar : args)
    {
        env << codeLiteral(", ")
            << codeType(ar.typeName)
            << codeLiteral(" ")
            << codeVar(ar.name);
        
        env.scopes.declLocal(ar.name, ValueType(ar.typeName));
    }

    env << codeLiteral(")\n")
        << statements;

    env.scopes.pop();
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
    //TODO: use env to determine if there should be a tab or not
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
        << codeLiteral(";\n");

    if (!env.classes.cur())
    {
        // TODO needed for testing?
        env.scopes.declLocal(name, ValueType(typeName));
    }
    else if (env.curFunc)
    {
        env.scopes.declLocal(name, ValueType(typeName));
    }
    else
    {
        env.classes.addMember(name, ValueType(typeName));
    }
}

void VarDecAssignStm::generateCode(GenEnv& env)
{
    //TODO: cannonical names
    //env << codeLiteral("\t"); // Hardcoding member vars to 1 tab.

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

    if (!env.classes.cur())
    {
        // TODO needed for testing?
        env.scopes.declLocal(name, ValueType(typeName)); 
    }
    else if (env.curFunc)
    {
        env.scopes.declLocal(name, ValueType(typeName)); 
    }
    else
    {
        env.classes.addMember(name, ValueType(typeName));
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
        env.classes.setParent(ValueType(parent->name));

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
