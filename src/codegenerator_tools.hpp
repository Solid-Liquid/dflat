#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"
#include "map.hpp"
#include "classmeta.hpp"
#include "variable.hpp"
#include "scopemeta.hpp"
#include "methodmeta.hpp"
#include "typechecker_tools.hpp"
#include "set.hpp"
#include <sstream>
#include "optional.hpp"
#include "vector.hpp"
#include "type.hpp"
#include <memory>

namespace dflat
{

//// Have to forward declare to break include cycle.
//struct ASN;
//using ASNPtr = std::unique_ptr<ASN>;
//
//struct Block;
//using BlockPtr = std::unique_ptr<Block>;
//
//// Something like "struct T*"
//struct CodeTypeName     
//{ 
//    ValueType type; 
//    CodeTypeName(ValueType _type)
//        : type(std::move(_type))
//    {}
//};
//
//// Something like "T"
//struct CodeClassDecl
//{ 
//    ValueType type; 
//    CodeClassDecl(ValueType _type)
//        : type(std::move(_type))
//    {}
//};
//
//struct CodeMethodName
//{ 
//    ValueType objectType;
//    CanonName methodName;
//
//    CodeMethodName(ValueType _objectType, CanonName _methodName)
//        : objectType(std::move(_objectType))
//        , methodName(std::move(_methodName))
//    {}
//};
//
//// Constructor
//// objectType is the CanonName's type's return type.
//struct CodeConsName
//{ 
//    CanonName consName;
//
//    CodeConsName(CanonName _consName)
//        : consName(std::move(_consName))
//    {}
//};
//    
//// Something like "dfv_T"
//struct CodeVTableName
//{
//    ValueType classType;
//
//    CodeVTableName(ValueType _classType)
//        : classType(std::move(_classType))
//    {}
//};
//
//// Something like "dfvm_f_int_int"
//struct CodeVTableMethodName
//{ 
//    CanonName methodName;
//
//    CodeVTableMethodName(CanonName _methodName)
//        : methodName(std::move(_methodName))
//    {}
//};
//
//struct CodeVarName
//{ 
//    String value; 
//    CodeVarName(String _value)
//        : value(std::move(_value))
//    {}
//};
//
//struct CodeMemberName
//{ 
//    String value; 
//    CodeMemberName(String _value)
//        : value(std::move(_value))
//    {}
//};
//
//struct CodeLiteral
//{ 
//    String value; 
//    CodeLiteral(String _value)
//        : value(std::move(_value))
//    {}
//};
//
//struct CodeNumber
//{ 
//    int value; 
//    CodeNumber(int _value)
//        : value(_value)
//    {}
//};
//
//struct CodeParent
//{};
//
//struct CodeTabs
//{};
//
//struct CodeTabIn
//{};
//
//struct CodeTabOut
//{};
//        
//String mangleTypeName(ValueType const&);
//String mangleClassDecl(ValueType const&);
//String mangleVarName(String const&);
//String mangleMemberName(String const&);
//String mangleMethodName(ValueType const& objectType, CanonName const& methodName);
//String mangleConsName(CanonName const&);
//String mangleVTableName(ValueType const&);
//String mangleVTableMethodName(CanonName const&);
//
class GenEnv
{
    public:
        GenEnv(TypeEnv const&);
//
//        GenEnv& operator<<(CodeTypeName const&);
//        GenEnv& operator<<(CodeClassDecl const&);
//        GenEnv& operator<<(CodeVarName const&);
//        GenEnv& operator<<(CodeMemberName const&);
//        GenEnv& operator<<(CodeMethodName const&);
//        GenEnv& operator<<(CodeConsName const&);
//        GenEnv& operator<<(CodeVTableName const&);
//        GenEnv& operator<<(CodeVTableMethodName const&);
//        GenEnv& operator<<(CodeLiteral const&);
//        GenEnv& operator<<(CodeNumber const&);
//        GenEnv& operator<<(CodeParent const&);
//        GenEnv& operator<<(CodeTabs const&);
//        GenEnv& operator<<(CodeTabIn const&);
//        GenEnv& operator<<(CodeTabOut const&);
//        GenEnv& operator<<(ASNPtr const&);
//        GenEnv& operator<<(BlockPtr const&);
//
        String prolog() const;
        String epilog() const;
        String concat() const;
        
//        void enterClass(ValueType const& classType);
//        void leaveClass();
//        bool inClass() const;
//        ClassMeta const& curClass() const;
//        void instantiate(ValueType const& classType);
// 
//        void enterMethod(CanonName const& methodName);
//        void leaveMethod();
//        bool inMethod() const;
//        MethodMeta const& curMethod() const;
//        MethodMeta const& getMethodMeta(ASN const*) const;
//        Set<CanonName> getClassMethods(ValueType const& classType) const;
//        Set<CanonName> getAllMethods() const;
//
//        void startBlock();
//        void endBlock();
//        void enterScope();
//        void leaveScope();
//        void declareLocal(String const& name, ValueType const& type);
//        ValueType const& getLocalType(String const& name) const;
//        ValueType const* lookupLocalType(String const& name) const;
//
//        void emitMemberVar(ValueType const& objectType, String const& memberName);
////        void emitObject(String const& objectName, String const& memberName);
//        void emitMethod(CanonName const& methodName, 
//                ValueType const& retType, Vector<FormalArg> const& args,
//                Vector<ASNPtr> const& body, bool isCons);
//        void emitConstructor(MethodType const& consType, 
//                Vector<FormalArg> const& args, Vector<ASNPtr> const& body);

    private:
        std::stringstream& write();
        
        std::stringstream _structDef;
        std::stringstream _funcDef;
        unsigned _methodTabs = 0;
        unsigned _classTabs = 0;
//        ScopeMetaMan _scopes;
//        ClassMetaMan _classes;
//        MethodMetaMan _methods;
        Optional<MethodMeta> _curMethod;
};

}

#endif // CODEGENERATOR_TOOLS_HPP
