#include "asn.hpp"
#include "typechecker.hpp"
#include "config.hpp"
#include <iostream>

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

Type ASN::typeCheck(TypeEnv& env)
{
    Type t = typeCheckPrv(env);
    this->type = t;

    if (config::traceTypeCheck)
    {
        std::cout << toString() << " : " << (t.empty() ? "?" : t) << "\n";
    }

    return t;
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

Type VariableExp::typeCheckPrv(TypeEnv& env)
{
    // Variable type is the declared type for this name.
    return lookupVarType(env, name);
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

Type NumberExp::typeCheckPrv(TypeEnv&)
{
    // Number type is int.
    return intType;
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

Type BoolExp::typeCheckPrv(TypeEnv&)
{
    // Boolean type is bool.
    return boolType;
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

Type BinopExp::typeCheckPrv(TypeEnv& env)
{
    // Look up the type by the canonical name for this operation.
    // e.g. "1 + 2" might be "+(int,int)" with type int.
    Type lhsType = lhs->typeCheck(env);
    Type rhsType = rhs->typeCheck(env);
    String funcName = binopCanonicalName(op, lhsType, rhsType);
    return lookupRuleType(env, funcName);
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

Type UnopExp::typeCheckPrv(TypeEnv& env)
{
    // Look up the type by the canonical name for this operation.
    // e.g. "1 == 2" might be "==(int,int)" with type bool.
    Type rhsType = rhs->typeCheck(env);
    String funcName = unopCanonicalName(op, rhsType);
    return lookupRuleType(env, funcName);
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

Type Block::typeCheckPrv(TypeEnv& env)
{
    // Make sure all statements typecheck (in a new scope).
    // Final type is void.
    TypeEnv scopeEnv = env;

    for (ASNPtr const& stm : statements)
    {
        stm->typeCheck(scopeEnv);
    }

    return voidType;
}

//IfBlock:
IfStm::IfStm(ASNPtr&& _logicExp, ASNPtr&& _trueStatements, bool _hasFalse, ASNPtr&& _falseStatements)
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

Type IfStm::typeCheckPrv(TypeEnv& env)
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

Type WhileStm::typeCheckPrv(TypeEnv& env)
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
             Vector<FormalArg>&& _args, ASNPtr&& _statements)
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
        str += ar.type + " " + ar.name;
        ++track;
    }
    str += ")\n";
    str += statements->toString();
    return str;
}

Type MethodDef::typeCheckPrv(TypeEnv& env)
{
    Vector<Type> argTypes; // Just the arg types.
    Vector<Type> methodType = { type }; // Proper type list.
    
    for (FormalArg const& arg : args)
    {
        argTypes.push_back(arg.type);
        methodType.push_back(arg.type);
    }

    String methodName = funcCanonicalName(name, argTypes);
    mapNameToType(env, methodName, methodType); 

    // Currenly in this method.
    env.currentMethod = methodName;

    // Open new scope and declare args in it.
    TypeEnv methodEnv = env;

    for (FormalArg const& arg : args)
    {
        mapNameToType(methodEnv, arg.name, { arg.type });
    }

    // Typecheck body.
    statements->typeCheck(methodEnv);

    // No longer in a method.
    env.currentMethod = nullopt;

    // This isn't an expression, so return void.
    return voidType;
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

Type MethodExp::typeCheckPrv(TypeEnv&)
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

Type MethodStm::typeCheckPrv(TypeEnv& env)
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

Type NewExp::typeCheckPrv(TypeEnv&)
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

Type AssignStm::typeCheckPrv(TypeEnv& env)
{
    // RHS expression must match declared type of LHS variable.
    // Final type is void.
    Type lhsType = lookupVarType(env, variable);
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

Type MemberAssignStm::typeCheckPrv(TypeEnv&)
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

Type VarDecStm::typeCheckPrv(TypeEnv&)
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

Type RetStm::typeCheckPrv(TypeEnv&)
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

Type MethodDecl::typeCheckPrv(TypeEnv&)
{
    //TODO -- Final type should be the return type.
    return "";
}

// Class Definition
ClassDecl::ClassDecl(String _name, Vector<ASNPtr>&& _members, bool _extends, String _baseClass)
    : name(_name), members(move(_members)), extends(_extends), baseClass(_baseClass)
{
}

String ClassDecl::toString() const
{
    String str = "class " + name;
    if(extends)
        str += " extends " + baseClass;
    str += "\n{\n";
    for(auto&& ex : members)
        str += ex->toString() + "\n\n";
    str += "};";
    return str;
}

Type ClassDecl::typeCheckPrv(TypeEnv& env)
{
    //TODO more stuff needs doing.
    
    for (ASNPtr& member : members)
    {
        member->typeCheck(env);
    }
    
    // Final type is just the class name.
    return name;
}

} //namespace dflat
