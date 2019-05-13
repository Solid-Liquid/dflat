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
    if (object)
    {
        // Lookup as a member of object.
        ValueType objectType = env.lookupVarType(*object);
        return env.lookupVarTypeByClass(objectType, name);
    }
    else
    {
        // Lookup as a local or a member of the current class.
        return env.lookupVarType(name);
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
        
    if (!lhsType.isValue())
    {
        throw TypeCheckerException("Using method as operator lhs");
    }
    
    if (!rhsType.isValue())
    {
        throw TypeCheckerException("Using method as operator rhs");
    }

    MethodType const type(undefinedType, { lhsType.value(), rhsType.value() });
    CanonName const canonName(opString(op), type);
    return env.lookupRuleType(canonName);
}

Type UnopExp::typeCheckPrv(TypeEnv& env)
{
    // Look up the type by the canonical name for this operation.
    // e.g. "-1" might be "-(int)" with type int.
    Type rhsType = rhs->typeCheck(env);
    
    if (!rhsType.isValue())
    {
        throw TypeCheckerException("Using method as operator rhs");
    }
   
    MethodType const type(undefinedType, { rhsType.value() });
    CanonName const canonName(opString(op), type);
    return env.lookupRuleType(canonName);
}

Type Block::typeCheckPrv(TypeEnv& env)
{
    // Make sure all statements typecheck (in a new scope).
    // Final type is void.
    env.enterScope();

    for (ASNPtr const& stm : statements)
    {
        stm->typeCheck(env);
    }

    env.leaveScope();
    return voidType;
}

Type IfStm::typeCheckPrv(TypeEnv& env)
{
    // Conditional must have bool type.
    // Blocks must typecheck with type void.
    // Final type is void.
    Type condType = logicExp->typeCheck(env);
    env.assertTypeIs(condType, boolType);

    Type trueBlockType = trueStatements->typeCheck(env);
    env.assertTypeIs(trueBlockType, voidType);

    Type falseBlockType = falseStatements->typeCheck(env);
    env.assertTypeIs(falseBlockType, voidType);

    return voidType;
}

Type WhileStm::typeCheckPrv(TypeEnv& env)
{
    // Conditional must have bool type.
    // Body must typecheck with type void.
    // Final type is void.
    Type condType = logicExp->typeCheck(env);
    env.assertTypeIs(condType, boolType);

    Type bodyType = statements->typeCheck(env);
    env.assertTypeIs(bodyType, voidType);

    return voidType;
}

Type PrintStm::typeCheckPrv(TypeEnv& env)
{
    // Print must be int or bool
    // Final type is void.

    Type printType = value->typeCheck(env);
    if(printType!=boolType && printType!=intType)
    {
        throw TypeCheckerException("Argument to print method must be bool or int.");
    }

    return voidType;
}

Type MethodDef::typeCheckPrv(TypeEnv& env)
{
    MethodType methodType(ValueType(retTypeName), {});
    ValueType const curClass = env.curClass().type;

    for (FormalArg const& arg : args)
    {
        ValueType argType(arg.typeName);
        methodType.addArgType(argType);
    }

    CanonName const methodName(name, methodType);
    env.enterMethod(methodName); // New current method

    for (FormalArg const& arg : args)
    {
        env.declareLocal(arg.name, ValueType(arg.typeName));
    }

    // Typecheck body.
    statements->typeCheck(env);

    // No longer in a method.
    env.leaveMethod();

    return methodType;
}

Type ConsDef::typeCheckPrv(TypeEnv& env)
{
    ValueType const curClass = env.curClass().type;
    MethodType consType(curClass, {});

    for (FormalArg const& arg : args)
    {
        ValueType argType(arg.typeName);
        consType.addArgType(argType);
    }

    CanonName const consName(config::consName, consType);
    env.enterMethod(consName); // New current method

    for (FormalArg const& arg : args)
    {
        env.declareLocal(arg.name, ValueType(arg.typeName));
    }

    // Typecheck body.
    statements->typeCheck(env);

    // No longer in a method.
    env.leaveMethod();

    return consType;
}

