//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include <memory>
#include "string.hpp"
#include "vector.hpp"
#include "variable.hpp"
#include "typechecker_tools.hpp"
#include "codegenerator_tools.hpp"

namespace dflat
{

enum ASNType { expBinop, expNumber, expBool, expVariable, expUnop,
                block, stmIf, defMethod, stmWhile, stmAssign,
                stmMethod, expMethod, stmVarDecAssign, expNew, stmRet,
                declMethod, declClass, expThis, stmVarDec };

enum OpType { opPlus, opMinus, opMult, opDiv, opNot, opAnd, opOr,
                opLogEq, opLogNotEq };

String opString(OpType);

// Type for method definition arguments.
struct FormalArg
{
    ValueType type;
    String name;
};

inline
bool operator ==(FormalArg const& a, FormalArg const& b)
{
    return a.type == b.type
        && a.name == b.name;
}

class ASN
{
    //Base class for all ASN types
    public:
        virtual ~ASN();
        virtual String toString() const = 0;        //Converts to printable string
        virtual ASNType getType() const = 0;        //Returns the ASNType
        virtual bool cmp(ASN const&) const = 0;     //Compares the ASNType
        /**
         * @brief Returns the pointer's ASNType enum.
         * @param TypeEnv
         * @return Type
         */
        Type typeCheck(TypeEnv&);

        bool operator==(ASN const& other) const
        {
            if (getType() != other.getType())
            {
                return false;
            }

            return cmp(other);
        }

        Optional<Type> asnType;

        virtual void generateCode(GenEnv &) const = 0;
    
    private:
        virtual Type typeCheckPrv(TypeEnv&) = 0;
};

// Do this once per ASN subclass.
#define DECLARE_CMP(Type) \
    bool cmp(ASN const& other) const \
    { \
        return *this == static_cast<Type const&>(other); \
    } \
    /*end DECL_CMP*/

using ASNPtr = std::unique_ptr<ASN>;

class Block;
using BlockPtr = std::unique_ptr<Block>;

bool operator==(ASNPtr const&, ASNPtr const&);
bool operator!=(ASNPtr const&, ASNPtr const&);
bool operator==(BlockPtr const&, BlockPtr const&);
bool operator!=(BlockPtr const&, BlockPtr const&);

class VariableExp : public ASN
{
    //Example Input: var
    //Example Input (as member variable): obj.var
    public:
        Optional<String> object;
        String name;

