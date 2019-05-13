#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

void VariableExp::generateCode(GenEnv & env) const
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

void NumberExp::generateCode(GenEnv & env) const
{
    env << CodeNumber(value);
}

void BoolExp::generateCode(GenEnv & env) const
{
    env << CodeNumber(value ? 1 : 0);
}

void BinopExp::generateCode(GenEnv & env) const
{
    env << CodeLiteral("(")
        << lhs
        << CodeLiteral(opString(op))
        << rhs
        << CodeLiteral(")");
}

void UnopExp::generateCode(GenEnv & env) const
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

void Block::generateCode(GenEnv & env) const
{
    startBlock(env);
    
    for (ASNPtr const& stm : statements)
    {
        env << stm;
    }

    endBlock(env);
}

void IfStm::generateCode(GenEnv & env) const
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

void WhileStm::generateCode(GenEnv & env) const
{
    env << CodeTabs()
        << CodeLiteral("while (")
        << logicExp
        << CodeLiteral(")\n")
        << statements;
}

static
void emitMethod(GenEnv& env, CanonName const& methodName, 
        ValueType const& retType, Vector<FormalArg> const& args,
        Vector<ASNPtr> const& body, bool isCons)
{
    env.enterMethod(methodName);
    ValueType const curClass = env.curClass().type;
    
    if (isCons)
    {
        env << CodeTabs()
            << CodeLiteral("void* ")
            << CodeConsName(methodName);
    }
    else
    {
        env << CodeTabs()
            << CodeTypeName(retType)
            << CodeLiteral(" ")
            << CodeMethodName(curClass, methodName);
    }

    env << CodeLiteral("(void* this");
    
    for(auto&& arg : args)
    {
        ValueType const argType(arg.typeName);

        env << CodeLiteral(", ")
            << CodeTypeName(argType)
            << CodeLiteral(" ")
            << CodeVarName(arg.name);
        
        env.declareLocal(arg.name, argType);
    }

    env << CodeLiteral(")\n");

    // Manually emitting the body.
    startBlock(env);
   
    env << CodeTabs()
        << CodeTypeName(curClass)
        << CodeLiteral(" ")
        << CodeVarName(config::thisName)
        << CodeLiteral(" = this;\n");

    for (ASNPtr const& stm : body)
    {
        env << stm;
    }
   
    if (isCons)
    {
        env << CodeTabs()
            << CodeLiteral("return ")
            << CodeVarName(config::thisName)
            << CodeLiteral(";\n");
    }
    
    endBlock(env);
    // Done with body.

    env.leaveMethod();
}

void MethodDef::generateCode(GenEnv & env) const
{
    CanonName const methodName(name, asnType->method());
    ValueType const retType(retTypeName);
    emitMethod(env, methodName, retType, args, statements->statements, false);
}

static
void emitConstructor(GenEnv& env, MethodType const& consType, 
        Vector<FormalArg> const& args, Vector<ASNPtr> const& body)
{
    CanonName const consName(config::consName, consType);
    ValueType const retType = env.curClass().type;
    emitMethod(env, consName, retType, args, body, true);
}

void ConsDef::generateCode(GenEnv & env) const
{
    emitConstructor(env, asnType->method(), args, statements->statements);
}

void MethodExp::generateCode(GenEnv & env) const
{
    String const objectName = method.object ? *method.object 
                                            : config::thisName;

    auto [thisType, methodName] = env.getMethodMeta(this);
    ValueType objectType = env.getLocalType(objectName);

    env << CodeLiteral("CALL(")
        << CodeTypeName(asnType->value())
        << CodeLiteral(", ")
        << CodeVTableMethodName(methodName)
        << CodeLiteral(", ")
        << CodeVarName(objectName);
    
    for (auto&& arg : args)
    {
        env << CodeLiteral(", ")
            << arg;
    }
   
    env << CodeLiteral(")");
}

void MethodStm::generateCode(GenEnv& env) const
{
    env << CodeTabs()
        << methodExp
        << CodeLiteral(";\n");
}

void NewExp::generateCode(GenEnv& env) const
{
    ValueType const resultType(typeName);

    // Need constructor's canonical name.
    auto [thisType, consName] = env.getMethodMeta(this);
    (void)thisType; // unused.
    if (resultType != thisType) //TODO ?
    {
        throw std::logic_error(resultType.toString() + " != " + thisType.toString());
    }

    // NEW(T,C,Args...) is a macro (see codegenerator_tools.cpp).

    if (args.empty())
    {
        env << CodeLiteral("NEW0(");
    }
    else
    {
        env << CodeLiteral("NEW(");
    }
   
    env << CodeClassDecl(resultType)
        << CodeLiteral(", ")
        << CodeVTableName(resultType)
        << CodeLiteral(", ")
        << CodeConsName(consName);

    for (ASNPtr const& arg : args)
    {
        env << CodeLiteral(", ")
            << arg;
    }

    env << CodeLiteral(")");
}

void AssignStm::generateCode(GenEnv& env) const
{
    env << CodeTabs()
        << lhs
        << CodeLiteral(" = ")
        << rhs
        << CodeLiteral(";\n");
}

void VarDecStm::generateCode(GenEnv& env) const
{
    ValueType const varType(typeName);

    env << CodeTabs()
        << CodeTypeName(varType)
        << CodeLiteral(" ")
        << CodeVarName(name)
        << CodeLiteral(";\n");

    if (env.inMethod())
    {
        env.declareLocal(name, varType);
    }
}

void VarDecAssignStm::generateCode(GenEnv& env) const
{
    ValueType const varType(typeName);
    
    env << CodeTabs()
        << CodeTypeName(varType)
        << CodeLiteral(" ")
        << CodeVarName(name)
        << CodeLiteral(" = ")
        << value
        << CodeLiteral(";\n");

    if (env.inMethod())
    {
        env.declareLocal(name, varType); 
    }
}

void RetStm::generateCode(GenEnv& env) const
{
    env << CodeTabs()
        << CodeLiteral("return ")
        << value
        << CodeLiteral(";\n");
}

void PrintStm::generateCode(GenEnv& env) const
{
    env << CodeTabs()
        << CodeLiteral("printf(\"%d\n\",")
        << value
        << CodeLiteral(");\n");
}

void ClassDecl::generateCode(GenEnv& env) const
{
    ValueType const classType(name);
    env.enterClass(classType);
                       
    env << CodeLiteral("struct ")
        << CodeClassDecl(classType)
        << CodeLiteral("\n{\n")
        << CodeTabIn();

    if (parent) 
    {
        ValueType const parentType(parent->name);

        env << CodeTabs()
            << CodeLiteral("struct ")
            << CodeClassDecl(parentType)
            << CodeLiteral(" ")
            << CodeParent()
            << CodeLiteral(";\n");
    }
    else
    {
        env << CodeTabs()
            << CodeLiteral("struct vtable vtable;\n");
    }
    
    // Emit default constructor.
    emitConstructor(env, MethodType(classType, {}), {}, {});

    // Emit members.
    for (ASNPtr const& member : members)
    {
        env << member;
    }

    env << CodeTabOut()
        << CodeLiteral("};\n");

    env.leaveClass();
}

} //namespace dflat
