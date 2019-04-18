#ifndef CODEGENERATOR_TOOLS_HPP
#define CODEGENERATOR_TOOLS_HPP
#include "string.hpp"
#include "map.hpp"
#include "classmeta.hpp"
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

struct CodeTypeName     { String value; };
struct CodeMethodName   { String value; };
struct CodeVarName      { String value; };
struct CodeMemberName   { String value; };
struct CodeLiteral      { String value; };
struct CodeNumber       { int value; };
struct CodeParent       {};
struct CodeTabs         {};
struct CodeTabIn        {};
struct CodeTabOut       {};

enum class DeclType
{
    local,
    member,
    method,
};

struct Decl
{
    DeclType declType;  
    Type type;
};

class GenEnv
{
    public:
        GenEnv& operator<<(CodeTypeName const&);
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
        String prolog() const;
        String concat() const;
       
        ClassMetaMan classes;
        Vector<Map<String, Decl>> scopes = {{}};
        Optional<String> curFunc;

    private:
        std::stringstream& write();
        String mangleTypeName(String const&);
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
