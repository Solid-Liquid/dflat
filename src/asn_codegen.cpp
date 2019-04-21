#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

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
            env << CodeVarName(name);
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
    env << CodeNumber(value);
}

void BoolExp::generateCode(GenEnv & env)
{
    env << CodeNumber(value ? 1 : 0);
}

void BinopExp::generateCode(GenEnv & env)
{
    env << CodeLiteral("(")
        << lhs
        << CodeLiteral(opString(op))
        << rhs
        << CodeLiteral(")");
}

void UnopExp::generateCode(GenEnv & env)
{
    env << CodeLiteral("(") 
        << CodeLiteral(opString(op))
        << rhs
        << CodeLiteral(")");
}

void Block::generateCode(GenEnv & env)
{
    env.scopes.push();

    env << CodeMethodTabs()
        << CodeLiteral("{\n")
        << CodeTabIn();

    for (ASNPtr const& stmt : statements)
    {
        env << stmt;
    }

    env << CodeTabOut()
        << CodeMethodTabs()
        << CodeLiteral("}\n");

    env.scopes.pop();
}

void IfStm::generateCode(GenEnv & env)
{
    env << CodeMethodTabs()
        << CodeLiteral("if (")
        << logicExp
        << CodeLiteral(")\n")
        << trueStatements;

    if (hasFalse)
    {
        env << CodeMethodTabs()
            << CodeLiteral("else\n")
            << falseStatements;
    }
}

void WhileStm::generateCode(GenEnv & env)
{
    env << CodeMethodTabs()
        << CodeLiteral("while (")
        << logicExp
        << CodeLiteral(")\n")
        << statements;
}

void MethodDef::generateCode(GenEnv & env)
{
    env.curFunc = name;
    ValueType curClass = env.classes.cur()->type;
    
//    scope_decl_method(name, *asnType);
    env.scopes.push();
    
    env << CodeMethodTabs()
        << CodeTypeName(retTypeName)
        << CodeLiteral(" ")
        << CodeMethodName(name)
        << CodeLiteral("(");
    
    env << CodeTypeName(curClass.name())
        << CodeLiteral(" ")
        << CodeVarName("this");
        
    env.scopes.declLocal("this", curClass);

    for(auto&& ar : args)
    {
        env << CodeLiteral(", ")
            << CodeTypeName(ar.typeName)
            << CodeLiteral(" ")
            << CodeVarName(ar.name);
        
        env.scopes.declLocal(ar.name, ValueType(ar.typeName));
    }

    env << CodeLiteral(")\n")
        << statements;

    env.scopes.pop();
    env.curFunc = nullopt;
}

void MethodExp::generateCode(GenEnv & env)
{
    // TODO: Scope resolution. Canonical name
    env << method
        << CodeLiteral("(");

    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
        {
            env << CodeLiteral(", ")
                << ar;
        }
        
        ar->generateCode(env);
        ++track;
    }
   
    env << CodeLiteral(")");
    //TODO: Possible set env.curFunc to null
}

void MethodStm::generateCode(GenEnv & env)
{
    env << CodeMethodTabs()
        << methodExp
        << CodeLiteral(";\n");
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
    env << CodeMethodTabs()
        << lhs
        << CodeLiteral(" = ")
        << rhs
        << CodeLiteral(";\n");
}

void VarDecStm::generateCode(GenEnv& env)
{
    //TODO: cannonical names
    //TODO: use env to determine if there should be a tab or not
    env << CodeClassTabs()
        << CodeTypeName(typeName)
        << CodeLiteral(" ")
        << CodeVarName(name)
        << CodeLiteral(";\n");

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
    env << CodeTypeName(typeName)
        << CodeLiteral(" ")
        << CodeVarName(name)
        << CodeLiteral(" = ")
        << value
        << CodeLiteral(";\n");

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
    env << CodeMethodTabs()
        << CodeLiteral("return ")
        << value
        << CodeLiteral(";\n");
}

void ClassDecl::generateCode(GenEnv& env)
{
    env.classes.enter(ValueType(name));
                       
    env << CodeLiteral("struct ")
        << CodeClassDecl(name)
        << CodeLiteral("\n{\n");

    if(parent) 
    {
        env.classes.setParent(ValueType(parent->name));

        env << CodeClassTabs()
            << CodeLiteral("struct ") // Hardcoded to 1 tab
            << CodeClassDecl(parent->name)
            << CodeLiteral(" ")
            << CodeParent()
            << CodeLiteral(";\n");
    }

    for (ASNPtr& member : members)
    {
        env << member;
    }

    env << CodeLiteral("};\n");
    env.classes.leave();
}

} //namespace dflat
