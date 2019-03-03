//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>
#include "vector.hpp"

namespace dflat
{

enum ASNType { expBinop, expNumber, expVariable, expUnop,
               block, stmIf, defMethod, stmWhile, stmAssignment,
               expMethod, stmVarDef, expNew, stmRet, declMethod, declClass};

enum OpType { opNull = 0, opPlus, opMinus, opMult, opDiv, opNot, opAnd, opOr,
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
    //Example Input: func(int x, int y)
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

class AssignmentStm : public ASN
{
    //Example Input: x = 1 + y
    public:
        String variable;
        ASNPtr expression;

        AssignmentStm(String, ASNPtr&&);
        ASNType getType() const { return stmAssignment; }
        String toString() const;
        
        bool operator==(AssignmentStm const& other) const
        {
            return variable   == other.variable
                && expression == other.expression;
        }
        
        DECLARE_CMP(AssignmentStm)
};

class VarDefStm : public ASN
{
    //Example Input: int x
    public:
        String type;
        String name;

        VarDefStm(String, String);
        ASNType getType() const { return stmVarDef; }
        String toString() const;
        
        bool operator==(VarDefStm const& other) const
        {
            return type == other.type
                && name == other.name;
        }
        
        DECLARE_CMP(VarDefStm)
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
