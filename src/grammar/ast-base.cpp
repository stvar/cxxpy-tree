// Copyright (C) 2016, 2017, 2021  Stefan Vargyas
// 
// This file is part of C++Py-Tree.
// 
// C++Py-Tree is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// C++Py-Tree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with C++Py-Tree.  If not, see <http://www.gnu.org/licenses/>.

#include <functional>
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "ast-base.hpp"

namespace AST {

PoolAllocator::PoolAllocator(
#ifdef DEBUG
    bool _debug,
    const char* _name,
#endif
    size_t _nobjs,
    size_t _pool_sz,
    destr_t _destr)
    noexcept :
#ifdef DEBUG
    debug(_debug),
#endif
    name(_name),
    destr(_destr),
    nobjs(_nobjs),
    pool_sz(_pool_sz),
    nptrs(0)
{
    auto n = nobjs * sizeof(char*);
    auto p = realloc(n + pool_sz);
    if (p == nullptr)
        SYS_UNEXPECT_ERR(
            "<%s> PoolAllocator: out of memory",
            name);
    ptrs = reinterpret_cast<void**>(p);
    ptr = pool = p + n;
}

#ifdef DEBUG

struct named_ptr
{
    named_ptr(const char* _name, const void* _ptr, bool _destr) :
        name(_name), ptr(_ptr), destr(_destr)
    {}
    void print(std::ostream& ost) const
    {
        using namespace std;
        auto w = ost.width();
        auto c = ost.fill();
        auto f = ost.flags();
        if (destr) ost
            << '~';
        ost
            << name << "(0x" << right << hex << setfill('0')
            << setw(UINTPTR_FMT_WIDTH)
            << Ext::ptr_to_int(ptr)
            << setw(w) << setfill(c) << ')' << endl;
        ost.flags(f);
    }
    const char* name;
    const void* ptr;
    bool destr;
};

inline std::ostream& operator<<(std::ostream& ost, const named_ptr& val)
{ val.print(ost); return ost; }

#endif

static inline bool is_linear_find(size_t nptrs)
{ return nptrs < 10; }

bool PoolAllocator::find(const void* what) const
{
    using namespace std;
    if (is_linear_find(nptrs)) {
        auto end = ptrs + nptrs;
        return find_if(ptrs, end, [=](const void* p)
            { return Ext::ptr_to_int(p) == Ext::ptr_to_int(what); }) != end;
    }
    else {
        auto rng = equal_range(
            ptrs, ptrs + nptrs, what, [](const void* p, const void* q)
                { return Ext::ptr_to_int(p) < Ext::ptr_to_int(q); });
        SYS_ASSERT(
            rng.second == rng.first ||
            rng.second == rng.first + 1);
        return rng.first != rng.second;
    }
}

bool PoolAllocator::find(const void* what, size_t& index) const
{
    using namespace std;
    if (is_linear_find(nptrs)) {
        auto end = ptrs + nptrs;
        auto ptr = find_if(ptrs, end, [=](const void* p)
            { return Ext::ptr_to_int(p) == Ext::ptr_to_int(what); });
        if (ptr == end)
            return false;
        index = Ext::ptr_diff(ptr, ptrs);
    }
    else {
        auto rng = equal_range(
            ptrs, ptrs + nptrs, what, [](const void* p, const void* q)
                { return Ext::ptr_to_int(p) < Ext::ptr_to_int(q); });
        SYS_ASSERT(
            rng.second == rng.first ||
            rng.second == rng.first + 1);
        if (rng.second == rng.first)
            return false;
        index = Ext::ptr_diff(rng.first, ptrs);
    }
    return true;
}

PoolAllocator::~PoolAllocator()
    noexcept
{
#ifdef DEBUG
    if (debug) {
        using namespace std;
        transform(
            ptrs, ptrs + nptrs,
            ostream_iterator<named_ptr>(cout),
            [=](const void* ptr)
                { return named_ptr(name, ptr, true); });
        cout << flush;
    }
#endif
    if (destr == nullptr)
        return;
    for (auto p = ptrs + nptrs; p > ptrs; )
        destr(*-- p);
}

void* PoolAllocator::allocate(size_t nbytes, size_t align)
{
    if (nbytes == 0)
        nbytes =  1;
    if (nptrs >= nobjs)
        throw BadAlloc(this, "nobjs");
    align -= Ext::ptr_to_int(ptr) % align;
    if (Ext::max<size_t>() - align < nbytes ||
        nbytes + align > pool_sz ||
        Ext::ptr_diff(ptr, pool) >
        pool_sz - (nbytes + align))
        throw BadAlloc(this, "pool_sz");
    // => ptr + nbytes + align <= pool + pool_sz
    // => ptr + align <= pool + pool_sz
    auto r = ptr + align;
    ptr = r + nbytes;
    ptrs[nptrs ++] = r;
#ifdef DEBUG
    if (debug) {
        using namespace std;
        cout << named_ptr(name, r, false) << flush;
    }
#endif
    return r;
}

void PoolAllocator::rollback(void* obj)
{
    if (nptrs < 1 || ptrs[nptrs - 1] != obj)
        throw BadRollback(this, obj);
    ptr = static_cast<char*>(obj);
    nptrs --;
}

} // namespace AST

