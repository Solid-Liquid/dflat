//ASN - Abstract Syntax Node Class and derived classes.
//Used to form an AST - abstract syntax tree.

#ifndef ASN_HPP
#define ASN_HPP

#include <memory>
#include "string.hpp"
#include "vector.hpp"
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
    TypeName typeName;
    String name;
};

inline
bool operator ==(FormalArg const& a, FormalArg const& b)
{
    return a.typeName == b.typeName
        && a.name     == b.name;
}

// Type for object.member expressions.
struct ObjectMember
{
    String object;
    String member;
};

inline
bool operator ==(ObjectMember const& a, ObjectMember const& b)
{
    return a.object == b.object
        && a.member == b.member;
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

        virtual void generateCode(GenEnv &) = 0;
    
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

inline
bool operator==(ASNPtr const& a, ASNPtr const& b)
{
    if (!a && !b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    return *a == *b;
}

class VariableExp : public ASN
{
    //Example Input: var
    //Example Input (as member variable): obj.var
    public:
        Optional<String> object;
        String name;

        VariableExp(String const&); // var or implicit this.member
        VariableExp(String const&, String const&); // object, member
        ASNType getType() const { return expVariable; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

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
        void generateCode(GenEnv &);

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
        void generateCode(GenEnv &);

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
        void generateCode(GenEnv &);

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
        void generateCode(GenEnv &);

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
        void generateCode(GenEnv &);

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
        ASNPtr logicExp;
        ASNPtr trueStatements;
        bool hasFalse;          //check if there is an else{} block
        ASNPtr falseStatements;

        IfStm(ASNPtr&&, ASNPtr&&, bool, ASNPtr&&);
        ASNType getType() const { return stmIf; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

        bool operator==(IfStm const& other) const
        {
            return logicExp        == other.logicExp
                && trueStatements  == other.trueStatements
                && hasFalse        == other.hasFalse
                && falseStatements == other.falseStatements;
        }

        DECLARE_CMP(IfStm)
};

class WhileStm : public ASN
{
    //Example Input: while(x == y) { statement }
    public:
        ASNPtr logicExp;
        ASNPtr statements;

        WhileStm(ASNPtr&&, ASNPtr&&);
        ASNType getType() const { return stmWhile; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

        bool operator==(WhileStm const& other) const
        {
            return logicExp   == other.logicExp
                && statements == other.statements;
        }

        DECLARE_CMP(WhileStm)
};

class MethodDef : public ASN
{
    //Example Input: int func(int x, int y) { statement }
    public:
        String retTypeName;
        String name;
        Vector<FormalArg> args;
        ASNPtr statements;

        MethodDef(String, String, Vector<FormalArg>&&, ASNPtr&&);
        ASNType getType() const { return defMethod; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

        bool operator==(MethodDef const& other) const
        {
            return retTypeName == other.retTypeName
                && name       == other.name
                && args       == other.args
                && statements == other.statements;
        }

        DECLARE_CMP(MethodDef)
};

class MethodExp : public ASN
{
    //Example Input: func(var, 1)
    public:
        ASNPtr method;
        Vector<ASNPtr> args;

        MethodExp(ASNPtr&&, Vector<ASNPtr>&&);
        ASNType getType() const { return expMethod; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

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
        void generateCode(GenEnv &);

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
        void generateCode(GenEnv &);

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
        String typeName;
        String name;

        // first: type, second: name, third: exp
        VarDecStm(String, String);
        ASNType getType() const { return stmVarDec; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

        bool operator==(VarDecStm const& other) const
        {
            return typeName == other.typeName
                && name == other.name;
        }

        DECLARE_CMP(VarDecStm)
};

class VarDecAssignStm : public ASN
{
    //Example Input: int x = 5;
    public:
        String typeName;
        String name;
        ASNPtr value;

        // first: type, second: name, third: exp
        VarDecAssignStm(String, String, ASNPtr&&);
        ASNType getType() const { return stmVarDecAssign; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

        bool operator==(VarDecAssignStm const& other) const
        {
            return typeName == other.typeName
                && name == other.name
                && value == other.value;
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
        void generateCode(GenEnv &);

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
        String typeName;
        Vector<ASNPtr> args;

        NewExp(String, Vector<ASNPtr>&&);
        ASNType getType() const { return expNew; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);

        bool operator==(NewExp const& other) const
        {
            return typeName == other.typeName
                && args == other.args;
        }

        DECLARE_CMP(NewExp)
};

class ClassDecl : public ASN
{
    /*
    class name              (optional): extends baseClass
    {
        block
    };
    */
    public:
        String name;
        Vector<ASNPtr> members;
        ClassDecl* parent;

        ClassDecl(String, Vector<ASNPtr>&&, ClassDecl*); 
        ASNType getType() const { return declClass; }
        String toString() const;
        Type typeCheckPrv(TypeEnv&);
        void generateCode(GenEnv &);
        
        bool operator==(ClassDecl const& other) const
        {
            return name   == other.name
                && members == other.members
                && parent == other.parent;
        }
        
        DECLARE_CMP(ClassDecl)
};

} //namespace dflat

#endif // ASN_HPP
