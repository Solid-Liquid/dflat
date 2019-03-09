#include "asn.hpp"
#include "typechecker.hpp"

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

//VariableExp:
VariableExp::VariableExp(String const& name_)
    : name(name_)
{
}

String VariableExp::toString() const
{
    return name;
}

Type VariableExp::typeCheck(TypeEnv& env) const
{
    // Variable type is the declared type for this name.
    return lookupType(env, name);
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

Type TypeVariableExp::typeCheck(TypeEnv&) const
{
    //TODO add 'name' to the map and look for 'type' in env.type
    //note this variable only exists in the context of a method.
    //stack logic??
    return "";
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

Type NumberExp::typeCheck(TypeEnv&) const
{
    // Number type is int.
    return intType;
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

Type BinopExp::typeCheck(TypeEnv& env) const
{
    // Look up the type by the canonical name for this operation.
    // e.g. "1 + 2" might be "+(int,int)" with type int.
    Type lhsType = lhs->typeCheck(env);
    Type rhsType = rhs->typeCheck(env);
    String funcName = binopCanonicalName(op, lhsType, rhsType);
    return lookupType(env, funcName);
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

Type UnopExp::typeCheck(TypeEnv& env) const
{
    // Look up the type by the canonical name for this operation.
    // e.g. "1 == 2" might be "==(int,int)" with type bool.
    Type rhsType = rhs->typeCheck(env);
    String funcName = unopCanonicalName(op, rhsType);
    return lookupType(env, funcName);
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

Type Block::typeCheck(TypeEnv& env) const
{
    // Make sure all statements typecheck (in a new scope).
    // Final type is void.
    pushTypeScope(env);

    for (ASNPtr const& stm : statements)
    {
        stm->typeCheck(env);
    }

    popTypeScope(env);
    return voidType;
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

Type IfStm::typeCheck(TypeEnv& env) const
{
    // Conditional must have bool type.
    // Blocks must typecheck with type void.
    // Final type is void.
    Type condType = logicExp->typeCheck(env);
    assertTypeIs(condType, boolType);

    Type trueBlockType = trueStatements->typeCheck(env);
    assertTypeIs(trueBlockType, voidType);
   
    Type falseBlockType = falseStatements->typeCheck(env);
    assertTypeIs(falseBlockType, voidType);

    return voidType;
}

//WhileStm:
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

Type WhileStm::typeCheck(TypeEnv& env) const
{
    // Conditional must have bool type.
    // Body must typecheck with type void.
    // Final type is void.
    Type condType = logicExp->typeCheck(env);
    assertTypeIs(condType, boolType);

    Type bodyType = statements->typeCheck(env);
    assertTypeIs(bodyType, voidType);

    return voidType;
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

Type MethodDef::typeCheck(TypeEnv&) const
{
    //TODO
    return "";
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

Type MethodExp::typeCheck(TypeEnv&) const
{
    //TODO -- Look up canonical name of function.
    return "";
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

Type MethodStm::typeCheck(TypeEnv& env) const
{
    // Make sure the call typechecks.
    // Final type is void.
    methodExp->typeCheck(env);
    return voidType;
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

Type NewExp::typeCheck(TypeEnv&) const
{
    //TODO -- Similar to MethodExp
    return "";
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

Type AssignStm::typeCheck(TypeEnv& env) const
{
    // RHS expression must match declared type of LHS variable.
    // Final type is void.
    Type lhsType = lookupType(env, variable);
    Type rhsType = expression->typeCheck(env);
    assertTypeIs(rhsType, lhsType);
    return voidType;
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

Type MemberAssignStm::typeCheck(TypeEnv&) const
{
    //TODO -- Need to look up object.member's type.
    return "";
}

//VarDecStm:
VarDecStm::VarDecStm(String _type, String _name, ASNPtr&& _value)
    : type(_type), name(_name), value(move(_value))
{
}

String VarDecStm::toString() const
{
    return type + " " + name + " = " + value->toString() + ";";
}

Type VarDecStm::typeCheck(TypeEnv&) const
{
    //TODO -- Like AssignStm but adds a new name->type to the environment.
    return "";
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

Type RetStm::typeCheck(TypeEnv&) const
{
    //TODO -- Somehow check against return type of current method.
    return "";
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

Type MethodDecl::typeCheck(TypeEnv&) const
{
    //TODO -- Final type should be the return type.
    return "";
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

Type ClassDecl::typeCheck(TypeEnv&) const
{
    //TODO -- Final type should be the class name.
    return "";
}

} //namespace dflat
