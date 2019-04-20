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
    declareClass(classType);
    _classes.enter(classType);
}

void TypeEnv::leaveClass()
{
    _classes.leave();
}

void TypeEnv::addClassMember(String const& name, Type const& memberType)
{
    _classes.addMember(name, memberType);
}

ClassMeta const* TypeEnv::curClass() const
{
    return _classes.cur();
}

void TypeEnv::enterMethod(String const& name)
{
    _curMethod = MethodMeta{};
    _curMethod->name = name;
}

void TypeEnv::leaveMethod()
{
    _curMethod = nullopt;
}

MethodMeta const* TypeEnv::curMethod() const
{
    return &*_curMethod;
}

Type TypeEnv::lookupRuleType(String const& name) const
{
    Type const* type = lookup(_rules, name);

    if (type)
    {
        return *type;
    }
    else
    {
        throw TypeCheckerException("Invalid operands to operator: " + name);
    }
}

MethodType TypeEnv::lookupMethodType(String const& methodName) const
{
    return lookupMethodTypeByClass(methodName, _classes.cur()->type);
}

MethodType TypeEnv::lookupMethodTypeByClass(String const& methodName, 
        ValueType const& classType) const
{
    Map<String, Type> const* classVars = lookup(_vars, classType);

    if (!classVars)
    {
        throw std::logic_error(String(__func__) + ": bad class lookup " 
                + classType.toString());
    }

    Type const* methodType = lookup(*classVars, methodName);

    if (!methodType)
    {
        throw TypeCheckerException("Invalid reference to method '" 
                + methodName + "' in class: " + classType.toString());
    }
    
    if (!methodType->isMethod())
    {
        throw TypeCheckerException("Referenced name '" + methodName
            + "' in class " + classType.toString() + " is not a method type");
    }
    
    return methodType->method();
}

ValueType TypeEnv::lookupVarType(String const& varName)
{
    if (varName == "this")
    {
        return ValueType(_classes.cur()->type);
    }
    else
    {
        return lookupVarTypeByClass(varName, _classes.cur()->type);
    }
}

ValueType TypeEnv::lookupVarTypeByClass(String const& varName, 
        ValueType const& classType)
{
    Map<String, Type> const* classVars = lookup(_vars, classType);

    if (!classVars)
    {
        throw std::logic_error(String(__func__) + ": bad class lookup " 
                + classType.toString());
    }

    Type const* varType = lookup(*classVars, varName);

    if (!varType)
    {
        throw TypeCheckerException("Invalid reference to variable '" 
                + varName + "' in class: " + classType.toString());
    }
    
    if (!varType->isValue())
    {
        throw TypeCheckerException("Referenced name '" + varName
            + "' in class " + classType.toString() 
            + " is not a variable type");
    }

    return varType->value();
}

bool TypeEnv::assertValidType(ValueType const& type)
{
    if (_classes.cur() && _classes.cur()->type == type)
    {
        throw TypeCheckerException("Cannot use an instance of a class inside its own definition. Inside class: " + _classes.cur()->type.toString());
    }

    ValueType const* valid = lookup(_types, type);

    if (valid)
    {
        return true;
    }
    else
    {
        throw TypeCheckerException("Invalid reference to unknown type: " 
                + type.toString());
    }
}

void TypeEnv::mapNameToType(String const& name, Type const& type)
{
    ValueType classType = _classes.cur() ? _classes.cur()->type 
                                         : config::globalClass;

    Map<String, Type>* classVars = lookup(_vars, classType);

    if (!classVars)
    {
        throw std::logic_error(String(__func__) + ": bad class lookup " 
                + classType.toString());
    }

    // Insert OR replace (this might be a new scope).
    auto it = classVars->find(name);

    if (it == classVars->end())
    {
        classVars->insert({ name, type });
    }
    else
    {
        it->second = type;
    }
    
    if (config::traceTypeCheck)
    {
        std::cout << "map " << classType.toString() << "::" 
            << name << " to " << type.toString() << "\n";
    }
}

void TypeEnv::initialize() 
{
    auto predefined = [&](ValueType const& t)
    {
        _types.insert(t);
    };

    auto binopRule = [&](OpType op, Type const& ret, 
            Type const& lhs, Type const& rhs)
    {
        String name = binopCanonicalName(op, lhs, rhs);
        _rules.insert({ name, ret });
    };
    
    auto unopRule = [&](OpType op, Type const& ret, Type const& rhs)
    {
        String name = unopCanonicalName(op, rhs);
        _rules.insert({ name, ret });
    };

    ValueType const i = intType;
    ValueType const b = boolType;
    ValueType const v = voidType;

    // Initialize the global namespace
    _vars.insert({ config::globalClass, {} });
    
    // Predefined declarable types. 
    predefined(i);
    predefined(b);
    predefined(v);

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

void TypeEnv::declareClass(ValueType const& classType)
{
    auto it = _vars.find(classType);

    if (it == _vars.end())
    {
        _types.insert(classType);
        _vars.insert({ classType, {} });

        if (config::traceTypeCheck)
        {
            std::cout << "declare class " << classType.toString() << "\n";
        }
    }
    else
    {
        throw TypeCheckerException("Duplicate class declaration " 
                + classType.toString());
    }
}


} //namespace dflat
