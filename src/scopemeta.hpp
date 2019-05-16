#pragma once

#include "string.hpp"
#include "map.hpp"
#include "vector.hpp"
#include "type.hpp"
#include "optional.hpp"

namespace dflat
{

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

class ScopeMetaMan
{
    public:
        void push();
        void pop();
        void declLocal(String const&, Type const&);
//        void print() const;
        Decl const* lookup(String const&) const;

    private:
        void declAny(String const&, Decl const&);
        
        Vector<Map<String, Decl>> _scopes = {{}};
};

} // namespace dflat
