//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>

namespace dflat
{

enum ASNType { expBinop, expIf, expNumber, expVariable, expUnaryMinus, expUnaryNot };

enum OpType { opPlus, opMinus, opMult, opDiv, opAnd, opOr, opLogEqual, opLogNotEq };

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
public:
    ASNPtr left, right;
    OpType op;

    BinopExp(ASNPtr&& _left, OpType _op, ASNPtr&& _right);
    ASNType getType() const { return expBinop; }
    String toString() const;
};

class VariableExp : public ASN
{
    public:
        String name;

        VariableExp(String const&);
        ASNType getType() const { return expVariable; }
        String toString() const;
};

class NumberExp : public ASN
{
    public:
        int value;
       
        NumberExp(int);
        ASNType getType() const { return expNumber; }
        String toString() const;
};

} //namespace dflat

#endif // ASN_HPP
