#include "asn.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

void VariableExp::generateCode(GenEnv & env)
{
    env.write() << (object ? *object + "->":"") << name;
}

void NumberExp::generateCode(GenEnv & env)
{
    env.write() << to_string(value);
}

void BoolExp::generateCode(GenEnv & env)
{
    env.write() << (value ? "1" : "0");
}

void BinopExp::generateCode(GenEnv & env)
{
    env.write() << "(";
    lhs->generateCode(env);
    env.write() << opString(op);
    rhs->generateCode(env);
    env.write() << ")";
}

void UnopExp::generateCode(GenEnv & env)
{
    env.write() << "(" + opString(op);
    rhs->generateCode(env);
    env.write() << ")";
}

void Block::generateCode(GenEnv & env)
{
    env.writeTabbed() << "{\n";
    env.tabs++;
    for (ASNPtr const& stmt : statements)
    {
        stmt->generateCode(env);
    }
    env.tabs--;
    env.writeTabbed() << "}\n";
}

void IfStm::generateCode(GenEnv & env)
{
    env.writeTabbed() << "if(";
    logicExp->generateCode(env);
    env.write() << ")\n";
    trueStatements->generateCode(env);
    if(hasFalse)
    {
        env.writeTabbed() << "else\n";
        falseStatements->generateCode(env);
    }
}

void WhileStm::generateCode(GenEnv & env)
{
    env.writeTabbed() << "while(";
    logicExp->generateCode(env);
    env.write() << ")\n";
    statements->generateCode(env);
}

void MethodDef::generateCode(GenEnv & env)
{
    //TODO: check scope and use connonical/overloaded name.
    //possible TODO: prototype.
    env.writeTabbed() << retTypeName + " " + name + "(";
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            env.write() << ", ";
        env.write() << ar.typeName + " " + ar.name;
        ++track;
    }
    env.write() << ")\n";
    statements->generateCode(env);
}

void MethodExp::generateCode(GenEnv & env)
{
    // TODO: Scope resolution. Cannonical name
    method->generateCode(env);
    env.write() << "(";
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            env.write() << ",";
        ar->generateCode(env);
        ++track;
    }
    env.write() <<  ")";
    //TODO: Possible set env.curFunc to null
}

void MethodStm::generateCode(GenEnv & env)
{
    //TODO: check scope and use connonical/overloaded name.
    //possible TODO: prototype.

    env.writeTabbed();
    //TODO: set env.func to canonical func name 
    methodExp->generateCode(env);
    env.write() << ";\n";
    //TODO: set env.func to null
}

void NewExp::generateCode(GenEnv & env)
{
    // TODO: everything.
    //  <type>* <name> = (<type>*)malloc(sizeof(<type>));
    //  theoretically call constructor
    //
//        String typeName;
//        Vector<ASNPtr> args;
    env.write() << "";
}

void AssignStm::generateCode(GenEnv & env)
{
    // TODO: check if this works for every case (i.e. rhs is new stm)
    env.writeTabbed();
    lhs->generateCode(env);
    env.write() << " = ";
    rhs->generateCode(env);
    env.write() << ";\n";
}

void VarDecStm::generateCode(GenEnv & env)
{
    //TODO: cannonical names
    env.writeTabbed();
    //typename could be cannonical name of class
    if(typeName == "bool")
        env.write() << "int";
    else
        env.write() << typeName;
    env.write() << " " + name + " = ";
    value->generateCode(env);
    env.write() << ";\n";
}

void RetStm::generateCode(GenEnv & env)
{
    env.writeTabbed() << "return ";
    value->generateCode(env);
    env.write() << ";\n";
}

void ClassDecl::generateCode(GenEnv & env)
{
                       
    env.curClass = name;
    
    // class was already declared.

    env.writeTabbed() << "struct " + name + "\n{\n";

    env.tabs++;

    if(parent) {

        for(ASNPtr& member : parent->members) {
            member->generateCode(env);
        }
    }

    for (ASNPtr& member : members)
    {
        member->generateCode(env);
    }

    env.tabs--;

    env.structDef << "};\n";

    env.curClass = nullopt;    
}

} //namespace dflat
