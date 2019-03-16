#include "typechecker.hpp"

namespace dflat
{

TypeEnv initialTypeEnv()
{
#define BINOP binopCanonicalName
#define UNOP unopCanonicalName
    Type const i = intType;
    Type const b = boolType;

    return TypeEnv{
        { 
            // Predefined types ("types" variable)
            intType, 
            boolType, 
            voidType, 
        },
        {
            // Predefined symbols ("rules" variable)
            { BINOP(opPlus,     i, i), i }, // int  +  int  -> int
            { BINOP(opMinus,    i, i), i }, // int  -  int  -> int
            { BINOP(opMult,     i, i), i }, // int  *  int  -> int
            { BINOP(opDiv,      i, i), i }, // int  /  int  -> int
            { BINOP(opLogEq,    i, i), b }, // int  == int  -> bool
            { BINOP(opLogEq,    b, b), b }, // bool == bool -> bool
            { BINOP(opLogNotEq, i, i), b }, // int  != int  -> bool
            { BINOP(opLogNotEq, b, b), b }, // bool != bool -> bool
            { BINOP(opAnd,      i, i), b }, // int  &&  int  -> bool
            { BINOP(opAnd,      b, b), b }, // bool  &&  bool  -> bool
            { BINOP(opOr,       i, i), b }, // int  ||  int  -> bool
            { BINOP(opOr,       b, b), b }, // bool  ||  bool -> bool
            
            { UNOP(opMinus,     i), i }, // -int  -> int
            { UNOP(opNot,       b), b }, // !bool -> bool
        },
        {
            // "variables" variable initialized as empty
        },
        {
            "" // "currentClass" variable initialized as empty string
        }
    };

#undef BINOP
#undef UNOP
}

// Typecheck entire program, returning final environment.
TypeEnv typeCheck(Vector<ASNPtr> const& program)
{
    TypeEnv env = initialTypeEnv();

    for (ASNPtr const& class_ : program)
    {
        //Add current class to the set of types.
        String className = cast(class_,ClassDecl)->name;
        if(lookup(env.types,className))
            throw TypeCheckerException("Redefinition of class: " + className);
        env.types.insert(className);
        env.currentClass = className;

        //Check all types in all classes (ASN->typeCheck runs recursively).
        class_->typeCheck(env);
    }

    return env;
}

// Typecheck a single syntax node, returning its type.
Type typeCheck(ASNPtr const& asn)
{
    TypeEnv env = initialTypeEnv();
    Type type = asn->typeCheck(env);
    //TODO set asn's type here 
    return type;
}

// Look up the rule for an operator based on cannonical name
// Throw if not found.
Type lookupRuleType(TypeEnv const& env, String const& name)
{
    Optional<Type> type = lookup(env.rules, name);

    if (type)
    {
        return *type;
    }
    else
    {
        throw TypeCheckerException("Invalid operands to operator: " + name);
    }
}

//Lookup the return type/arg type(s) of a method by name in current class.
//Throw if method does not exist
Vector<Type> lookupMethodType(TypeEnv const& env, String const& mthd)
{
    return lookupMethodTypeByClass(env,mthd,env.currentClass);
}


//Same as lookupMethodType, but with a specified class.
Vector<Type> lookupMethodTypeByClass(TypeEnv const& env, String const& mthd, String const& clss)
{
    Optional<Map<String,Vector<String>>> classVars = lookup(env.variables, clss);

    if(!classVars)
    {
        throw TypeCheckerException("Invalid reference to class: " + clss);
    }

    Optional<Vector<String>> mthdTypes = lookup(*classVars, mthd);

    if(mthdTypes)
    {
        return *mthdTypes;
    }
    else
    {
        throw TypeCheckerException("Invalid reference to method '" + mthd
                                   + "' in class: " + clss);
    }
}

//Lookup the return type/arg type(s) of a variable by name in current class.
//Throw if method does not exist
Type lookupVarType(TypeEnv const& env, String const& var)
{
    return lookupVarTypeByClass(env,var,env.currentClass);
}


//Same as lookupVarType, but with a specified class.
Type lookupVarTypeByClass(TypeEnv const& env, String const& var, String const& clss)
{
    Optional<Map<String,Vector<String>>> classVars = lookup(env.variables, clss);

    if(!classVars)
    {
        throw TypeCheckerException("Invalid reference to class: " + clss);
    }

    Optional<Vector<String>> varType = lookup(*classVars, var);

    if(varType)
    {
        return (*varType)[0];
    }
    else
    {
        throw TypeCheckerException("Invalid reference to variable '" + var
                                   + "' in class: " + clss);
    }
}

//Check if "type" is a valid type within the set of types.
//Return true if valid, throw if not valid.
bool validType(TypeEnv const& env, String const& type)
{
    if(env.currentClass == type)
    {
        throw TypeCheckerException("Cannot use an instance of a class inside its own definition. Inside class: "
                                   + env.currentClass);
    }

    Optional<Type> valid = lookup(env.types, type);

    if (valid)
    {
        return true;
    }
    else
    {
        throw TypeCheckerException("Invalid reference to unknown type: " + type);
    }
}

// Throw if two types aren't equal.
void assertTypeIs(Type const& test, Type const& against)
{
    if (test != against)
    {
        throw TypeCheckerException(
                "Type '" + test + "' must be '" + against + "'"
                );
    }
}

// Make a canonical name for functions (methods, operators).
// This name can be used to look up a function's return type.
String funcCanonicalName(String const& name, Vector<Type> const& argTypes)
{
    String canonicalName = name + "(";
    bool first = true;

    for (Type const& argType : argTypes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            canonicalName += ",";
        }

        canonicalName += argType;
    }

    canonicalName += ")";
    return canonicalName;
}

String unopCanonicalName(OpType op, Type const& rhsType)
{
    return funcCanonicalName(opString(op), { rhsType });
}

String binopCanonicalName(OpType op, Type const& lhsType, Type const& rhsType)
{
    return funcCanonicalName(opString(op), { lhsType, rhsType });
}

void mapNameToType(TypeEnv& env, String const& name, Vector<Type> const& type)
{
    env.variables[env.currentClass][name] = type;
}


//TypeChecker Exception:
TypeCheckerException::TypeCheckerException(String msg) noexcept
{
    message = "TypeChecker Exception:\n" + msg;
}

const char* TypeCheckerException::what() const noexcept
{
    return message.c_str();
}

} //namespace dflat
