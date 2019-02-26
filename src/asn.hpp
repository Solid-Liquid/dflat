//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>
#include "vector.hpp"

namespace dflat
{

enum ASNType { expBinop, expNumber, expVariable, expUnaryMinus, expUnaryNot,
             blockIf, blockElse, blockMethod, blockWhile, stmAssignment,
             stmMethod, stmDeclaration};

enum OpType { opNull = 0, opPlus, opMinus, opMult, opDiv, opAnd, opOr, opLogEq, opLogNotEq };

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

class UnaryMinusExp : public ASN
{
    //Example Input: -6
    //Example Input: -(5 + 6)
    public:
        ASNPtr nested;

        UnaryMinusExp(ASNPtr&&);
        ASNType getType() const { return expUnaryMinus; }
        String toString() const;
        
        bool operator==(UnaryMinusExp const& other) const
        {
            return nested == other.nested;
        }
        
        DECLARE_CMP(UnaryMinusExp)
};

class UnaryNotExp : public ASN
{
    //Example Input: !var
    //Example Input: !(x == y)
    public:
        ASNPtr nested;

        UnaryNotExp(ASNPtr&&);
        ASNType getType() const { return expUnaryNot; }
        String toString() const;
        
        bool operator==(UnaryNotExp const& other) const
        {
            return nested == other.nested;
        }
        
        DECLARE_CMP(UnaryNotExp)
};

class IfBlock : public ASN
{
    //Example Input: if(x == y) { statement }
    public:
        ASNPtr logicExp;
        std::vector<ASNPtr> statements;

        IfBlock(ASNPtr&&,std::vector<ASNPtr>&&);
        ASNType getType() const { return blockIf; }
        String toString() const;
        
        bool operator==(IfBlock const& other) const
        {
            return logicExp   == other.logicExp
                && statements == other.statements;
        }
        
        DECLARE_CMP(IfBlock)
};

class ElseBlock : public ASN
{
    //Example Input: else { statement }
    public:
        std::vector<ASNPtr> statements;

        ElseBlock(std::vector<ASNPtr>&&);
        ASNType getType() const { return blockElse; }
        String toString() const;
        
        bool operator==(ElseBlock const& other) const
        {
            return statements == other.statements;
        }
        
        DECLARE_CMP(ElseBlock)
};

class WhileBlock : public ASN
{
    //Example Input: while(x == y) { statement }
    public:
        ASNPtr logicExp;
        std::vector<ASNPtr> statements;

        WhileBlock(ASNPtr&&,std::vector<ASNPtr>&&);
        ASNType getType() const { return blockWhile; }
        String toString() const;
        
        bool operator==(WhileBlock const& other) const
        {
            return logicExp   == other.logicExp
                && statements == other.statements;
        }
        
        DECLARE_CMP(WhileBlock)
};

class MethodBlock : public ASN
{
    //Example Input: int func(int x, int y) { statement }
    public:
        String type;
        String name;
        std::vector<ASNPtr> args;
        std::vector<ASNPtr> statements;

        MethodBlock(String,String,std::vector<ASNPtr>&&,std::vector<ASNPtr>&&);
        ASNType getType() const { return blockMethod; }
        String toString() const;
        
        bool operator==(MethodBlock const& other) const
        {
            return type       == other.type
                && name       == other.name
                && args       == other.args
                && statements == other.statements;
        }
        
        DECLARE_CMP(MethodBlock)
};

class MethodStm : public ASN
{
    //Example Input: func(int x, int y)
    public:
        String name;
        std::vector<ASNPtr> args;

        MethodStm(String,std::vector<ASNPtr>&&);
        ASNType getType() const { return stmMethod; }
        String toString() const;
        
        bool operator==(MethodStm const& other) const
        {
            return name == other.name
                && args == other.args;
        }
        
        DECLARE_CMP(MethodStm)
};

class AssignmentStm : public ASN
{
    //Example Input: x = 1 + y
    public:
        String variable;
        ASNPtr expression;

        AssignmentStm(String,ASNPtr&&);
        ASNType getType() const { return stmAssignment; }
        String toString() const;
        
        bool operator==(AssignmentStm const& other) const
        {
            return variable   == other.variable
                && expression == other.expression;
        }
        
        DECLARE_CMP(AssignmentStm)
};

class DeclarationStm : public ASN
{
    //Example Input: int x
    public:
        String type;
        String name;

        DeclarationStm(String,String);
        ASNType getType() const { return stmDeclaration; }
        String toString() const;
        
        bool operator==(DeclarationStm const& other) const
        {
            return type == other.type
                && name == other.name;
        }
        
        DECLARE_CMP(DeclarationStm)
};

} //namespace dflat

#endif // ASN_HPP
