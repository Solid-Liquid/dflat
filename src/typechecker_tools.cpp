#include "typechecker_tools.hpp"
#include "config.hpp"
#include "typechecker.hpp"
#include <iostream>

namespace dflat
{

TypeCheckerException::TypeCheckerException(String msg) noexcept
    : std::runtime_error("TypeChecker Exception:\n" + std::move(msg))
{}


TypeEnv::TypeEnv()
{
    initialize();
}

Type TypeEnv::getType(CanonName const& name) const
{
    Type const* type = _meta.lookupType(name);

    if (type)
    {
        return *type;
    }

    throw TypeCheckerException("Undefined '" + name.toString() + "'");
}

TClass TypeEnv::getClassType(CanonName const& name) const
{
    Type type = getType(name);
    TClass* t = type.as<TClass>();

    if (t)
    {
        return *t;
    }
     
    throw TypeCheckerException("'" + name.toString() 
            + "' does not have class type");
}

TMethod TypeEnv::getMethodType(CanonName const& name) const
{
    Type type = getType(name);
    TMethod* t = type.as<TMethod>();

    if (t)
    {
        return *t;
    }
    
    throw TypeCheckerException("'" + name.toString() 
            + "' does not have method type");
}

TFunc TypeEnv::getFuncType(CanonName const& name) const
{
    Type type = getType(name);
    TFunc* t = type.as<TFunc>();

    if (t)
    {
        return *t;
    }
        
    throw TypeCheckerException("'" + name.toString() 
            + "' does not have function type");
}

Type TypeEnv::getReturnType(CanonName const& name) const
{
    Type type = getType(name);
    TMethod* m = type.as<TMethod>();

    if (m)
    {
        return m->ret();
    }

    TFunc* f = type.as<TFunc>();

    if (f)
    {
        return f->ret();
    }

    throw TypeCheckerException("'" + name.toString() 
            + "' does not have a return type");
}

bool TypeEnv::inMethod() const
{
    return _meta.curMethod() != nullopt;
}

ClassMeta TypeEnv::getCurClass() const
{
   Optional<ClassMeta> m = _meta.curClass();

   if (!m)
   {
       // TODO possibly should be a logic_error
       throw TypeCheckerException("Not in a class");
   }

   return *m;
}

MethodMeta TypeEnv::getCurMethod() const
{
    Optional<MethodMeta> m = _meta.curMethod();

    if (!m)
    {
       // TODO possibly should be a logic_error
        throw TypeCheckerException("Not in a method");
    }

    return *m;
}

void TypeEnv::assertTypeIs(Type const& t1, Type const& t2) const
{
    if (t1 != t2)
    {
        throw TypeCheckerException("Type '" + t1.toString()
                + "' must be equal to '" + t2.toString() + "'");
    }
}

void TypeEnv::assertTypeIsOrBase(Type const& t1, Type const& t2) const
{
    if (t1 != t2)
    {
        if (!_meta.isTypeBaseOf(t1, t2))
        {
            throw TypeCheckerException("Type '" + t1.toString()
                    + "' must be equal to or a base of '" + t2.toString() 
                    + "'");
        }
    }
}

Scope TypeEnv::newScope()
{
    return _meta.newScope();
}

void TypeEnv::leaveScope(Scope const& scope)
{
    _meta.leaveScope(scope);
}

static
void assertConcrete(Type const& type)
{
    if (!type.isConcrete())
    {
        throw TypeCheckerException("Type '" + type.toString()
                + "' is not a concrete type");
    }
}

void TypeEnv::registerClass(TClass const& type, ClassDecl* node)
{
    //TODO error if duplicate
    _meta.registerClass(type, node);
}

Scope TypeEnv::newMethod(CanonName const& name, Type const& type)
{
    //TODO error if duplicate
    assertConcrete(type);
    return _meta.newMethod(name, type);
}

void TypeEnv::leaveMethod(Scope const& scope)
{
    _meta.leaveMethod(scope);
}

void TypeEnv::newMethodCall(ASN* node, CanonName const& name)
{
    _meta.newMethodCall(node, name);
}

void TypeEnv::newMember(CanonName const& name, Type const& type)
{
    //TODO error if duplicate
    assertConcrete(type);
    _meta.newMember(name, type);
}

void TypeEnv::newLocal(CanonName const& name, Type const& type)
{
    //TODO error if duplicate
    assertConcrete(type);
    _meta.newLocal(name, type);
}

void TypeEnv::realize(Type const& type)
{
    //TODO this
    assertConcrete(type);
}

void TypeEnv::initialize()
{
    auto binop = [&](OpType op, Type const& ret, 
            Type const& lhs, Type const& rhs)
    {
        CanonName opName = canonizeOp(op, { lhs, rhs });
        TFunc opType(ret, { lhs, rhs });
        _meta.newFunc(opName, opType);
    };
    
    auto unop = [&](OpType op, Type const& ret, 
            Type const& rhs)
    {
        CanonName opName = canonizeOp(op, { rhs });
        TFunc opType(ret, { rhs });
        _meta.newFunc(opName, opType);
    };

    Type const i = intType;
    Type const b = boolType;
    Type const v = voidType;

    // Predefined function types. 
    binop(opPlus,  i, i, i); // int +(int,int)
    binop(opMinus, i, i, i);
    binop(opMult,  i, i, i);
    binop(opDiv,   i, i, i);
    
    binop(opLogEq,    b, i, i); // bool ==(int,int)
    binop(opLogNotEq, b, i, i);
    
    binop(opLogEq,    b, b, b);
    binop(opLogNotEq, b, b, b);
   
    binop(opAnd,      b, b, b); // bool &&(bool,bool)
    binop(opOr,       b, b, b);
    
    binop(opAnd,      b, i, i); // bool &&(int,int)
    binop(opOr,       b, i, i);

    unop(opMinus, i, i); // int -(int)
    unop(opNot,   b, b); // bool !(bool)
}

/*
// TODO possibly make these private
void TypeEnv::enterClass(ValueType const& classType)
{
    if (_classes.lookup(classType))
    {
        throw TypeCheckerException("Duplicate class declaration " 
                + classType.toString());
    }

    _classes.declare(classType);
    _classes.enter(classType);
    
    // Add a default constructor.
    addClassMethod(
        CanonName(
            config::consName,
            MethodType(curClass().type, {})
            )
        );
}

void TypeEnv::setClassParent(ValueType const& parentType)
{
    _classes.setParent(parentType);
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

// If t is a bound typevar, returns the type u that t maps to.
// Otherwise returns whatever t is.
ValueType TypeEnv::mappedType(ValueType const& t) const
{
    if (!t.tvars().empty())
    {
        // This can't be a type variable.
        return t;
    }

    ValueType const* u = lookup(_tvars, t.name());

    if (u)
    {
        // It was a type variable. Here's the real type.
        return *u;
    }
    else
    {
        // Either an unbound type variable or just a normal type.
        // If unbound, error will surface eventually.
        return t;
    }
}

// Instantiate a class type with concrete type variables.
void TypeEnv::instantiate(ValueType const& type)
{
    std::cout << "DEBUG: instantiating " << type.toString() << "\n";
    ClassDecl* decl = _classes.lookupClassDecl(type);
    
    if (!decl)
    {
        throw TypeCheckerException("Cannot instantiate unknown type '"
                + type.toString() + "'");
    }
    
    // Instanciate parent first if necessary.
    if (decl->parentType)
    {
        assertValidType(*decl->parentType);
    }
   
    // Save and leave the current class context.
    auto oldClass = saveClass();

    // Map the type vars.
    Vector<ValueType> const formalTVars = decl->type.tvars();
    Vector<ValueType> const actualTVars = type.tvars();

    if (formalTVars.size() != actualTVars.size())
    {
        throw TypeCheckerException("Cannot instantiate '"
                + decl->type.toString() + "' as '"
                + type.toString() + "'");
    }

    for (unsigned i = 0; i < formalTVars.size(); ++i)
    {
        if (!formalTVars[i].tvars().empty())
        {
            throw TypeCheckerException("Type variable '"
                    + formalTVars[i].toString()
                    + "' of class '"
                    + decl->type.toString()
                    + "' is not a simple type variable");
        }

        _tvars.insert({ formalTVars[i].name(), actualTVars[i] });
    }

    // Set the new class context.
    enterClass(type);

    // Process parent class.
    if (decl->parentType)
    {
        setClassParent(*decl->parentType);
    }
    
    // Typecheck members.
    for (ASNPtr& member : decl->members)
    {
        member->typeCheck(*this);
    }

    // Clear the class context.
    leaveClass();

    // Restore the old class context.
    restoreClass(oldClass);
    
    std::cout << "DEBUG: done instantiating " << type.toString() << "\n";
}

void TypeEnv::assertValidType(ValueType const& type)
{
    if (isBuiltinType(type))
    {
        // It's an int or something.
        return;
    }

    if (!_classes.lookup(type))
    {
        if (type.tvars().empty())
        {
            ValueType const* mappedType = lookup(_tvars, type.name());

            if (mappedType)
            {
                // It's a type var that maps to some real type. Rerun on that.
                assertValidType(*mappedType);
            }
            else
            {
                // It's really a type we've never heard of.
                throw TypeCheckerException("Invalid reference to unknown type: " 
                        + type.toString());
            }
        }
        else
        {
            // It's a parametric type.
            instantiate(type);
        }
    }
}

void TypeEnv::assertTypeIsOrBase(Type const& t1, Type const& t2) const
{
    if (t1 == t2)
    {
        return;
    }

    if (t1.isValue() && t2.isValue())
    {
        // Testing base/derived only makes sense with ValueTypes.
        ValueType const t1v = t1.value();
        ValueType const t2v = t2.value();
        ClassMeta const* meta1 = _classes.lookup(t1v);
        ClassMeta const* meta2 = _classes.lookup(t2v);

        if (meta1)
        {
            while (meta2)
            {
                if (!meta2->parent)
                {
                    break;
                }

                if (*meta2->parent == t1v)
                {
                    return;
                }

                meta2 = _classes.lookup(*meta2->parent);
            }
        }
    }

    throw TypeCheckerException(
        "Type '" + t1.toString() + 
        "' must be '" + t2.toString() + "' or a base class of it"
        );
}

*/
} //namespace dflat
