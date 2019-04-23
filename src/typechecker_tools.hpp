#ifndef TYPECHECKER_TOOLS_HPP
#define TYPECHECKER_TOOLS_HPP

#include "type.hpp"
#include "string.hpp"
#include "map.hpp"
#include "set.hpp"
#include "optional.hpp"
#include "vector.hpp"
#include "classmeta.hpp"
#include "scopemeta.hpp"
#include "methodmeta.hpp"

namespace dflat
{

//Macro that makes a dynamic cast look like less of a mess:
#define cast(ptr,type) dynamic_cast<type*>(ptr.get())

/// Struct used for type checking:
class TypeEnv
{
    public:
        TypeEnv();

        void enterClass(ValueType const& classType);
        void leaveClass();
        void addClassVar(String const& name, ValueType const& type);
        void addClassMethod(CanonName const&);
        bool inClass() const;
        ClassMeta const& curClass() const;
 
        void enterMethod(CanonName const&);
        void leaveMethod();
        bool inMethod() const;
        MethodMeta const& curMethod() const;
        void setCanonName(MethodExp const*, CanonName const&);

        void enterScope();
        void leaveScope();
        
        void declareLocal(String const& name, ValueType const& type);
        
        Type lookupRuleType(CanonName const&) const;
        MethodType lookupMethodType(CanonName const&) const;
        MethodType lookupMethodTypeByClass(ValueType const& classType,
                CanonName const&) const;
        ValueType lookupVarType(String const& varName) const;
        ValueType lookupVarTypeByClass(ValueType const& classType,
                String const& varName) const;
        void assertValidType(ValueType const& type) const;

    private:
        void initialize();

        ClassMetaMan _classes;
        ScopeMetaMan _scopes;
        MethodMetaMan _methods;
    
        /// rules - Map of valid rules for how types interact with operators:
        ///     Map: String canonical name -> String expressions return type.
        Map<CanonName,Type> _rules;

        Optional<MethodMeta> _curMethod;

        friend class GenEnv;

};

class TypeCheckerException : public std::exception
{
    public:
        TypeCheckerException(String msg) noexcept;
        const char* what() const noexcept;
    private:
        String message;
};

} //namespace dflat

#endif // TYPECHECKER_TOOLS_HPP
