//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"
#include <memory>

namespace dflat
{

enum ASN_Type { expBinop, expIf, expNumber, expUnaryMinus, expUnaryNot };

enum OpType { opPlus, opMinus, opMult, opDiv, opAnd, opOr, opLogEqual, opLogNotEq };

class ASN
{
public: 
    virtual ~ASN();
    virtual String toString() = 0;
    virtual ASN_Type getType() = 0;
};

using ASN_Ptr = std::unique_ptr<ASN>;

class BinopExp: public ASN
{
public:
    ASN_Ptr left, right;
    OpType op;

    BinopExp(ASN_Ptr&& _left, OpType _op, ASN_Ptr&& _right);
    ASN_Type getType() { return expBinop; }
    String toString();
};

} //namespace dflat

#endif // ASN_HPP
