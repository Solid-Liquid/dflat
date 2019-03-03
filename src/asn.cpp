#include "asn.hpp"

namespace dflat
{

String opString(OpType op)
{
    switch(op)
    {
        case opNull: std::abort(); // This shall never be printed.
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


//ASN:
ASN::~ASN()
{
}

//BinopExp:
BinopExp::BinopExp(ASNPtr&& _left, OpType _op, ASNPtr&& _right)
    : left(std::move(_left))
    , right(std::move(_right))
    , op(_op)
{
}

String BinopExp::toString() const
{
    return "(" + left->toString() + " " + opString(op) +
            " " + right->toString() + ")";
}

//VariableExp:
VariableExp::VariableExp(String const& name_)
    : name(name_)
{
}

String VariableExp::toString() const
{
    return name;
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

//UnopExp:
UnopExp::UnopExp(ASNPtr&& _nested, OpType _op)
    : nested(move(_nested)), op(_op)
{
}

String UnopExp::toString() const
{
    return "(" + opString(op) + nested->toString() + ")";
}

//Block:
Block::Block(Vector<ASNPtr>&& _statements)
    : statements(move(_statements))
{
}

String Block::toString() const
{
    String s = "{";
    for (ASNPtr const& stmt : statements)
    {
        s += stmt->toString() + "\n";
    }
    s += "}";
    return s;
}

//IfBlock:
IfStm::IfStm(ASNPtr&& _logicExp, ASNPtr&& _trueStatements, ASNPtr&& _falseStatements)
    : logicExp(move(_logicExp))
    , trueStatements(move(_trueStatements))
    , falseStatements(move(_falseStatements))
{
}

String IfStm::toString() const
{
    String str = "\nif(" + logicExp->toString() + ")\n";
    str += trueStatements->toString() + "\n";
    str += "else";
    str += falseStatements->toString() + "\n";
    return str;
}

//WhileBlock:
WhileStm::WhileStm(ASNPtr&& _logicExp, ASNPtr&& _statements)
    : logicExp(move(_logicExp)), statements(move(_statements))
{
}

String WhileStm::toString() const
{
    String str = "\nif(" + logicExp->toString() + ")\n";
    str += statements->toString() + "\n";
    return str;
}

//MethodBlock:
MethodDef::MethodDef(String _type, String _name,
             Vector<ASNPtr>&& _args, ASNPtr&& _statements)
    : type(_type),name(_name),args(move(_args)), statements(move(_statements))
{
}

String MethodDef::toString() const
{
    String str = "\n" + type + " " + name + "(";
    int track = 0;
    for(auto&& ar : args)
    {
        str += ar->toString();
        if(track > 0)
            str += ", ";
        ++track;
    }
    str += ")\n";
    str += statements->toString() + "\n";
    return str;
}

//MethodStm:
MethodExp::MethodExp(String _name, Vector<ASNPtr>&& _args)
  : name(_name), args(move(_args))
{
}

String MethodExp::toString() const
{
    String str = name + "(";
    int track = 0;
    for(auto&& ar : args)
    {
        str += ar->toString();
        if(track > 0)
            str += ", ";
        ++track;
    }
    str += ")";
    return str;
}

//AssignmentStm:
AssignmentStm::AssignmentStm(String _variable, ASNPtr&& _expression)
  : variable(_variable), expression(move(_expression))
{
}

String AssignmentStm::toString() const
{
    return variable + " = " + expression->toString();
}

//DeclarationStm:
VarDefStm::VarDefStm(String _type, String _name)
    : type(_type), name(_name)
{
}

String VarDefStm::toString() const
{
    return type + " " + name;
}

} //namespace dflat
