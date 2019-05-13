#include "asn.hpp"
#include "typechecker.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

using std::move;

String opString(OpType op)
{
    switch(op)
    {
        case opPlus:        return "+";
        case opMinus:       return "-";
        case opDiv:         return "/";
        case opMult:        return "*";
        case opNot:         return "!";
        case opAnd:         return "&&";
        case opOr:          return "||";
        case opLogEq:       return "==";
        case opLogNotEq:    return "!=";
    }

    std::abort(); // Unhandled op.
}

bool operator==(ASNPtr const& a, ASNPtr const& b)
{
    if (!a && !b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    return *a == *b;
}

bool operator!=(ASNPtr const& a, ASNPtr const& b)
{
    return !(a == b);
}

bool operator==(BlockPtr const& a, BlockPtr const& b)
{
    if (!a && !b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    return *a == *b;
}

bool operator!=(BlockPtr const& a, BlockPtr const& b)
{
    return !(a == b);
}


//ASN:
ASN::~ASN()
{
}

//VariableExp:
VariableExp::VariableExp(String const& name_)
    : name(name_)
{
}

VariableExp::VariableExp(String const& object_, String const& member_)
    : object(object_), name(member_)
{
}

String VariableExp::toString() const
{
    if (object)
    {
        return *object + "." + name;
    }
    else
    {
        return name;
    }
}

//NumberExp:
NumberExp::NumberExp(int value_)
    : value(value_)
{
}

String NumberExp::toString() const
{
    return to_string(value);
}

//BooleanExp:
BoolExp::BoolExp(bool value_)
    : value(value_)
{
}

String BoolExp::toString() const
{
    if(value)
        return "true";
    else
        return "false";
}

//BinopExp:
BinopExp::BinopExp(ASNPtr&& _lhs, OpType _op, ASNPtr&& _rhs)
    : lhs(std::move(_lhs))
    , rhs(std::move(_rhs))
    , op(_op)
{
}

String BinopExp::toString() const
{
    return "(" + lhs->toString() + " " + opString(op) +
            " " + rhs->toString() + ")";
}

//UnopExp:
UnopExp::UnopExp(ASNPtr&& _rhs, OpType _op)
    : rhs(move(_rhs)), op(_op)
{
}

String UnopExp::toString() const
{
    return "(" + opString(op) + rhs->toString() + ")";
}

//Block:
Block::Block(Vector<ASNPtr>&& _statements)
    : statements(move(_statements))
{
}

String Block::toString() const
{
    String s = "{\n";
    for (ASNPtr const& stmt : statements)
    {
        s += stmt->toString() + "\n";
    }
    s += "}";
    return s;
}

//IfBlock:
IfStm::IfStm(ASNPtr&& _logicExp, BlockPtr&& _trueStatements, bool _hasFalse, BlockPtr&& _falseStatements)
    : logicExp(move(_logicExp))
    , trueStatements(move(_trueStatements))
    , hasFalse(_hasFalse)
    , falseStatements(move(_falseStatements))
{
}

String IfStm::toString() const
{
    String str = "if(" + logicExp->toString() + ")\n";
    str += trueStatements->toString();
    if(hasFalse)
    {
        str += "\nelse\n";
        str += falseStatements->toString();
    }
    return str;
}

//WhileStm:
WhileStm::WhileStm(ASNPtr&& _logicExp, BlockPtr&& _statements)
    : logicExp(move(_logicExp)), statements(move(_statements))
{
}

String WhileStm::toString() const
{
    String str = "while(" + logicExp->toString() + ")\n";
    str += statements->toString();
    return str;
}

//MethodDef:
MethodDef::MethodDef(String _retTypeName, String _name,
             Vector<FormalArg>&& _args, BlockPtr&& _statements)
    : retTypeName(_retTypeName)
    , name(_name)
    , args(move(_args))
    , statements(move(_statements))
{
}

String MethodDef::toString() const
{
    String str = retTypeName + " " + name + "(";
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            str += ", ";
        str += ar.typeName + " " + ar.name;
        ++track;
    }
    str += ")\n";
    str += statements->toString();
    return str;
}

//ConsDef:
ConsDef::ConsDef(Vector<FormalArg>&& _args, BlockPtr&& _statements)
    : args(move(_args))
    , statements(move(_statements))
{
}

String ConsDef::toString() const
{
    String str = "cons(";
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            str += ", ";
        str += ar.typeName + " " + ar.name;
        ++track;
    }
    str += ")\n";
    str += statements->toString();
    return str;
}

//MethodExp:
MethodExp::MethodExp(Variable _method, Vector<ASNPtr>&& _args)
    : method(move(_method)), args(move(_args))
{
}

String MethodExp::toString() const
{
    String str = method.toString() + "(";
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            str += ", ";
        str += ar->toString();
        ++track;
    }
    str += ")";
    return str;
}

//MethodStm:
MethodStm::MethodStm(ASNPtr&& methodExp_)
    : methodExp(move(methodExp_))
{
}

String MethodStm::toString() const
{
    return methodExp->toString() + ";";
}

//NewExp:
NewExp::NewExp(String _typeName, Vector<ASNPtr>&& _args)
    : typeName(_typeName), args(move(_args))
{
}

String NewExp::toString() const
{
    String str = "new " + typeName + " (";
    size_t track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            str += ", ";
        str += ar -> toString();
        ++track;
    }
    str += ")";
    return str;
}

//AssignStm:
AssignStm::AssignStm(ASNPtr&& _lhs, ASNPtr&& _rhs)
    : lhs(move(_lhs)), rhs(move(_rhs))
{
}

String AssignStm::toString() const
{
    return lhs->toString() + " = " + rhs->toString() + ";";
}

//VarDecStm:
VarDecStm::VarDecStm(String _typeName, String _name)
    : typeName(_typeName), name(_name)
{
}

String VarDecStm::toString() const
{
    return typeName + " " + name + ";";
}

//VarDecAssignStm:
VarDecAssignStm::VarDecAssignStm(String _typeName, String _name, ASNPtr&& _value)
    : typeName(_typeName), name(_name), value(move(_value))
{
}

String VarDecAssignStm::toString() const
{
    return typeName + " " + name + " = " + value->toString() + ";";
}

//RetStm:
RetStm::RetStm(ASNPtr&& _value)
    : value(move(_value))
{
}

String RetStm::toString() const
{
    return "return " + value->toString() + ";";
}

//PrintStm:
PrintStm::PrintStm(ASNPtr&& _value)
    : value(move(_value))
{
}

String PrintStm::toString() const
{
    return "print(" + value->toString() + ");";
}

// Class Definition
ClassDecl::ClassDecl(String _name, Vector<ASNPtr>&& _members, ClassDecl* _parent)
    : name(_name), members(move(_members)), parent(_parent)
{
}

String ClassDecl::toString() const
{
    String str = "class " + name;
    if(parent)
        str += " extends " + parent->name;
    str += "\n{\n";
    for(auto&& ex : members)
        str += ex->toString() + "\n\n";
    str += "};";
    return str;
}

} //namespace dflat
