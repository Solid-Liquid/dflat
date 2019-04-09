#include "asn.hpp"
#include "typechecker.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

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

Type NumberExp::typeCheckPrv(TypeEnv&)
{
    // Number type is int.
    return intType;
}

Type BoolExp::typeCheckPrv(TypeEnv&)
{
    // Boolean type is bool.
    return boolType;
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

Type UnopExp::typeCheckPrv(TypeEnv& env)
{
    // Look up the type by the canonical name for this operation.
    // e.g. "-1" might be "-(int)" with type int.
    Type rhsType = rhs->typeCheck(env);
    String funcName = unopCanonicalName(op, rhsType);
    return lookupRuleType(env, funcName);
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

Type MethodStm::typeCheckPrv(TypeEnv& env)
{
    // Make sure the call typechecks.
    // Final type is void.
    methodExp->typeCheck(env);
    return voidType;
}

Type NewExp::typeCheckPrv(TypeEnv& env)
{
    ValueType type(typeName);
    validType(env, type);
    // TODO: check args against class constructor (contructor coming soon, sit tite!)
    return type;
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

Type ClassDecl::typeCheckPrv(TypeEnv& env)
{
    // Entering new class.
    ValueType myType(name);
    declareClass(env, myType);
    env.currentClass = myType;

    if (parent)
    {
        ValueType baseType(parent->name);
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
