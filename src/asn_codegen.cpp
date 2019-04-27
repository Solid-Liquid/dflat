#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

void VariableExp::generateCode(GenEnv & env)
{
    if (object)
    {
        // object->member
        env.emitObject(*object, name);
    }
    else
    {
        if (env.lookupLocalType(name))
        {
            env << CodeVarName(name);
        }
        else
        {
            // Must be using implicit this.
            // TODO this logic might be a source of errors.
            env.emitObject(config::thisName, name);
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

static
void startBlock(GenEnv& env)

{
    env.enterScope();

    env << CodeTabs()
        << CodeLiteral("{\n")
        << CodeTabIn();
}

static
void endBlock(GenEnv& env)
{
    env << CodeTabOut()
        << CodeTabs()
        << CodeLiteral("}\n\n");

    env.leaveScope();
}

void Block::generateCode(GenEnv & env)
{
    startBlock(env);
    
    for (ASNPtr const& stmt : statements)
    {
        env << stmt;
    }

    endBlock(env);
}

void IfStm::generateCode(GenEnv & env)
{
    env << CodeTabs()
        << CodeLiteral("if (")
        << logicExp
        << CodeLiteral(")\n")
        << trueStatements;

    if (hasFalse)
    {
        env << CodeTabs()
            << CodeLiteral("else\n")
            << falseStatements;
    }
}

void WhileStm::generateCode(GenEnv & env)
{
    env << CodeTabs()
        << CodeLiteral("while (")
        << logicExp
        << CodeLiteral(")\n")
        << statements;
}

void MethodDef::generateCode(GenEnv & env)
{
    CanonName const methodName(name, asnType->method());
    env.enterMethod(methodName);
    ValueType const curClass = env.curClass().type;
    
    env << CodeTabs()
        << CodeTypeName(retTypeName)
        << CodeLiteral(" ")
        << CodeMethodName(curClass, methodName)
        << CodeLiteral("(")
        << CodeTypeName(curClass.name())
        << CodeLiteral(" ")
        << CodeVarName(config::thisName);
        
    for(auto&& ar : args)
    {
        env << CodeLiteral(", ")
            << CodeTypeName(ar.typeName)
            << CodeLiteral(" ")
            << CodeVarName(ar.name);
        
        env.declareLocal(ar.name, ValueType(ar.typeName));
    }

    env << CodeLiteral(")\n")
        << statements;

    env.leaveMethod();
}

void ConsDef::generateCode(GenEnv & env)
{
    CanonName const consName(config::consName, asnType->method());
    env.enterMethod(consName);
    ValueType const curClass = env.curClass().type;
    
    env << CodeTabs()
        << CodeTypeName(curClass.toString())
        << CodeLiteral(" ")
        << CodeConsName(consName)
        << CodeLiteral("(")
        << CodeTypeName(curClass.toString())
        << CodeLiteral(" ")
        << CodeVarName(config::thisName);
        
    for(auto&& ar : args)
    {
        env << CodeLiteral(", ")
            << CodeTypeName(ar.typeName)
            << CodeLiteral(" ")
            << CodeVarName(ar.name);
        
        env.declareLocal(ar.name, ValueType(ar.typeName));
    }

    env << CodeLiteral(")\n");
   
    // Due to having to insert a statement,
    //  we're going to do the block manually.
    startBlock(env);
    
    for (ASNPtr const& stmt : statements->statements)
    {
        env << stmt;
    }
    
    env << CodeTabs()
        << CodeLiteral("return ")
        << CodeVarName(config::thisName)
        << CodeLiteral(";\n");

    endBlock(env);
    env.leaveMethod();
}

void MethodExp::generateCode(GenEnv & env)
{
    String const objectName = method.object ? *method.object 
                                            : config::thisName;

    auto [thisType, methodName] = env.getMethodMeta(this);
    ValueType objectType = env.getLocalType(objectName);

    env << CodeMethodName(thisType, methodName)
        << CodeLiteral("(");

    // Base-cast if necessary.
    if (objectType != thisType)
    {
        env << CodeLiteral("(")
            << CodeTypeName(thisType.toString())
            << CodeLiteral(")");
    }

    env << CodeVarName(objectName);
    
    for (auto&& ar : args)
    {
        env << CodeLiteral(", ")
            << ar;
        
        ar->generateCode(env);
    }
   
    env << CodeLiteral(")");
}

void MethodStm::generateCode(GenEnv& env)
{
    env << CodeTabs()
        << methodExp
        << CodeLiteral(";\n");
}

void NewExp::generateCode(GenEnv& env)
{
    // Need constructor's canonical name.
    auto [thisType, consName] = env.getMethodMeta(this);
    (void)thisType; // unused.

    // DF_NEW(T,C,Args...) is a macro (see codegenerator_tools.cpp).

    if (args.empty())
    {
        env << CodeLiteral("DF_NEW0(");
    }
    else
    {
        env << CodeLiteral("DF_NEW(");
    }

    env << CodeClassDecl(typeName)
        << CodeLiteral(", ")
        << CodeConsName(consName);

    for (ASNPtr const& arg : args)
    {
        env << CodeLiteral(", ")
            << arg;
    }

    env << CodeLiteral(")");
}

void AssignStm::generateCode(GenEnv& env)
{
    // TODO: check if this works for every case (i.e. rhs is new stm)
    env << CodeTabs()
        << lhs
        << CodeLiteral(" = ")
        << rhs
        << CodeLiteral(";\n");
}

void VarDecStm::generateCode(GenEnv& env)
{
    env << CodeTabs()
        << CodeTypeName(typeName)
        << CodeLiteral(" ")
        << CodeVarName(name)
        << CodeLiteral(";\n");

    if (env.inMethod())
    {
        env.declareLocal(name, ValueType(typeName));
    }
}

void VarDecAssignStm::generateCode(GenEnv& env)
{
    env << CodeTabs()
        << CodeTypeName(typeName)
        << CodeLiteral(" ")
        << CodeVarName(name)
        << CodeLiteral(" = ")
        << value
        << CodeLiteral(";\n");

    if (env.inMethod())
    {
        env.declareLocal(name, ValueType(typeName)); 
    }
}

void RetStm::generateCode(GenEnv& env)
{
    env << CodeTabs()
        << CodeLiteral("return ")
        << value
        << CodeLiteral(";\n");
}

void ClassDecl::generateCode(GenEnv& env)
{
    ValueType const classType(name);
    env.enterClass(classType);
                       
    env << CodeLiteral("struct ")
        << CodeClassDecl(name)
        << CodeLiteral("\n{\n");

    if(parent) 
    {
        env << CodeTabs()
            << CodeLiteral("struct ")
            << CodeClassDecl(parent->name)
            << CodeLiteral(" ")
            << CodeParent()
            << CodeLiteral(";\n");
    }
    
    // Emit default constructor.
    // TODO duplicates a ton of code from ConsDef
    MethodType const consType(classType, {});
    CanonName const consName(config::consName, consType);
    env.enterMethod(consName);
    
    env << CodeTabs()
        << CodeTypeName(classType.toString())
        << CodeLiteral(" ")
        << CodeConsName(consName)
        << CodeLiteral("(")
        << CodeTypeName(classType.toString())
        << CodeLiteral(" ")
        << CodeVarName(config::thisName)
        << CodeLiteral(")\n");
   
    startBlock(env);
    
    env << CodeTabs()
        << CodeLiteral("return ")
        << CodeVarName(config::thisName)
        << CodeLiteral(";\n");

    endBlock(env);
    env.leaveMethod();

    // Emit members.
    for (ASNPtr& member : members)
    {
        env << member;
    }

    env << CodeLiteral("};\n\n");
    env.leaveClass();
}

} //namespace dflat
