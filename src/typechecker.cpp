#include "typechecker.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{
TypeEnv initialTypeEnv() {
    TypeEnv env;

    auto predefined = [&](ValueType const& t)
    {
        env.types.insert(t);
    };

    auto binopRule = [&](OpType op, Type const& ret, 
            Type const& lhs, Type const& rhs)
    {
        String name = binopCanonicalName(op, lhs, rhs);
        env.rules.insert({ name, ret });
    };
    
    auto unopRule = [&](OpType op, Type const& ret, Type const& rhs)
    {
        String name = unopCanonicalName(op, rhs);
        env.rules.insert({ name, ret });
    };

    ValueType const i = intType;
    ValueType const b = boolType;
    ValueType const v = voidType;

    // Initialize the global namespace
    env.variables.insert({ config::globalClass, {} });
    
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

    return env;
}

// Typecheck entire program, returning final environment.
TypeEnv typeCheck(Vector<ASNPtr> const &program)
{
    TypeEnv env = initialTypeEnv();

    for (ASNPtr const &class_ : program)
    {
        class_->typeCheck(env);
    }

    return env;
}

// Typecheck a single syntax node, returning its type.
Type typeCheck(ASNPtr const &asn)
{
    TypeEnv env = initialTypeEnv();
    return asn->typeCheck(env);
}

Type lookupRuleType(TypeEnv const &env, String const &name)
{
    Type const* type = lookup(env.rules, name);

    if (type)
    {
        return *type;
    }
    else
    {
        throw TypeCheckerException("Invalid operands to operator: " + name);
    }
}

MethodType lookupMethodType(TypeEnv const &env, String const &mthd)
{
    return lookupMethodTypeByClass(env, mthd, env.classes.cur()->type);
}

MethodType lookupMethodTypeByClass(TypeEnv const &env, String const &mthd, ValueType const &clss)
{
    Map<String, Type> const* classVars = lookup(env.variables, clss);

    if (!classVars)
    {
        throw std::logic_error(String(__func__) + ": bad class lookup " 
                + clss.toString());
    }

    Type const* mthdType = lookup(*classVars, mthd);

    if (!mthdType)
    {
        throw TypeCheckerException("Invalid reference to method '" 
                + mthd + "' in class: " + clss.toString());
    }
    
    if (!mthdType->isMethod())
    {
        throw TypeCheckerException("Referenced name '" + mthd
            + "' in class " + clss.toString() + " is not a method type");
    }
    
    return mthdType->method();
}

ValueType lookupVarType(TypeEnv const &env, String const &var)
{
    if (var == "this")
    {
        return ValueType(env.classes.cur()->type);
    }
    else
    {
        return lookupVarTypeByClass(env, var, env.classes.cur()->type);
    }
}

ValueType lookupVarTypeByClass(TypeEnv const &env, String const &var, ValueType const &clss)
{
    Map<String, Type> const* classVars = lookup(env.variables, clss);

    if (!classVars)
    {
        throw std::logic_error(String(__func__) + ": bad class lookup " 
                + clss.toString());
    }

    Type const* varType = lookup(*classVars, var);

    if (!varType)
    {
        throw TypeCheckerException("Invalid reference to variable '" 
                + var + "' in class: " + clss.toString());
    }
    
    if (!varType->isValue())
    {
        throw TypeCheckerException("Referenced name '" + var
            + "' in class " + clss.toString() + " is not a variable type");
    }

    return varType->value();
}

bool validType(TypeEnv const &env, ValueType const &type)
{
    if (env.classes.cur()->type == type)
    {
        throw TypeCheckerException("Cannot use an instance of a class inside its own definition. Inside class: " + env.classes.cur()->type.toString());
    }

    ValueType const* valid = lookup(env.types, type);

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

void assertTypeIs(Type const &test, Type const &against)
{
    if (test != against)
    {
        throw TypeCheckerException(
            "Type '" + test.toString() + 
            "' must be '" + against.toString() + "'"
            );
    }
}

// Note that this function can produce a canonical name for an illegal type.
// This is for the sake of decent errors. That is, while only ValueTypes
// are allowed as arguments, this takes any Type.
String funcCanonicalName(String const &name, Vector<Type> const &argTypes)
{
    String canonicalName = name + "(";
    bool first = true;

    for (Type const &argType : argTypes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            canonicalName += ",";
        }

        canonicalName += argType.toString();
    }

    canonicalName += ")";
    return canonicalName;
}

String unopCanonicalName(OpType op, Type const &rhsType)
{
    return funcCanonicalName(opString(op), { rhsType });
}

String binopCanonicalName(OpType op, Type const &lhsType, Type const &rhsType)
{
    return funcCanonicalName(opString(op), { lhsType, rhsType });
}

void mapNameToType(TypeEnv &env, String const &name, Type const &type)
{
    ValueType clss = env.classes.cur() ? env.classes.cur()->type
                                       : config::globalClass;

    Map<String, Type>* classVars = lookup(env.variables, clss);

    if (!classVars)
    {
        throw std::logic_error(String(__func__) + ": bad class lookup " 
                + clss.toString());
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
        std::cout << "map " << clss.toString() << "::" 
            << name << " to " << type.toString() << "\n";
    }
}

void declareClass(TypeEnv& env, ValueType const& clss)
{
    auto it = env.variables.find(clss);

    if (it == env.variables.end())
    {
        env.types.insert(clss);
        env.variables.insert({ clss, {} });

        if (config::traceTypeCheck)
        {
            std::cout << "declare class " << clss.toString() << "\n";
        }
    }
    else
    {
        throw TypeCheckerException("Duplicate class declaration " 
                + clss.toString());
    }
}

TypeCheckerException::TypeCheckerException(String msg) noexcept
{
    message = "TypeChecker Exception:\n" + msg;
}

const char *TypeCheckerException::what() const noexcept
{
    return message.c_str();
}

} //namespace dflat
