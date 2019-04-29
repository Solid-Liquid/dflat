#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

void VariableExp::generateCode(GenEnv & env) const
{
    (void)env;
//    if (object)
//    {
//        // object->member
//        env.emitObject(*object, name);
//    }
//    else
//    {
//        if (env.lookupLocalType(name))
//        {
//            env << CodeVarName(name);
//        }
//        else
//        {
//            // Must be using implicit this.
//            // TODO this logic might be a source of errors.
//            env.emitObject(config::thisName, name);
//        }
//    }
}

void NumberExp::generateCode(GenEnv & env) const
{
    (void)env;
//    env << CodeNumber(value);
}

void BoolExp::generateCode(GenEnv & env) const
{
    (void)env;
//    env << CodeNumber(value ? 1 : 0);
}

void BinopExp::generateCode(GenEnv & env) const
{
    (void)env;
//    env << CodeLiteral("(")
//        << lhs
//        << CodeLiteral(opString(op))
//        << rhs
//        << CodeLiteral(")");
}

void UnopExp::generateCode(GenEnv & env) const
{
    (void)env;
//    env << CodeLiteral("(") 
//        << CodeLiteral(opString(op))
//        << rhs
//        << CodeLiteral(")");
}

void Block::generateCode(GenEnv & env) const
{
    (void)env;
//    env.startBlock();
//    
//    for (ASNPtr const& stm : statements)
//    {
//        env << stm;
//    }
//
//    env.endBlock();
}

void IfStm::generateCode(GenEnv & env) const
{
    (void)env;
//    env << CodeTabs()
//        << CodeLiteral("if (")
//        << logicExp
//        << CodeLiteral(")\n")
//        << trueBlock;
//
//    if (!falseBlock->statements.empty())
//    {
//        env << CodeTabs()
//            << CodeLiteral("else\n")
//            << falseBlock;
//    }
}

void WhileStm::generateCode(GenEnv & env) const
{
    (void)env;
//    env << CodeTabs()
//        << CodeLiteral("while (")
//        << logicExp
//        << CodeLiteral(")\n")
//        << body;
}


void MethodDef::generateCode(GenEnv & env) const
{
    (void)env;
//    CanonName const methodName(name, asnType->method());
//    env.emitMethod(methodName, retType, args, body->statements, false);
}

void ConsDef::generateCode(GenEnv & env) const
{
    (void)env;
//    env.emitConstructor(asnType->method(), args, body->statements);
}

void MethodExp::generateCode(GenEnv & env) const
{
    (void)env;
//    String const objectName = method.object ? *method.object 
//                                            : config::thisName;
//
//    auto [thisType, methodName] = env.getMethodMeta(this);
//    ValueType objectType = env.getLocalType(objectName);
//
//    // CALL is a macro (see codegenerator_tools.cpp).
//    env << CodeLiteral("CALL(")
//        << CodeTypeName(asnType->value())
//        << CodeLiteral(", ")
//        << CodeVTableMethodName(methodName)
//        << CodeLiteral(", ")
//        << CodeVarName(objectName);
//    
//    for (auto&& arg : args)
//    {
//        env << CodeLiteral(", ")
//            << arg;
//    }
//   
//    env << CodeLiteral(")");
}

void MethodStm::generateCode(GenEnv& env) const
{
    (void)env;
//    env << CodeTabs()
//        << methodExp
//        << CodeLiteral(";\n");
}

void NewExp::generateCode(GenEnv& env) const
{
    (void)env;
//    // Need constructor's canonical name.
//    auto [thisType, consName] = env.getMethodMeta(this);
//
//    if (type != thisType) // Small sanity check.
//    {
//        throw std::logic_error("new: " + type.toString() + " != " + thisType.toString());
//    }
//
//    // NEW/NEW0 are macros (see codegenerator_tools.cpp).
//    if (args.empty())
//    {
//        env << CodeLiteral("NEW0(");
//    }
//    else
//    {
//        env << CodeLiteral("NEW(");
//    }
//   
//    env << CodeClassDecl(type)
//        << CodeLiteral(", ")
//        << CodeVTableName(type)
//        << CodeLiteral(", ")
//        << CodeConsName(consName);
//
//    for (ASNPtr const& arg : args)
//    {
//        env << CodeLiteral(", ")
//            << arg;
//    }
//
//    env << CodeLiteral(")");
}

void AssignStm::generateCode(GenEnv& env) const
{
    (void)env;
//    env << CodeTabs()
//        << lhs
//        << CodeLiteral(" = ")
//        << rhs
//        << CodeLiteral(";\n");
}

void VarDecStm::generateCode(GenEnv& env) const
{
    (void)env;
//    env << CodeTabs()
//        << CodeTypeName(type)
//        << CodeLiteral(" ")
//        << CodeVarName(name)
//        << CodeLiteral(";\n");
//
//    if (env.inMethod())
//    {
//        env.declareLocal(name, type);
//    }
}

void VarDecAssignStm::generateCode(GenEnv& env) const
{
    (void)env;
//    env << CodeTabs()
//        << CodeTypeName(lhsType)
//        << CodeLiteral(" ")
//        << CodeVarName(lhsName)
//        << CodeLiteral(" = ")
//        << rhs
//        << CodeLiteral(";\n");
//
//    if (env.inMethod())
//    {
//        env.declareLocal(lhsName, lhsType); 
//    }
}

void RetStm::generateCode(GenEnv& env) const
{
    (void)env;
//    env << CodeTabs()
//        << CodeLiteral("return ")
//        << value
//        << CodeLiteral(";\n");
}

void ClassDecl::generateCode(GenEnv& env) const
{
    (void)env;
//    if (type.tvars().empty())
//    {
//        // Non-parametric type, instantiate it now.
//        env.instantiate(type);
//    }
}

} //namespace dflat
