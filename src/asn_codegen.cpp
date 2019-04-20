#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

static CodeVarName    codeVar(String x)     { return { std::move(x) }; }
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
    //TODO whose variable? local? member?
    if (object)
    {
        env << codeVar(*object) 
            << codeLiteral("->");
    }
    
    env << codeVar(name);
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
    
    //TODO: check scope and use connonical/overloaded name.
    //possible TODO: prototype.
    env << codeTabs
        << codeType(retTypeName)
        << codeLiteral(" ")
        << codeMethod(name)
        << codeLiteral("(");
    
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
        {
            env << codeLiteral(", ");
        }
       
        env << codeType(ar.typeName)
            << codeLiteral(" ")
            << codeVar(ar.name);
        
        ++track;
    }

    env << codeLiteral(")\n")
        << statements;

    env.curFunc = nullopt;
}

void MethodExp::generateCode(GenEnv & env)
{
    // TODO: Scope resolution. Cannonical name
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
                       
    env.curClass = name;
    env << codeLiteral("struct ")
        << codeType(name)
        << codeLiteral("\n{\n)");

    if(parent) 
    {
        env << codeLiteral("\tstruct ") // Hardcoded to 1 tab
            << codeType(parent->name)
            << codeLiteral(" ")
            << codeParent;
    }

    for (ASNPtr& member : members)
    {
        env << member;
    }

    env << codeLiteral("};\n");
    env.curClass = nullopt;    
}

} //namespace dflat
