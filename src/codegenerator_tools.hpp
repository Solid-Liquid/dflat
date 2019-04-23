#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"
#include "map.hpp"
#include "classmeta.hpp"
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

// Have to forward declare to break include cycle.
struct ASN;
using ASNPtr = std::unique_ptr<ASN>; // TODO don't like this code dupe.

struct CodeTypeName     
{ 
    String value; 
    CodeTypeName(String _value)
        : value(std::move(_value))
    {}
};

struct CodeClassDecl
{ 
    String value; 
    CodeClassDecl(String _value)
        : value(std::move(_value))
    {}
};

struct CodeMethodName
{ 
    ValueType objectType;
    CanonName methodName;

    CodeMethodName(ValueType _objectType, CanonName _methodName)
        : objectType(std::move(_objectType))
        , methodName(std::move(_methodName))
    {}
};

struct CodeVarName
{ 
    String value; 
    CodeVarName(String _value)
        : value(std::move(_value))
    {}
};

struct CodeMemberName
{ 
    String value; 
    CodeMemberName(String _value)
        : value(std::move(_value))
    {}
};

struct CodeLiteral
{ 
    String value; 
    CodeLiteral(String _value)
        : value(std::move(_value))
    {}
};

struct CodeNumber
{ 
    int value; 
    CodeNumber(int _value)
        : value(_value)
    {}
};

struct CodeParent
{};

struct CodeTabs
{};

struct CodeTabIn
{};

struct CodeTabOut
{};
        
String codeProlog();
String codeEpilog();
String mangleTypeName(String const&);
String mangleClassDecl(String const&);
String mangleVarName(String const&);
String mangleMemberName(String const&);
String mangleMethodName(ValueType const& objectType, CanonName const& methodName);

class GenEnv
{
    public:
        GenEnv(TypeEnv const&);

        GenEnv& operator<<(CodeTypeName const&);
        GenEnv& operator<<(CodeClassDecl const&);
        GenEnv& operator<<(CodeVarName const&);
        GenEnv& operator<<(CodeMemberName const&);
        GenEnv& operator<<(CodeMethodName const&);
        GenEnv& operator<<(CodeLiteral const&);
        GenEnv& operator<<(CodeNumber const&);
        GenEnv& operator<<(CodeParent const&);
        GenEnv& operator<<(CodeTabs const&);
        GenEnv& operator<<(CodeTabIn const&);
        GenEnv& operator<<(CodeTabOut const&);
        GenEnv& operator<<(ASNPtr const&);
        String concat() const;
        
        void enterClass(ValueType const& classType);
        void leaveClass();
        bool inClass() const;
        ClassMeta const& curClass() const;
 
        void enterMethod(CanonName const& methodName);
        void leaveMethod();
        bool inMethod() const;
        MethodMeta const& curMethod() const;
        MethodMeta const& getMethodMeta(MethodExp const*) const;

        void enterScope();
        void leaveScope();
        void declareLocal(String const& name, ValueType const& type);
        ValueType const& getLocalType(String const& name) const;
        ValueType const* lookupLocalType(String const& name) const;

        void emitMemberVar(ValueType const& objectType, String const& memberName);
        void emitObject(String const& objectName, String const& memberName);

    private:
        std::stringstream& write();
        
        std::stringstream _structDef;
        std::stringstream _funcDef;
        unsigned _tabs = 0;
        ScopeMetaMan _scopes;
        ClassMetaMan _classes;
        MethodMetaMan _methods;
        Optional<MethodMeta> _curMethod;
};

}

#endif // CODEGENERATOR_TOOLS_HPP
