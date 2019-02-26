//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>


// #define DECL(name) struct name; using name##Ptr = unique_ptr<name>
// DECL(ASN);
// #undef DECL


namespace dflat
{

enum ASNType { expBinop, expIf, expNumber, expVariable, expUnaryMinus, expUnaryNot };

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
    bool cmp(ASN const& other) const override \
    { \
        return *this == static_cast<Type const&>(other); \
    } \
    /*end DECL_CMP*/



using ASNPtr = std::unique_ptr<ASN>;

class BinopExp: public ASN
{
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
    
    DECLARE_CMP(BinopExp);
};

class VariableExp : public ASN
{
public:
    String name;

    VariableExp(String const&);
    ASNType getType() const { return expVariable; }
    String toString() const;

    bool operator==(VariableExp const& other) const
    {
        return name == other.name;
    }
    
    DECLARE_CMP(VariableExp);
};

class NumberExp : public ASN
{
public:
    int value;
   
    NumberExp(int);
    ASNType getType() const { return expNumber; }
    String toString() const;

    bool operator==(NumberExp const& other) const
    {
        return value == other.value;
    }
    
    DECLARE_CMP(NumberExp);
};

} //namespace dflat

#endif // ASN_HPP
