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

namespace dflat
{

//Macro that makes a dynamic cast look like less of a mess:
#define cast(ptr,type) dynamic_cast<type*>(ptr.get())

struct MethodMeta
{
    String name;
};

/// Struct used for type checking:
class TypeEnv
{
    public:
        TypeEnv();

        void enterClass(ValueType const& classType);
        void leaveClass();
        void addClassMember(String const& memberName, Type const& memberType);
        ClassMeta const* curClass() const;
 
        void enterMethod(String const& methodName);
        void leaveMethod();
        MethodMeta const* curMethod() const;
        
        Type lookupRuleType(String const& rule) const;
        MethodType lookupMethodType(String const& methodName) const;
        MethodType lookupMethodTypeByClass(String const& methodName, 
                ValueType const& classType) const;
        ValueType lookupVarType(String const& varName);
        ValueType lookupVarTypeByClass(String const& varName, 
            ValueType const& classType);
        bool assertValidType(ValueType const& type);
        void mapNameToType(String const& name, Type const& type);

    private:
        void initialize();
        void declareClass(ValueType const&);

        /// Set of instanciable types (user classes, builtin types).
        Set<ValueType> _types;
    
        ///Map of Maps for classes and their relevant variables:
        /// Map: ValueType class ->
        ///     Map: String variable/function name -> Type
        Map<ValueType, Map<String, Type>> _vars;
    
        ClassMetaMan _classes;
        ScopeMetaMan _scopes;
    
        /// rules - Map of valid rules for how types interact with operators:
        ///     Map: String canonical name -> String expressions return type.
        Map<String,Type> _rules;
        
        /// The canonical name of the method that is currently being typechecked.
        Optional<MethodMeta> _curMethod;

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
