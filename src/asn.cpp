#include "asn.hpp"

namespace dflat
{

String opString(OpType op)
{
    switch(op)
    {
        case opPlus: return "+";
        case opMinus: return "-";
        case opDiv: return "/";
        case opMult: return "*";
        case opAnd: return "&&";
        case opOr: return "||";
        case opLogEqual: return "==";
        case opLogNotEq: return "!=";
        default: return "oopsie";
    }
}


//ASN:
ASN::~ASN()
{
}

//BinopExp:
BinopExp::BinopExp(ASN_Ptr&& _left, OpType _op, ASN_Ptr&& _right)
    :left(std::move(_left)),right(std::move(_right)),op(_op)
{
}

String BinopExp::toString()
{
    return "(" + left->toString() + " " + opString(op) +
            " " + right->toString() + ")";
}

} //namespace dflat
