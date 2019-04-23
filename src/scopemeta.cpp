#include "scopemeta.hpp"
#include <iostream>

namespace dflat
{

void ScopeMetaMan::push()
{
    _scopes.push_back({});
}

void ScopeMetaMan::pop()
{
    _scopes.pop_back();
}

void ScopeMetaMan::declAny(String const& name, Decl const& decl)
{
    _scopes.back().insert({name, decl});
}

//void ScopeMetaMan::declMethod(CanonName const& name, Type const& type)
//{
//    Decl decl{ DeclType::method, type };
//    declAny(name, decl);
//}

void ScopeMetaMan::declLocal(String const& name, Type const& type)
{
    Decl decl{ DeclType::local, type };
    declAny(name, decl);
}

void ScopeMetaMan::print() const
{
    std::cout << "scope_print\n";
    unsigned tab = 1;
    for (auto&& s : _scopes)
    {
        for (auto&& [k,v] : s)
        {
            std::cout << String(tab*2, ' ') << k << "\n";
        }
        ++tab;
    }
}

Decl const* ScopeMetaMan::lookup(String const& name) const
{
    auto it = _scopes.rbegin();
    auto const end = _scopes.rend();

    while (it != end)
    {
        if (Decl const* decl = dflat::lookup(*it, name))
        {
            return decl;
        }

        ++it;
    }

    return nullptr;
}

} // namespace dflat
