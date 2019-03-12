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
        // Predefined types
        { 
            intType, 
            boolType, 
            voidType, 
        },
        // Predefined symbols
        {
            { BINOP(opPlus,     i, i), i }, // int  +  int  -> int
            { BINOP(opMinus,     i, i), i }, // int  -  int  -> int
            { BINOP(opMult,     i, i), i }, // int  *  int  -> int
            { BINOP(opDiv,     i, i), i }, // int  /  int  -> int
            { BINOP(opLogEq,    i, i), b }, // int  == int  -> bool
            { BINOP(opLogEq,    b, b), b }, // bool == bool -> bool
            { BINOP(opLogNotEq, i, i), b }, // int  != int  -> bool
            { BINOP(opLogNotEq, b, b), b }, // bool != bool -> bool
            { BINOP(opAnd,     i, i), b }, // int  &&  int  -> bool
            { BINOP(opAnd,     b, b), b }, // bool  &&  bool  -> bool
            { BINOP(opOr,     i, i), b }, // int  ||  int  -> bool
            { BINOP(opOr,     b, b), b }, // bool  ||  bool -> bool
            
            { UNOP(opMinus, i), i }, // -int  -> int
            { UNOP(opNot,   b), b }, // !bool -> bool
        },
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

        //Check all types in all classes (ASN->typeCheck runs recursively).
        class_->typeCheck(env);
    }

    return env;
}

// Typecheck a single syntax node, returning its type.
Type typeCheck(ASNPtr const& asn)
{
    TypeEnv env = initialTypeEnv();
    return asn->typeCheck(env);
}

// Look up the type of a name in the environment.
// Throw if not found.
Type lookupType(TypeEnv const& env, String const& name)
{
    Optional<Type> type = lookup(env.variables, name);

    if (type)
    {
        return *type;
    }
    else
    {
        //TODO more information?
        throw TypeCheckerException("Invalid reference to unknown type: " + name);
    }
}

// Introduce a new scope to the type environment.
void pushTypeScope(TypeEnv&)
{
    // TODO
}

// Destroy the current scope in the type environment.
void popTypeScope(TypeEnv&)
{
    // TODO
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
