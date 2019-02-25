#ifndef EXP_HPP
#define EXP_HPP

namespace dlfat
{

enum OpType { opPlus, opMinus, opMult, opDiv, opAnd, opOr, opLogEqual, opLogNotEq };

enum ExpType { expBinop, expIf, expNumber, expUnaryMinus, expUnaryNot };

class AST
{
public:
    AST();
};

} //namespace dflat

#endif // EXP_HPP