        VariableExp(String); // var or implicit this.member
        VariableExp(String, String); // object, member
        ASNType getType() const { return expVariable; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(VariableExp const& other) const
        {
            return object == other.object
                && name == other.name;
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
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(NumberExp const& other) const
        {
            return value == other.value;
        }

        DECLARE_CMP(NumberExp)
};

class BoolExp : public ASN
{
    //Example Input: false
    public:
        bool value;

        BoolExp(bool);
        ASNType getType() const { return expBool; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(BoolExp const& other) const
        {
            return value == other.value;
        }

        DECLARE_CMP(BoolExp)
};

class BinopExp: public ASN
{
    //Example Input: 5 + 6
    public:
        ASNPtr lhs, rhs;
        OpType op;

        BinopExp(ASNPtr&& _lhs, OpType _op, ASNPtr&& _rhs);
        ASNType getType() const { return expBinop; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(BinopExp const& other) const
        {
            return lhs == other.lhs
                && op  == other.op
                && rhs == other.rhs;
        }

        DECLARE_CMP(BinopExp)
};

class UnopExp : public ASN
{
    //Example Input: !var
    //Example Input: !(x == y)
    public:
        ASNPtr rhs;
        OpType op;

        UnopExp(ASNPtr&&, OpType);
        ASNType getType() const { return expUnop; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(UnopExp const& other) const
        {
            return rhs == other.rhs 
                && op  == other.op;
        }

        DECLARE_CMP(UnopExp)
};

class Block : public ASN
{
    public:
        Vector<ASNPtr> statements;
    
        Block() = default;
        Block(Vector<ASNPtr>&&);
        ASNType getType() const { return block; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(Block const& other) const
        {
            return statements == other.statements;
        }

        DECLARE_CMP(Block)
};

class IfStm : public ASN
{
    //Example Input: if(x == y) { statement } else { statement }
    public:
        ASNPtr   logicExp;
        BlockPtr trueBlock;
        BlockPtr falseBlock;

        IfStm(ASNPtr&&, BlockPtr&&, BlockPtr&&);
        ASNType getType() const { return stmIf; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(IfStm const& other) const
        {
            return logicExp   == other.logicExp
                && trueBlock  == other.trueBlock
                && falseBlock == other.falseBlock;
        }

        DECLARE_CMP(IfStm)
};

class WhileStm : public ASN
{
    //Example Input: while(x == y) { statement }
    public:
        ASNPtr   logicExp;
        BlockPtr body;

        WhileStm(ASNPtr&&, BlockPtr&&);
        ASNType getType() const { return stmWhile; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(WhileStm const& other) const
        {
            return logicExp == other.logicExp
                && body     == other.body;
        }

        DECLARE_CMP(WhileStm)
};

class MethodDef : public ASN
{
    //Example Input: int func(int x, int y) { statement }
    public:
        ValueType retType;
        String name;
        Vector<FormalArg> args;
        BlockPtr body;

        MethodDef(ValueType, String, Vector<FormalArg>&&, BlockPtr&&);
        ASNType getType() const { return defMethod; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(MethodDef const& other) const
        {
            return retType == other.retType
                && name    == other.name
                && args    == other.args
                && body    == other.body;
        }

        DECLARE_CMP(MethodDef)
};

class ConsDef : public ASN
{
    //Example Input: cons(int x, int y) { statements }
    public:
        Vector<FormalArg> args;
        BlockPtr body;

        ConsDef(Vector<FormalArg>&&, BlockPtr&&);
        ASNType getType() const { return defMethod; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(ConsDef const& other) const
        {
            return args == other.args
                && body == other.body;
        }

        DECLARE_CMP(ConsDef)
};

class MethodExp : public ASN
{
    //Example Input: func(var, 1)
    public:
        Variable method;
        Vector<ASNPtr> args;

        MethodExp(Variable, Vector<ASNPtr>&&);
        ASNType getType() const { return expMethod; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(MethodExp const& other) const
        {
            return method == other.method
                && args   == other.args;
        }

        DECLARE_CMP(MethodExp)
};

class MethodStm : public ASN
{
    //Example Input: func(var, 1);
    public:
        ASNPtr methodExp;

        MethodStm(ASNPtr&&);
        ASNType getType() const { return stmMethod; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(MethodStm const& other) const
        {
            return methodExp == other.methodExp;
        }

        DECLARE_CMP(MethodStm)
};

class AssignStm : public ASN
{
    //Example Input: x = 1 + y
    public:
        ASNPtr lhs;
        ASNPtr rhs;

        // first: variable name, second: Expression
        AssignStm(ASNPtr&&, ASNPtr&&);
        ASNType getType() const { return stmAssign; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(AssignStm const& other) const
        {
            return lhs == other.lhs
                && rhs == other.rhs;
        }

        DECLARE_CMP(AssignStm)
};

class VarDecStm : public ASN
{
    //Example Input: int x;
    public:
        ValueType type;
        String name;

        // first: type, second: name, third: exp
        VarDecStm(ValueType, String);
        ASNType getType() const { return stmVarDec; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(VarDecStm const& other) const
        {
            return type == other.type
                && name == other.name;
        }

        DECLARE_CMP(VarDecStm)
};

class VarDecAssignStm : public ASN
{
    //Example Input: int x = 5;
    public:
        ValueType lhsType;
        String lhsName;
        ASNPtr rhs;

        // first: type, second: name, third: exp
        VarDecAssignStm(ValueType, String, ASNPtr&&);
        ASNType getType() const { return stmVarDecAssign; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(VarDecAssignStm const& other) const
        {
            return lhsType == other.lhsType
                && lhsName == other.lhsName
                && rhs     == other.rhs;
        }

        DECLARE_CMP(VarDecAssignStm)
};

class RetStm : public ASN
{
    public:
        ASNPtr value;

        // first: exp
        RetStm(ASNPtr&&);
        ASNType getType() const { return stmRet; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(RetStm const& other) const
        {
            return value == other.value;
        }

        DECLARE_CMP(RetStm)
};

class NewExp : public ASN
{
    //Example Input: new type(exp, exp)
    public:
        ValueType type;
        Vector<ASNPtr> args;

        NewExp(ValueType, Vector<ASNPtr>&&);
        ASNType getType() const { return expNew; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;

        bool operator==(NewExp const& other) const
        {
            return type == other.type
                && args == other.args;
        }

        DECLARE_CMP(NewExp)
};

class ClassDecl : public ASN
{
    public:
        ValueType type;
        Vector<ASNPtr> members;
        Optional<ValueType> parentType;

        ClassDecl(ValueType, Vector<ASNPtr>&&, Optional<ValueType>); 
        ASNType getType() const { return declClass; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &) const;
        
        bool operator==(ClassDecl const& other) const
        {
            return type       == other.type
                && members    == other.members
                && parentType == other.parentType;
        }
        
        DECLARE_CMP(ClassDecl)
};

} //namespace dflat

#endif // ASN_HPP
