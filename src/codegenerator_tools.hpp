#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"
#include "map.hpp"
#include "classmeta.hpp"
#include "scopemeta.hpp"
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
    String value; 
    CodeMethodName(String _value)
        : value(std::move(_value))
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

struct CodeMethodTabs
{};

struct CodeTabIn
{};

struct CodeTabOut
{};
        
extern String const codeProlog;

class GenEnv
{
    public:
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
        ScopeMetaMan scopes;
        ClassMetaMan classes;
        Optional<String> curFunc;

        void emitMember(ValueType const& objectType, String const& memberName);
        void emitObject(String const& objectName, String const& memberName);

    private:
        std::stringstream& write();
        String mangleTypeName(String const&);
        String mangleClassDecl(String const&);
        String mangleVarName(String const&);
        String mangleMemberName(String const&);
        String mangleMethodName(String const&);
        
        std::stringstream _structDef;
        std::stringstream _funcDef;
        std::stringstream _main;
        unsigned _tabs = 0;
};

}

#endif // CODEGENERATOR_TOOLS_HPP
