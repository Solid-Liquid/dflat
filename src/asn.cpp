#include "asn.hpp"

namespace dflat
{

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

//TypeVariableExp:
TypeVariableExp::TypeVariableExp(String const& type_, String const& name_)
    : type(type_), name(name_)
{
}

String TypeVariableExp::toString() const
{
    return type + " " + name;
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
    String s = "{\n";
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
    String str = "if(" + logicExp->toString() + ")\n";
    str += trueStatements->toString();
    str += "\nelse\n";
    str += falseStatements->toString();
    return str;
}

//WhileBlock:
WhileStm::WhileStm(ASNPtr&& _logicExp, ASNPtr&& _statements)
    : logicExp(move(_logicExp)), statements(move(_statements))
{
}

String WhileStm::toString() const
{
    String str = "while(" + logicExp->toString() + ")\n";
    str += statements->toString();
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
    String str = type + " " + name + "(";
    int track = 0;
    for(auto&& ar : args)
    {
        if(track > 0)
            str += ", ";
        str += ar->toString();
        ++track;
    }
    str += ")\n";
    str += statements->toString();
    return str;
}

//MethodExp:
MethodExp::MethodExp(String _object, String _method, Vector<ASNPtr>&& _args)
  : object(_object), method(_method), args(move(_args))
{
}

String MethodExp::toString() const
{
    String str = object + "." + method + "(";
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

//NewStm:
NewExp::NewExp(String _type, Vector<ASNPtr>&& _args)
    : type(_type), args(move(_args))
{
}

String NewExp::toString() const
{
    String str = "new " + type + " (";
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
AssignStm::AssignStm(String _variable, ASNPtr&& _expression)
  : variable(_variable), expression(move(_expression))
{
}

String AssignStm::toString() const
{
    return variable + " = " + expression->toString() + ";";
}

//MemberAssignStm:
MemberAssignStm::MemberAssignStm(String _object, String _member, ASNPtr&& _expression)
  : object(_object), member(_member), expression(move(_expression))
{
}

String MemberAssignStm::toString() const
{
    return object + "." + member + " = " + expression->toString() + ";";
}

//DeclarationStm:
VarDecStm::VarDecStm(String _type, String _name, ASNPtr&& _value)
    : type(_type), name(_name), value(move(_value))
{
}

String VarDecStm::toString() const
{
    return type + " " + name + " = " + value->toString() + ";";
}

RetStm::RetStm(ASNPtr&& _value)
    : value(move(_value))
{
}

String RetStm::toString() const
{
    return "return " + value->toString() + ";";
}

// Method Declaration
MethodDecl::MethodDecl(String _type, String _name, Vector<ASNPtr>&& _exps)
    : type(_type), name(_name), exps(move(_exps))
{
}

String MethodDecl::toString() const
{
    String str = type + " " + name + "(";
    size_t track = 0;
    for(auto&& ex : exps)
    {
        if(track > 0)
            str += ", ";
        str += ex -> toString();
        ++track;
    }
    str += ");";
    return str;
}

// Class Definition
ClassDecl::ClassDecl(String _name, Vector<ASNPtr>&& _members)
    : name(_name), members(move(_members))
{
}

String ClassDecl::toString() const
{
    String str = "class " + name + "\n{\n";
    for(auto&& ex : members)
        str += ex->toString() + "\n\n";
    str += "};";
    return str;
}

} //namespace dflat
