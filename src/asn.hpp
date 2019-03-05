//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>
#include "vector.hpp"

namespace dflat
{

enum ASNType { expBinop, expNumber, expVariable, expTypeVariable, expUnop,
               block, stmIf, defMethod, stmWhile, stmAssign, stmMethod,
               expMethod, stmVarDef, expNew, stmRet, declMethod, declClass};

enum OpType { opPlus, opMinus, opMult, opDiv, opNot, opAnd, opOr,
              opLogEq, opLogNotEq };

class ASN
{
public:
    virtual ~ASN();
    virtual String toString() const = 0;
    virtual ASNType getType() const = 0;
    virtual bool cmp(ASN const&) const = 0;

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

class Program
{
    public:
        Vector<ASNPtr> classes;
        //TODO might need to add other stuff
};

class BinopExp: public ASN
{
    //Example Input: 5 + 6
    public:
        ASNPtr left, right;
        OpType op;

        BinopExp(ASNPtr&& _left, OpType _op, ASNPtr&& _right);
        ASNType getType() const { return expBinop; }
        String toString() const;

        bool operator==(BinopExp const& other) const
        {
            return left  == other.left
                && op    == other.op
                && right == other.right;
        }

        DECLARE_CMP(BinopExp)
};

class VariableExp : public ASN
{
    //Example Input: var
    public:
        String name;

        VariableExp(String const&);
        ASNType getType() const { return expVariable; }
        String toString() const;

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

        bool operator==(NumberExp const& other) const
        {
            return value == other.value;
        }

        DECLARE_CMP(NumberExp)
};

class UnopExp : public ASN
{
    //Example Input: !var
    //Example Input: !(x == y)
    public:
        ASNPtr nested;
        OpType op;

        UnopExp(ASNPtr&&, OpType);
        ASNType getType() const { return expUnop; }
        String toString() const;

        bool operator==(UnopExp const& other) const
        {
            return nested == other.nested && op == other.op;
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
        String name;
        Vector<ASNPtr> args;

        MethodExp(String, Vector<ASNPtr>&&);
        ASNType getType() const { return expMethod; }
        String toString() const;

        bool operator==(MethodExp const& other) const
        {
            return name == other.name
                && args == other.args;
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
        ASNType getType() const { return stmAssign; }
        String toString() const;

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
        
        bool operator==(ClassDecl const& other) const
        {
            return name   == other.name
                && members == other.members;
        }
        
        DECLARE_CMP(ClassDecl)
};

} //namespace dflat

#endif // ASN_HPP
