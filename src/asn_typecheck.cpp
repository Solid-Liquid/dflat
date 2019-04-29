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
        CanonName const objectName = canonizeLocal(*object);
        TClass const objectType = env.getClassType(objectName);
        CanonName const memberName = canonizeMember(name, objectType);
        return env.getType(memberName);
    }
    else
    {
        // Lookup as a local or a member of the current class.
        CanonName const varName = canonizeLocal(name);
        return env.getType(varName);
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
   
    CanonName const opName = canonizeOp(op, { lhsType, rhsType });
    return env.getReturnType(opName);
}

Type UnopExp::typeCheckPrv(TypeEnv& env)
{
    // Look up the type by the canonical name for this operation.
    // e.g. "-1" might be "-(int)" with type int.
    Type rhsType = rhs->typeCheck(env);
    
    CanonName const opName = canonizeOp(op, { rhsType });
    return env.getReturnType(opName);
}

Type Block::typeCheckPrv(TypeEnv& env)
{
    // Make sure all statements typecheck (in a new scope).
    // Final type is void.
    Scope s = env.newScope();

    for (ASNPtr const& stm : statements)
    {
        stm->typeCheck(env);
    }

    env.leaveScope(s);
    return voidType;
}

Type IfStm::typeCheckPrv(TypeEnv& env)
{
    // Conditional must have bool type.
    // Blocks must typecheck with type void.
    // Final type is void.
    Type condType = logicExp->typeCheck(env);
    env.assertTypeIs(condType, boolType);

    Type trueBlockType = trueBlock->typeCheck(env);
    env.assertTypeIs(trueBlockType, voidType);

    Type falseBlockType = falseBlock->typeCheck(env);
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

    Type bodyType = body->typeCheck(env);
    env.assertTypeIs(bodyType, voidType);

    return voidType;
}

Type MethodDef::typeCheckPrv(TypeEnv& env)
{
    ClassMeta const curClass = env.getCurClass();
    Vector<Type> argTypes;
    
    for (FormalArg const& arg : args)
    {
        argTypes.push_back(arg.type);
    }
    
    TMethod const methodType(curClass.type, retType, argTypes);
    CanonName const methodName = canonizeMethod(name, methodType);
    Scope scope = env.newMethod(methodName, methodType);

    for (FormalArg const& arg : args)
    { 
        CanonName const argName = canonizeLocal(arg.name);
        env.newLocal(argName, arg.type);
    }

    body->typeCheck(env);
    env.leaveMethod(scope);
    return methodType;
}

Type ConsDef::typeCheckPrv(TypeEnv& env)
{
    ClassMeta const curClass = env.getCurClass();
    Vector<Type> argTypes;

    for (FormalArg const& arg : args)
    {
        argTypes.push_back(arg.type);
    }

    TMethod const consType(curClass.type, curClass.type, argTypes);
    CanonName const consName = canonizeCons(consType);
    Scope scope = env.newMethod(consName, consType);

    for (FormalArg const& arg : args)
    {
        CanonName const argName = canonizeLocal(arg.name);
        env.newLocal(argName, arg.type);
    }

    body->typeCheck(env);
    env.leaveMethod(scope);
    return consType;
}

Type MethodExp::typeCheckPrv(TypeEnv& env)
{
    // If no name, use implicit "this".
    CanonName const objectName = canonizeLocal(
        method.object ? *method.object : config::thisName
        );

    // Get class we're calling method on.
    TClass const objectType = env.getClassType(objectName);

    // Construct a canonical name for this call.
    Vector<Type> argTypes;

    for (ASNPtr& arg : args)
    {
        argTypes.push_back(arg->typeCheck(env));
    }
    
    // Return type isn't known/needed to look up.
    TMethod const methodType(objectType, TUndefined{}, argTypes);
    CanonName const methodName = canonizeMethod(method.variable, methodType);
    env.newMethodCall(this, methodName); // Needed later.
    return env.getReturnType(methodName);
}

Type MethodStm::typeCheckPrv(TypeEnv& env)
{
    methodExp->typeCheck(env);
    return voidType; // Statements are void type.
}

Type NewExp::typeCheckPrv(TypeEnv& env)
{
    env.realize(type);

    // Construct method type to get canonical name for constructor.
    Vector<Type> argTypes;

    for (ASNPtr& arg : args)
    {
        argTypes.push_back(arg->typeCheck(env));
    }
    
    // Return type isn't needed.
    TMethod const consType(type, TUndefined{}, argTypes);
    CanonName const consName = canonizeCons(consType);
    env.newMethodCall(this, consName); // Needed later.

    // Check this exists.
    Type const retType = env.getReturnType(consName);

    if (retType != type)
    {
        throw std::logic_error("Constructor for '" + type.toString()
                + "' returns '" + retType.toString() + "'");
    }

    return retType;
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
    env.realize(type);
    
    if (env.inMethod())
    {
        CanonName const varName = canonizeLocal(name);
        env.newLocal(varName, type); 
    }
    else
    {
        ClassMeta const curClass = env.getCurClass();
        CanonName const memberName = canonizeMember(name, curClass.type);
        env.newMember(memberName, type);
    }
    
    return voidType;
}

Type VarDecAssignStm::typeCheckPrv(TypeEnv& env)
{
    env.realize(lhsType);
    Type rhsType = rhs->typeCheck(env);
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
        CanonName const varName = canonizeLocal(lhsName);
        env.newLocal(varName, lhsType); 
    }
    else
    {
        ClassMeta const curClass = env.getCurClass();
        CanonName const memberName = canonizeMember(lhsName, curClass.type);
        env.newMember(memberName, lhsType);
    }
    
    return voidType;
}

Type RetStm::typeCheckPrv(TypeEnv& env)
{
    MethodMeta const curMethod = env.getCurMethod();
    ClassMeta const curClass = env.getCurClass();

    // Confirm return types match.
    Type const declaredRet = curMethod.type.ret();
    Type const actualRet = value->typeCheck(env);

    if (declaredRet == actualRet)
    {
        return declaredRet;
    }

    throw TypeCheckerException(
            "Attempting to return type '" + actualRet.toString() 
            + "' in method '" + curMethod.name.toString()
            + "' which has return type '" + declaredRet.toString() 
            + "' in class '" + curClass.type.toString() + "'");
}

Type ClassDecl::typeCheckPrv(TypeEnv& env)
{
    env.registerClass(type, this);
    return type;
}

} //namespace dflat
