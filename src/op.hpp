#ifndef OP_HPP
#define OP_HPP

#include "string.hpp"
#include <memory>

namespace dflat
{

enum OpType { opPlus, opMinus, opMult, opDiv };

class Op
{
public:
    virtual ~Op();
    virtual OpType getType() const = 0;
    virtual operator String() const = 0;
    virtual String toString() const = 0;
};

using OpPtr = std::unique_ptr<Op>;

class PlusOp : public Op
{
    OpType getType() const { return opPlus; }
    operator String() const { return "+"; }
    String toString() const { return "+"; }
};

class MinusOp : public Op
{
    OpType getType() const { return opMinus; }
    operator String() const { return "-"; }
    String toString() const { return "-"; }
};

class MultOp : public Op
{
    OpType getType() const { return opMult; }
    operator String() const { return "*"; }
    String toString() const { return "*"; }
};

class DivOp : public Op
{
    OpType getType() const { return opDiv; }
    operator String() const { return "/"; }
    String toString() const { return "/"; }
};


} //namespace dflat
#endif // OP_HPP
