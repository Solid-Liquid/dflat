//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"

namespace dflat
{

enum OpType { opPlus, opMinus, opMult, opDiv, opAnd, opOr, opLogEqual, opLogNotEq };

enum ExpType { expBinop, expIf, expNumber, expUnaryMinus, expUnaryNot };

class ASN
{
public: 
    virtual ~ASN();
    virtual String toString() = 0;
};

} //namespace dflat

#endif // ASN_HPP
