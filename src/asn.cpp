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
    Type type = typeCheckPrv(env);
    asnType = type;

    if (config::traceTypeCheck)
    {
        std::cout << toString() << " : " << type.toString() << "\n";
    }

    return type;
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

Type VariableExp::typeCheckPrv(TypeEnv& env)
{
    // Variable type is the declared type for this name.
    if (object)
    {
        // Lookup as a member of object.
        ValueType objectType = lookupVarType(env, *object);
        return lookupVarTypeByClass(env, name, objectType);
    }
    else
    {
        // Lookup as a local or a member of the current class.
        return lookupVarType(env, name);
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
    // e.g. "-1" might be "-(int)" with type int.
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
MethodDef::MethodDef(String _retTypeName, String _name,
             Vector<FormalArg>&& _args, ASNPtr&& _statements)
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

Type MethodDef::typeCheckPrv(TypeEnv& env)
{
    Vector<Type> argTypes; // Just the arg types.
    MethodType methodType(ValueType(retTypeName), {});

    for (FormalArg const& arg : args)
    {
        ValueType argType(arg.typeName);
        argTypes.push_back(argType);
        methodType.addArgType(argType);
    }

    String methodName = funcCanonicalName(name, argTypes);
    mapNameToType(env, methodName, methodType);

    // Currenly in this method.
    env.currentMethod = methodName;

    // Open new scope and declare args in it.
    TypeEnv methodEnv = env;

    for (FormalArg const& arg : args)
    {
        mapNameToType(methodEnv, arg.name, ValueType(arg.typeName));
    }

    // Typecheck body.
    statements->typeCheck(methodEnv);

    // No longer in a method.
    env.currentMethod = nullopt;

    // This isn't an expression, so return void.
    return voidType;
}

//MethodExp:
MethodExp::MethodExp(ASNPtr&& _method, Vector<ASNPtr>&& _args)
  : method(move(_method)), args(move(_args))
{
}

String MethodExp::toString() const
{
    String str = method->toString();
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

Type MethodExp::typeCheckPrv(TypeEnv& env)
{
    auto varExp = dynamic_cast<VariableExp const*>(method.get());

    if (!varExp) {
        throw std::logic_error("INTERNAL ERROR: bad MethodExp method");
    }

    String objectName = (varExp->object ? *varExp->object : "this");

    // Get class type of method.
    ValueType objectType = lookupVarType(env, objectName);

    // Make overload name.
    Vector<Type> argTypes;

    for (ASNPtr& arg : args)
    {
        Type argType = arg->typeCheck(env);
        argTypes.push_back(argType);
    }

    // Get name for this overload.
    String methodName = funcCanonicalName(varExp->name, argTypes);

    // Get return type.
    MethodType methodType = lookupMethodTypeByClass(env, methodName, objectType);
    return ValueType(methodType.ret());
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

Type NewExp::typeCheckPrv(TypeEnv& env)
{
    ValueType type(typeName);
    validType(env, type);
    // TODO: check args against class constructor (contructor coming soon, sit tite!)
    return type;
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

Type AssignStm::typeCheckPrv(TypeEnv& env)
{
    // RHS expression must match declared type of LHS variable.
    // Final type is void.
    Type lhsType = lhs->typeCheck(env);
    Type rhsType = rhs->typeCheck(env);
    assertTypeIs(rhsType, lhsType);
    return voidType;
}

//VarDecStm:
VarDecStm::VarDecStm(String _typeName, String _name, ASNPtr&& _value)
    : typeName(_typeName), name(_name), value(move(_value))
{
}

String VarDecStm::toString() const
{
    return typeName + " " + name + " = " + value->toString() + ";";
}

Type VarDecStm::typeCheckPrv(TypeEnv& env)
{
    ValueType lhsType(typeName);
    validType(env, lhsType); //make sure that "type" is a declared type
    Type rhsType = value->typeCheck(env);

    if (Type(lhsType) != rhsType)
    {
        throw TypeCheckerException(
                "In declaration of variable '" + name + "' of type '" + 
                lhsType.toString() +
                "' inside class '" + env.currentClass->toString() +
                "':\nRHS expression of type '" + rhsType.toString() +
                "' does not match the expected type.");
    }

    mapNameToType(env, name, lhsType);
    return voidType;
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

Type RetStm::typeCheckPrv(TypeEnv& env)
{
    MethodType methodType = lookupMethodType(env, *env.currentMethod);

    // Make a new type consisting only of the return type
    //   so that it can be correctly compared to mine.
    Type methodRetType = ValueType(methodType.ret());
    Type myRetType     = value->typeCheck(env);

    if (methodRetType == myRetType)
    {
        return myRetType;
    }
    else
    {
        throw TypeCheckerException(
                    "Attempting to return type '" + myRetType.toString() + 
                    "' in method '" + *env.currentMethod 
                    + "' which has return type '" + methodRetType.toString() +
                    "' in class '" + env.currentClass->toString() + "'");
    }
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
    // Entering new class.
    ValueType myType(name);
    ValueType baseType(baseClass);
    declareClass(env, myType);
    env.currentClass = myType;

    if (extends)
    {
        validType(env, baseType); //check if the base class is valid
    }

    for (ASNPtr& member : members)
    {
        member->typeCheck(env);
    }

    // No longer in a class.
    env.currentClass = nullopt;

    // Final type is just the class name.
    return myType;
}

} //namespace dflat
