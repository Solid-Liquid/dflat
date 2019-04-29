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

void TypeEnv::setMethodMeta(ASN const* node, 
        ValueType const& objectType, CanonName const& name)
{
    Optional<MemberMeta> member = _classes.lookupMethod(objectType, name);

    if (!member)
    {
        throw std::logic_error("setMethodMeta: no method '" + name.canonName() 
                + "' in '" + objectType.toString() + "'");
    }

    _methods.setMeta(node, MethodMeta{ member->baseClassType, name });
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
        throw TypeCheckerException("Undeclared method '" 
                + methodName.canonName() + "' in class '" 
                + classType.toString() + "'");
    }
    
    if (!member->type.isMethod())
    {
        throw TypeCheckerException("Referenced method name '" 
                + methodName.canonName() + "' in class " 
                + classType.toString() + " is not a method type");
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

void TypeEnv::declareClass(ValueType const& type, ClassDecl* decl)
{
    if (_classes.lookupClassDecl(type))
    {
        throw TypeCheckerException("Already declared class '" 
                + type.name() + "'");
    }

    _classes.newClassDecl(type, decl);
}

// Saves and clears the current class context.
// Note: Must be very careful that this doesn't mess up any state.
TypeEnv::SavedClass TypeEnv::saveClass()
{
    SavedClass c;

    ClassMeta const* cur = _classes.cur();

    if (cur)
    {
        c.type = cur->type;
        _classes.leave();
    }

    c.tvars = std::move(_tvars);
    _tvars.clear();

    return c;
}

// Restores the current class context.
void TypeEnv::restoreClass(TypeEnv::SavedClass const& c)
{
    if (_classes.cur())
    {
        throw std::logic_error("restoreClass when in class");
    }

    if (c.type)
    {
        _classes.enter(*c.type);
    }

    _tvars = std::move(c.tvars);
}

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

void TypeEnv::assertTypeIs(Type const &test, Type const &against) const
{
    if (test == against)
    {
        return;
    }

    throw TypeCheckerException(
        "Type '" + test.toString() + 
        "' must be '" + against.toString() + "'"
        );
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
