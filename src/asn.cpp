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
VariableExp::VariableExp(String name_)
    : name(move(name_))
{
}

VariableExp::VariableExp(String object_, String member_)
    : object(move(object_))
    , name(move(member_))
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
    : lhs(move(_lhs))
    , rhs(move(_rhs))
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
    : rhs(move(_rhs))
    , op(_op)
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
IfStm::IfStm(ASNPtr&& _logicExp, BlockPtr&& _trueBlock, BlockPtr&& _falseBlock)
    : logicExp(move(_logicExp))
    , trueBlock(move(_trueBlock))
    , falseBlock(move(_falseBlock))
{
}

String IfStm::toString() const
{
    String str = "if(" + logicExp->toString() + ")\n";
    str += trueBlock->toString(); 

    if (!falseBlock->statements.empty())
    {
        str += "\nelse\n";
        str += falseBlock->toString();
    }
    return str;
}

//WhileStm:
WhileStm::WhileStm(ASNPtr&& _logicExp, BlockPtr&& _body)
    : logicExp(move(_logicExp))
    , body(move(_body))
{
}

String WhileStm::toString() const
{
    String str = "while(" + logicExp->toString() + ")\n";
    str += body->toString();
    return str;
}

//MethodDef:
MethodDef::MethodDef(ValueType _retType, String _name,
             Vector<FormalArg>&& _args, BlockPtr&& _body)
    : retType(move(_retType))
    , name(move(_name))
    , args(move(_args))
    , body(move(_body))
{
}

String MethodDef::toString() const
{
    String str = retType.toString() + " " + name + "(";
    int track = 0;
    for(FormalArg const& arg : args)
    {
        if(track > 0)
        {
            str += ", ";
        }

        str += arg.type.toString() + " " + arg.name;
        ++track;
    }
    str += ")\n";
    str += body->toString();
    return str;
}

//ConsDef:
ConsDef::ConsDef(Vector<FormalArg>&& _args, BlockPtr&& _body)
    : args(move(_args))
    , body(move(_body))
{
}

String ConsDef::toString() const
{
    String str = "cons(";
    int track = 0;
    
    for (FormalArg const& arg : args)
    {
        if (track > 0)
        {
            str += ", ";
        }

        str += arg.type.toString() + " " + arg.name;
        ++track;
    }

    str += ")\n";
    str += body->toString();
    return str;
}

//MethodExp:
MethodExp::MethodExp(Variable _method, Vector<ASNPtr>&& _args)
    : method(move(_method))
    , args(move(_args))
{
}

String MethodExp::toString() const
{
    String str = method.toString() + "(";
    int track = 0;
   
    for (ASNPtr const& arg : args)
    {
        if (track > 0)
        {
            str += ", ";
        }

        str += arg->toString();
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
NewExp::NewExp(ValueType _type, Vector<ASNPtr>&& _args)
    : type(move(_type))
    , args(move(_args))
{
}

String NewExp::toString() const
{
    String str = "new " + type.toString() + " (";
    size_t track = 0;

    for(ASNPtr const& arg : args)
    {
        if (track > 0)
        {
            str += ", ";
        }

        str += arg->toString();
        ++track;
    }

    str += ")";
    return str;
}

//AssignStm:
AssignStm::AssignStm(ASNPtr&& _lhs, ASNPtr&& _rhs)
    : lhs(move(_lhs))
    , rhs(move(_rhs))
{
}

String AssignStm::toString() const
{
    return lhs->toString() + " = " + rhs->toString() + ";";
}

//VarDecStm:
VarDecStm::VarDecStm(ValueType _type, String _name)
    : type(move(_type))
    , name(move(_name))
{
}

String VarDecStm::toString() const
{
    return type.toString() + " " + name + ";";
}

//VarDecAssignStm:
VarDecAssignStm::VarDecAssignStm(ValueType _lhsType, String _lhsName, ASNPtr&& _rhs)
    : lhsType(move(_lhsType))
    , lhsName(move(_lhsName))
    , rhs(move(_rhs))
{
}

String VarDecAssignStm::toString() const
{
    return lhsType.toString() + " " + lhsName + " = " + rhs->toString() + ";";
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

// Class Definition
ClassDecl::ClassDecl(ValueType _type, Vector<ASNPtr>&& _members, ClassDecl* _parent)
    : type(move(_type))
    , members(move(_members))
    , parent(_parent)
{
}

String ClassDecl::toString() const
{
    String str = "class " + type.toString();

    if (parent)
    {
        str += " extends " + parent->type.toString();
    }
    
    str += "\n{\n";
    
    for (ASNPtr const& m : members)
    {
        str += m->toString() + "\n\n";
    }
    
    str += "};";
    return str;
}

} //namespace dflat
