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
    env.write() << value ? "1" : "0";
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
    env.write() << "{\n";
    for (ASNPtr const& stmt : statements)
    {
        stmt->generateCode(env);
    }
    env.write() << "}\n";
}

void IfStm::generateCode(GenEnv & env)
{
    env.write() << "if(";
    logicExp->generateCode(env);
    env.write() << ")\n";
    trueStatements->generateCode(env);
    if(hasFalse)
    {
        env.write() << "else\n";
        falseStatements->generateCode(env);
    }
}

void WhileStm::generateCode(GenEnv & env)
{
    env.write() << "while(";
    logicExp->generateCode(env);
    env.write() << ")\n";
    statements->generateCode(env);
}

void MethodDef::generateCode(GenEnv & env)
{
    //TODO: check scope and use connonical/overloaded name.
    //possible TODO: prototype.
    env.write() << retTypeName + " " + name + "(";
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            env.write() << ", ";
        env.write() << ar.typeName + " " + ar.name;
        ++track;
    }
    env.write() << ")";
    statements->generateCode(env);
}

void MethodExp::generateCode(GenEnv & env)
{
    // TODO: Scope resolution. Cannonical name
    method->generateCode(env);
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
    lhs->generateCode(env);
    env.write() << " = ";
    rhs->generateCode(env);
    env.write() << ";\n";
}

void VarDecStm::generateCode(GenEnv & env)
{
    //TODO: cannonical names
    //typename could be cannonical name of class or
    //"int" for int, "int" for bool
    env.write() << typeName + " " + name + " = ";
    value->generateCode(env);
    env.write() << ";\n";
}

void RetStm::generateCode(GenEnv & env)
{
    env.write() << "return ";
    value->generateCode(env);
    env.write() << ";\n";
}

void ClassDecl::generateCode(GenEnv & env)
{
                        /*
    // class was already declared.
    if(env.classMembers.count(name)) {
        //Throw Exception, Complain, Die.
    }


    // add this class to the map
    Vector<Member>& thisClass = env.classMembers[name];

    for(ASNPtr& member : members) {
        thisClass.push_back(member);
        member->generateCode(env);
    }

    env.curClass = name;
    env.write() << "struct " + name + "\n{\n";
    if(extends) {
        // base Class has not been declared
        if(!env.classMembers.count(baseClass)) {
            //Throw Exception, Complain, Die.
        }

        for(ASNPtr& member : env.classMembers[baseClass]) {
            thisClass.push_back(member);
            
        }
    }

    env.structDef << "};\n";
    */
}

} //namespace dflat
