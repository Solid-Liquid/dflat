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

//IfBlock:
IfBlock::IfBlock(ASNPtr&& _logicExp,std::vector<ASNPtr>&& _statements)
    : logicExp(move(_logicExp)), statements(move(_statements))
{
}

String IfBlock::toString() const
{
    String str = "\nif(" + logicExp->toString() + ")\n{\n";
    for(auto&& stm : statements)
        str += stm->toString() + "\n";
    str += "}\n";
    return str;
}

//ElseBlock:
ElseBlock::ElseBlock(std::vector<ASNPtr>&& _statements)
    : statements(move(_statements))
{
}

String ElseBlock::toString() const
{
    String str = "\nelse\n{\n";
    for(auto&& stm : statements)
        str += stm->toString() + "\n";
    str += "}\n";
    return str;
}

//WhileBlock:
WhileBlock::WhileBlock(ASNPtr&& _logicExp,std::vector<ASNPtr>&& _statements)
    : logicExp(move(_logicExp)), statements(move(_statements))
{
}

String WhileBlock::toString() const
{
    String str = "\nif(" + logicExp->toString() + ")\n{\n";
    for(auto&& stm : statements)
        str += stm->toString() + "\n";
    str += "}\n";
    return str;
}

//MethodBlock:
MethodBlock::MethodBlock(String _type, String _name,
             std::vector<ASNPtr>&& _args,std::vector<ASNPtr>&& _statements)
    : type(_type),name(_name),args(move(_args)), statements(move(_statements))
{
}

String MethodBlock::toString() const
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
    str += ")\n{\n";
    for(auto&& stm : statements)
        str += stm->toString() + "\n";
    str += "}\n";
    return str;
}

//MethodStm:
MethodStm::MethodStm(String _name,std::vector<ASNPtr>&& _args)
  : name(_name), args(move(_args))
{
}

String MethodStm::toString() const
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
AssignmentStm::AssignmentStm(String _variable,ASNPtr&& _expression)
  : variable(_variable), expression(move(_expression))
{
}

String AssignmentStm::toString() const
{
    return variable + " = " + expression->toString();
}

//DeclarationStm:
DeclarationStm::DeclarationStm(String _type, String _name)
    : type(_type), name(_name)
{
}

String DeclarationStm::toString() const
{
    return type + " " + name;
}

} //namespace dflat