Type MethodExp::typeCheckPrv(TypeEnv& env)
{
    // If no name, use implicit "this".
    String objectName = (method.object ? *method.object : config::thisName);

    // Get method's class.
    ValueType objectType = env.lookupVarType(objectName);

    // Construct method type to get canonical name.
    // Return type is left undefined.
    Vector<ValueType> argTypes;
    int argNum = 1;

    for (ASNPtr& arg : args)
    {
        Type argType = arg->typeCheck(env);

        if (!argType.isValue())
        {
            throw TypeCheckerException("Passing method as argument " 
                    + to_string(argNum));
        }
        
        argTypes.push_back(argType.value());
        ++argNum;
    }

    MethodType const methodType(undefinedType, argTypes);
    CanonName const methodName(method.variable, methodType);
    
    // Get return type for whatever has this canonical name.
    Type resultType = env.lookupMethodTypeByClass(objectType, methodName).ret();
    env.setMethodMeta(this, objectType, methodName); // Needed later.
    return resultType;
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
    ValueType const type(typeName);
    env.assertValidType(type);

    // Construct method type to get canonical name for constructor.
    Vector<ValueType> argTypes;
    int argNum = 1;

    for (ASNPtr& arg : args)
    {
        Type argType = arg->typeCheck(env);

        if (!argType.isValue())
        {
            throw TypeCheckerException("Passing method as argument " 
                    + to_string(argNum));
        }
        
        argTypes.push_back(argType.value());
        ++argNum;
    }

    MethodType const consType(type, argTypes);
    CanonName const consName(config::consName, consType);

    // Check existence of such a constructor.
    Type resultType = env.lookupMethodTypeByClass(type, consName).ret();

    if (resultType != type)
    {
        throw std::logic_error("Constructor for '" + type.toString()
                + "' returns '" + resultType.toString() + "'");
    }

    env.setMethodMeta(this, type, consName); // Needed later.
    return type;
}

Type AssignStm::typeCheckPrv(TypeEnv& env)
{
    // RHS expression must match declared type of LHS variable.
    // Final type is void.
    Type lhsType = lhs->typeCheck(env);
    Type rhsType = rhs->typeCheck(env);
    env.assertTypeIsOrBase(lhsType, rhsType);
    return voidType;
}

Type VarDecStm::typeCheckPrv(TypeEnv& env)
{
    ValueType lhsType(typeName);
    env.assertValidType(lhsType); //make sure that "type" is a declared type
    
    if (env.inMethod())
    {
        env.declareLocal(name, ValueType(typeName)); 
    }
    else
    {
        env.addClassVar(name, ValueType(typeName));
    }
    
    return voidType;
}

Type VarDecAssignStm::typeCheckPrv(TypeEnv& env)
{
    ValueType lhsType(typeName);
    env.assertValidType(lhsType); //make sure that "type" is a declared type
    Type rhsType = value->typeCheck(env);
    env.assertTypeIsOrBase(lhsType, rhsType);

    //TODO reinstate this helpful error somehow.
//    if (Type(lhsType) != rhsType)
//    {
//        throw TypeCheckerException(
//                "In declaration of variable '" + name + "' of type '" + 
//                lhsType.toString() +
//                "' inside class '" + env.curClass().type.toString() +
//                "':\nRHS expression of type '" + rhsType.toString() +
//                "' does not match the expected type.");
//    }

    if (env.inMethod())
    {
        env.declareLocal(name, ValueType(typeName)); 
    }
    else
    {
        env.addClassVar(name, ValueType(typeName));
    }
    
    return voidType;
}

Type RetStm::typeCheckPrv(TypeEnv& env)
{
    MethodType methodType = env.lookupMethodType(env.curMethod().methodName);

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
                    "' in method '" + env.curMethod().methodName.canonName()
                    + "' which has return type '" + methodRetType.toString() +
                    "' in class '" + env.curClass().type.toString() + "'");
    }
}

Type ClassDecl::typeCheckPrv(TypeEnv& env)
{
    // Entering new class.
    ValueType myType(name);
    env.enterClass(myType);

    if (parent)
    {
        ValueType baseType(parent->name);
        env.assertValidType(baseType); //check if the base class is valid
        env.setClassParent(baseType);
    }
    
    // Typecheck member vars/methods.
    for (ASNPtr& member : members)
    {
        member->typeCheck(env);
    }

    // No longer in a class.
    env.leaveClass();

    // Final type is just the class name.
    return myType;
}

} //namespace dflat
