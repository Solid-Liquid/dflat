//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>
#include <vector>

namespace dflat
{

enum ASNType { expBinop, expNumber, expVariable, expUnop,
             blockIf, blockElse, blockMethod, blockWhile, stmAssignment,
             stmMethod, stmDeclaration};

enum OpType { opNull = 0, opPlus, opMinus, opMult, opDiv, opNot, opAnd, opOr, opLogEq, opLogNotEq };

class ASN
{
public: 
    virtual ~ASN();
    virtual String toString() const = 0;
    virtual ASNType getType() const = 0;
};

using ASNPtr = std::unique_ptr<ASN>;

class BinopExp: public ASN
{
    //Example Input: 5 + 6
    public:
        ASNPtr left, right;
        OpType op;

        BinopExp(ASNPtr&& _left, OpType _op, ASNPtr&& _right);
        ASNType getType() const { return expBinop; }
        String toString() const;
};

class VariableExp : public ASN
{
    //Example Input: var
    public:
        String name;

        VariableExp(String const&);
        ASNType getType() const { return expVariable; }
        String toString() const;
};

class NumberExp : public ASN
{
    //Example Input: 12
    public:
        int value;
       
        NumberExp(int);
        ASNType getType() const { return expNumber; }
        String toString() const;
};

class UnopExp : public ASN
{
    //Example Input: -6
    //Example Input: -(5 + 6)
    //Example Input: !var
    //Example Input: !(x == y)
    public:
        ASNPtr nested;
        OpType op;

        UnopExp(ASNPtr&&, OpType _op);
        ASNType getType() const { return expUnop; }
        String toString() const;
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
};

class ElseBlock : public ASN
{
    //Example Input: else { statement }
    public:
        std::vector<ASNPtr> statements;

        ElseBlock(std::vector<ASNPtr>&&);
        ASNType getType() const { return blockElse; }
        String toString() const;
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
};

} //namespace dflat

#endif // ASN_HPP
