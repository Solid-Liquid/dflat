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
        void declAny(String const&, Decl const&);
//        void declMethod(String const&, Type const&);
        void declLocal(String const&, Type const&);
        void print() const;
        Optional<Decl> lookup(String const&) const;
       

    private:
        Vector<Map<String, Decl>> _scopes = {{}};
};

} // namespace dflat
