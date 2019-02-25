//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include "string.hpp"


// #define DECL(name) struct name; using name##Ptr = unique_ptr<name>
// DECL(ASN);
// #undef DECL


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

    using ASNPtr = std::unique_ptr<ASN>;

} //namespace dflat

#endif // ASN_HPP
