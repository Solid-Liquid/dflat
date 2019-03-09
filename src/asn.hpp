//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>
#include "vector.hpp"
#include <set.hpp>
#include <map.hpp>

namespace dflat
{

enum ASNType { expBinop, expNumber, expVariable, expTypeVariable, expUnop,
               block, stmIf, defMethod, stmWhile, stmAssign, stmMemberAssign,
               stmMethod, expMethod, stmVarDef, expNew, stmRet, 
               declMethod, declClass };

enum OpType { opPlus, opMinus, opMult, opDiv, opNot, opAnd, opOr,
              opLogEq, opLogNotEq };

String opString(OpType);

struct TypeEnv
{
    //Struct used for Type checking
    Set<String> types;
    Map<String,String> variables;
};

using Type = String;

Type const intType = "int";
Type const boolType = "bool";
Type const voidType = "void";


class ASN
{
public:
    virtual ~ASN();
    virtual String toString() const = 0;
    virtual ASNType getType() const = 0;
    virtual bool cmp(ASN const&) const = 0;
    virtual Type typeCheck(TypeEnv&) const = 0;

    bool operator==(ASN const& other) const
    {
        if (getType() != other.getType())
        {
            return false;
        }

        return cmp(other);
    }
};

// Do this once per ASN subclass.
#define DECLARE_CMP(Type) \
    bool cmp(ASN const& other) const \
    { \
        return *this == static_cast<Type const&>(other); \
    } \
    /*end DECL_CMP*/

using ASNPtr = std::unique_ptr<ASN>;

inline
bool operator==(ASNPtr const& a, ASNPtr const& b)
{
    if (!a && !b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    return *a == *b;
}

class VariableExp : public ASN
{
    //Example Input: var
    public:
        String name;

        VariableExp(String const&);
        ASNType getType() const { return expVariable; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(VariableExp const& other) const
        {
            return name == other.name;
        }

        DECLARE_CMP(VariableExp)
};

class TypeVariableExp : public ASN
{
    //Example Input: int var
    public:
        String type;
        String name;

        TypeVariableExp(String const&, String const&);
        ASNType getType() const { return expTypeVariable; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(TypeVariableExp const& other) const
        {
            return name == other.name && type == other.type;
        }

        DECLARE_CMP(TypeVariableExp)
};

class NumberExp : public ASN
{
    //Example Input: 12
    public:
        int value;

        NumberExp(int);
        ASNType getType() const { return expNumber; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(NumberExp const& other) const
        {
            return value == other.value;
        }

        DECLARE_CMP(NumberExp)
};

class BinopExp: public ASN
{
    //Example Input: 5 + 6
    public:
        ASNPtr lhs, rhs;
        OpType op;

        BinopExp(ASNPtr&& _lhs, OpType _op, ASNPtr&& _rhs);
        ASNType getType() const { return expBinop; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(BinopExp const& other) const
        {
            return lhs == other.lhs
                && op  == other.op
                && rhs == other.rhs;
        }

        DECLARE_CMP(BinopExp)
};

class UnopExp : public ASN
{
    //Example Input: !var
    //Example Input: !(x == y)
    public:
        ASNPtr rhs;
        OpType op;

        UnopExp(ASNPtr&&, OpType);
        ASNType getType() const { return expUnop; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(UnopExp const& other) const
        {
            return rhs == other.rhs 
                && op  == other.op;
        }

        DECLARE_CMP(UnopExp)
};

class Block : public ASN
{
    public:
        Vector<ASNPtr> statements;
        Block() = default;
        Block(Vector<ASNPtr>&&);
        ASNType getType() const { return block; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(Block const& other) const
        {
            return statements == other.statements;
        }

        DECLARE_CMP(Block)
};

class IfStm : public ASN
{
    //Example Input: if(x == y) { statement } else { statement }
    public:
        ASNPtr logicExp;
        ASNPtr trueStatements;
        ASNPtr falseStatements;

        IfStm(ASNPtr&&, ASNPtr&&, ASNPtr&&);
        ASNType getType() const { return stmIf; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(IfStm const& other) const
        {
            return logicExp        == other.logicExp
                && trueStatements  == other.trueStatements
                && falseStatements == other.falseStatements;
        }

        DECLARE_CMP(IfStm)
};

class WhileStm : public ASN
{
    //Example Input: while(x == y) { statement }
    public:
        ASNPtr logicExp;
        ASNPtr statements;

        WhileStm(ASNPtr&&, ASNPtr&&);
        ASNType getType() const { return stmWhile; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(WhileStm const& other) const
        {
            return logicExp   == other.logicExp
                && statements == other.statements;
        }

        DECLARE_CMP(WhileStm)
};

class MethodDef : public ASN
{
    //Example Input: int func(int x, int y) { statement }
    public:
        String type;
        String name;
        Vector<ASNPtr> args;
        ASNPtr statements;

        MethodDef(String, String, Vector<ASNPtr>&&, ASNPtr&&);
        ASNType getType() const { return defMethod; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(MethodDef const& other) const
        {
            return type       == other.type
                && name       == other.name
                && args       == other.args
                && statements == other.statements;
        }

        DECLARE_CMP(MethodDef)
};

class MethodExp : public ASN
{
    //Example Input: func(var, 1)
    public:
        String object;
        String method;
        Vector<ASNPtr> args;

        MethodExp(String, String, Vector<ASNPtr>&&);
        ASNType getType() const { return expMethod; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(MethodExp const& other) const
        {
            return object == other.object
                && method == other.method
                && args   == other.args;
        }

        DECLARE_CMP(MethodExp)
};

class MethodStm : public ASN
{
    //Example Input: func(var, 1);
    public:
        ASNPtr methodExp;

        MethodStm(ASNPtr&&);
        ASNType getType() const { return stmMethod; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(MethodStm const& other) const
        {
            return methodExp == other.methodExp;
        }

        DECLARE_CMP(MethodStm)
};

class AssignStm : public ASN
{
    //Example Input: x = 1 + y
    public:
        String variable;
        ASNPtr expression;

        // first: variable name, second: Expression
        AssignStm(String, ASNPtr&&);
        ASNType getType() const { return stmAssign; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(AssignStm const& other) const
        {
            return variable   == other.variable
                && expression == other.expression;
        }

        DECLARE_CMP(AssignStm)
};

class MemberAssignStm : public ASN
{
    //Example Input: x = 1 + y
    public:
        String object;
        String member;
        ASNPtr expression;

        // first: variable name, second: Expression
        MemberAssignStm(String, String, ASNPtr&&);
        ASNType getType() const { return stmMemberAssign; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(MemberAssignStm const& other) const
        {
            return object     == other.object
                && member     == other.member
                && expression == other.expression;
        }

        DECLARE_CMP(MemberAssignStm)
};

class VarDecStm : public ASN
{
    //Example Input: int x
    public:
        String type;
        String name;
        ASNPtr value;

        // first: type, second: name, third: exp
        VarDecStm(String, String, ASNPtr&&);
        ASNType getType() const { return stmVarDef; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(VarDecStm const& other) const
        {
            return type == other.type
                && name == other.name
                && value == other.value;
        }

        DECLARE_CMP(VarDecStm)
};

class RetStm : public ASN
{
    public:
        ASNPtr value;

        // first: exp
        RetStm(ASNPtr&&);
        ASNType getType() const { return stmRet; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(RetStm const& other) const
        {
            return value == other.value;
        }

        DECLARE_CMP(RetStm)
};

class NewExp : public ASN
{
    //Example Input: new type(exp, exp)
    public:
        String type;
        Vector<ASNPtr> args;

        NewExp(String, Vector<ASNPtr>&&);
        ASNType getType() const { return expNew; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;

        bool operator==(NewExp const& other) const
        {
            return type == other.type
                && args == other.args;
        }

        DECLARE_CMP(NewExp)
};

class MethodDecl : public ASN
{
    //TODO: Delete? This is currently unused. Parser method 'parseMethodDecl()'
    //actually makes an instance of MethodDef. Method declaration currently must
    //include the definition of the method.

    public:
        String type;
        String name;
        Vector<ASNPtr> exps;

        MethodDecl(String, String, Vector<ASNPtr>&&);
        ASNType getType() const { return declMethod; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;
        
        bool operator==(MethodDecl const& other) const
        {
            return type     == other.type
                && name     == other.name
                && exps     == other.exps;
        }
        
        DECLARE_CMP(MethodDecl)
};

class ClassDecl : public ASN
{
    /*
    class name {
        block
    }\n
    */
    public:
        String name;
        Vector<ASNPtr> members;

        ClassDecl(String, Vector<ASNPtr>&&);
        ASNType getType() const { return declClass; }
        String toString() const;
        Type typeCheck(TypeEnv&) const;
        
        bool operator==(ClassDecl const& other) const
        {
            return name   == other.name
                && members == other.members;
        }
        
        DECLARE_CMP(ClassDecl)
};

} //namespace dflat

#endif // ASN_HPP
