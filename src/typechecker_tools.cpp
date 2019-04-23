#include "typechecker_tools.hpp"
#include "config.hpp"
#include "typechecker.hpp"
#include <iostream>

namespace dflat
{

TypeEnv::TypeEnv()
{
    initialize();
}

void TypeEnv::enterClass(ValueType const& classType)
{
    if (_classes.lookup(classType))
    {
        throw TypeCheckerException("Duplicate class declaration " 
                + classType.toString());
    }

    _classes.enter(classType);
}

void TypeEnv::setClassParent(ValueType const& parentType)
{
    _classes.setParent(parentType);
}

void TypeEnv::leaveClass()
{
    _classes.leave();
}

void TypeEnv::addClassVar(String const& name, ValueType const& type)
{
    _classes.addVar(name, type);
}

void TypeEnv::addClassMethod(CanonName const& methodName)
{
    _classes.addMethod(methodName);
}

bool TypeEnv::inClass() const
{
    return _classes.cur() != nullptr;
}

ClassMeta const& TypeEnv::curClass() const
{
    if (!_classes.cur())
    {
        throw std::logic_error("no curClass");
    }

    return *_classes.cur();
}

void TypeEnv::enterMethod(CanonName const& methodName)
{
    addClassMethod(methodName);
    _curMethod = MethodMeta{ curClass().type, methodName };
    _scopes.push(); // Argument scope.
    _scopes.declLocal(config::thisName, curClass().type);
    
}

void TypeEnv::leaveMethod()
{
    _scopes.pop();
    _curMethod = nullopt;
}

bool TypeEnv::inMethod() const
{
    return _curMethod != nullopt;
}

MethodMeta const& TypeEnv::curMethod() const
{
    if (!_curMethod)
    {
        throw std::logic_error("no curMethod");
    }

    return *_curMethod;
}

void TypeEnv::setMethodMeta(MethodExp const* exp, 
        ValueType const& objectType, CanonName const& name)
{
    Optional<MemberMeta> member = _classes.lookupMethod(objectType, name);

    if (!member)
    {
        throw std::logic_error("setMethodMeta: no method '" + name.canonName() 
                + "' in '" + objectType.toString() + "'");
    }

    _methods.setMeta(exp, MethodMeta{ member->baseClassType, name });
}

void TypeEnv::enterScope()
{
    _scopes.push();
}

void TypeEnv::leaveScope()
{
    _scopes.pop();
}

void TypeEnv::declareLocal(String const& name, ValueType const& type)
{
    if (!_curMethod)
    {
        throw std::logic_error("declareLocal with no curMethod");
    }

    _scopes.declLocal(name, type);
}

Type TypeEnv::lookupRuleType(CanonName const& name) const
{
    Type const* type = lookup(_rules, name);

    if (type)
    {
        return *type;
    }
    else
    {
        throw TypeCheckerException("Invalid operands to operator: " + name.canonName());
    }
}

MethodType TypeEnv::lookupMethodType(CanonName const& methodName) const
{
    return lookupMethodTypeByClass(_classes.cur()->type, methodName);
}

MethodType TypeEnv::lookupMethodTypeByClass(ValueType const& classType,
        CanonName const& methodName) const
{
    Optional<MemberMeta> member = _classes.lookupMethod(classType, methodName);
    
    if (!member)
    {
        throw TypeCheckerException("Undeclared method '" + methodName.canonName() + "'");
    }
    
    if (!member->type.isMethod())
    {
        throw TypeCheckerException("Referenced method name '" + methodName.canonName()
            + "' in class " + classType.toString() + " is not a method type");
    }

    return member->type.method();
}
            
ValueType TypeEnv::lookupVarType(String const& varName) const
{
    Decl const* decl = _scopes.lookup(varName);

    if (decl && decl->declType == DeclType::local)
    {
        Type varType = decl->type;

        if (!varType.isValue())
        {
            throw TypeCheckerException("Referenced var name '" + varName
                + " is not a variable type");
        }

        return varType.value();
    }
    else
    {
        Decl const* thisDecl = _scopes.lookup(config::thisName);

        if (thisDecl)
        {
            // It's probably our fault if "this" isn't a value type.
            ValueType classType = thisDecl->type.value();
            return lookupVarTypeByClass(classType, varName);
        }
        else
        {
            throw TypeCheckerException("Undeclared var name '" + varName + "'");
        }
    }
}

ValueType TypeEnv::lookupVarTypeByClass(ValueType const& classType,
        String const& memberName) const
{
    Optional<MemberMeta> member = _classes.lookupVar(classType, memberName);

    if (!member)
    {
        throw TypeCheckerException("Undeclared member var name '" + memberName + "'");
    }

    if (!member->type.isValue())
    {
        throw TypeCheckerException("Referenced member var name '" + memberName
            + "' in class " + classType.toString() 
            + " is not a variable type");
    }

    return member->type.value();
}

void TypeEnv::assertValidType(ValueType const& type) const
{
    if (_classes.cur() && _classes.cur()->type == type)
    {
        throw TypeCheckerException("Cannot use an instance of a class inside its own definition. Inside class: " + _classes.cur()->type.toString());
    }

    if (isBuiltinType(type))
    {
        return;
    }

    if (!_classes.lookup(type))
    {
        throw TypeCheckerException("Invalid reference to unknown type: " 
                + type.toString());
    }
}

void TypeEnv::initialize() 
{
    auto binopRule = [&](OpType op, ValueType const& ret, 
            ValueType const& lhs, ValueType const& rhs)
    {
        String name(opString(op));
        MethodType type(ret, { lhs, rhs });
        CanonName canonName(name, type);
        _rules.insert({ canonName, ret });
    };
    
    auto unopRule = [&](OpType op, ValueType const& ret, ValueType const& rhs)
    {
        String name(opString(op));
        MethodType type(ret, { rhs });
        CanonName canonName(name, type);
        _rules.insert({ canonName, ret });
    };

    ValueType const i = intType;
    ValueType const b = boolType;
    ValueType const v = voidType;

    // Predefined function types. 
    binopRule(opPlus,  i, i, i); // int +(int,int)
    binopRule(opMinus, i, i, i);
    binopRule(opMult,  i, i, i);
    binopRule(opDiv,   i, i, i);
    
    binopRule(opLogEq,    b, i, i); // bool ==(int,int)
    binopRule(opLogNotEq, b, i, i);
    
    binopRule(opLogEq,    b, b, b);
    binopRule(opLogNotEq, b, b, b);
   
    binopRule(opAnd,      b, b, b); // bool &&(bool,bool)
    binopRule(opOr,       b, b, b);
    
    binopRule(opAnd,      b, i, i); // bool &&(int,int)
    binopRule(opOr,       b, i, i);

    unopRule(opMinus, i, i); // int -(int)
    unopRule(opNot,   b, b); // bool !(bool)
}

} //namespace dflat
