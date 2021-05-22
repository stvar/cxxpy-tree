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

#ifndef __AST_BASE_HPP
#define __AST_BASE_HPP

#include "config.h"

#include <cstddef>

#include <new>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <utility>

#include "sys.hpp"
#include "std-ext.hpp"

namespace AST {

class PoolAllocator : private Sys::mem_t
{
public:
    typedef void (*constr_t)(void*, unsigned short);
    typedef void (*destr_t)(void*);

    class iterator : public std::iterator<std::input_iterator_tag, void*>
    {
    public:
        iterator& operator++()
        { ptr ++; return *this; };
        iterator operator++(int)
        { auto r = *this; ptr ++; return r; }

        void* operator*() const
        { return *ptr; }

        bool operator==(const iterator& v) const
        { return ptr == v.ptr; }
        bool operator!=(const iterator& v) const
        { return ptr != v.ptr; }

    private:
        iterator(void** _ptr) : ptr(_ptr) {}
        friend class PoolAllocator;

        void** ptr;
    };

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

    PoolAllocator(PoolAllocator&&) = delete;
    PoolAllocator& operator=(PoolAllocator&&) = delete;

    PoolAllocator(
#ifdef DEBUG
        bool _debug,
#endif
        const char* _name,
        size_t _nobjs,
        size_t _pool_sz,
        constr_t _constr = nullptr,
        destr_t _destr = nullptr) noexcept;
    ~PoolAllocator() noexcept;

    class BadAlloc : public std::bad_alloc
    {
    public:
        BadAlloc(const PoolAllocator* alloc, const char* what) : msg(Sys::sformat(
            "<%s> PoolAllocator exceeded %s limits [nobjs=%zu, pool_sz=%zu]",
            alloc->name, what, alloc->nobjs, alloc->pool_sz))
        {}
        ~BadAlloc() noexcept
        {}
        const char* what() const noexcept
        { return msg.c_str(); }

    private:
        std::string msg;
    };

    class BadRollback : public std::exception
    {
    public:
        BadRollback(const PoolAllocator* alloc, void* ptr) : msg(Sys::sformat(
            "<%s> PoolAllocator bad rollback [ptr=" UINTPTR_FORMAT "]",
            alloc->name, Ext::ptr_to_int(ptr)))
        {}
        ~BadRollback() noexcept
        {}
        const char* what() const noexcept { return msg.c_str(); }

    private:
        std::string msg;
    };

    template<typename T>
    void* allocate() throw (BadAlloc)
    { return allocate(sizeof(T), alignof(T)); }

    template<typename T>
    void* allocate(size_t n) throw (BadAlloc)
    { return allocate(sizeof(T) * n, alignof(T[0])); }

    void rollback(void* obj) throw (BadRollback);
    bool find(const void* what) const;
    bool find(const void* what, size_t&) const;
    size_t size() const { return nptrs; }

    iterator begin() const { return iterator(ptrs); }
    iterator end() const   { return iterator(ptrs + nptrs); }

protected:
#ifdef DEBUG
    bool is_debug() const { return debug; }
#endif
    const char* pool_name() const { return name; }
    void** base() const { return ptrs; }

private:
    void* allocate(size_t nbytes, size_t align) throw (BadAlloc);

#ifdef DEBUG
    bool        debug;
#endif
    const char *name;
    constr_t    constr;
    destr_t     destr;
    size_t      nobjs;
    size_t      pool_sz;
    char       *pool;
    char       *ptr;
    void      **ptrs;
    size_t      nptrs;
};

template<typename R, typename T = const R*>
struct Arg;

template<typename T>
struct base_enum_t
{
    typedef T self_t;

    static const char* name(size_t type)
    {
        SYS_ASSERT(type < Ext::array_size(self_t::names));
        return self_t::names[type];
    }

    static bool lookup(const char* name, size_t& type)
    {
        using namespace std;
        const auto size = Ext::array_size(self_t::names);
        const auto beg = self_t::names;
        const auto end = beg + size;
        const auto ptr = find_if(beg, end,
            [=](const char* p) { return strcmp(p, name) == 0; });
        if (ptr == end)
            return false;
        type = Ext::ptr_diff(ptr, beg);
        SYS_ASSERT(type < size);
        return true;
    }
};

template <typename R>
class BaseAST
{
protected:
    typedef R root_t;

    BaseAST(const BaseAST&) = delete;
    BaseAST& operator=(const BaseAST&) = delete;

    BaseAST(BaseAST&&) = delete;
    BaseAST& operator=(BaseAST&&) = delete;

    BaseAST(
#ifdef DEBUG
        bool _debug,
#endif
        size_t _node_nobjs, size_t _node_pool_sz,
        size_t _raw_nobjs, size_t _raw_pool_sz,
        PoolAllocator::constr_t _node_constr,
        PoolAllocator::destr_t _node_destr
    ) :
        node_alloc(
#ifdef DEBUG
            _debug,
#endif
            "node",
            _node_nobjs,
            _node_pool_sz,
            _node_constr,
            _node_destr),
        raw_alloc(
#ifdef DEBUG
            _debug,
#endif
            "raw",
            _raw_nobjs,
            _raw_pool_sz),
        root(nullptr)
    {}

    ~BaseAST()
    {}

    size_t num_node() const { return node_alloc.size(); }

    static void node_constr(void *ptr, unsigned short id) noexcept
    { static_cast<root_t*>(ptr)->n = id + 1; }

    static void node_destr(void *ptr) noexcept
    { static_cast<root_t*>(ptr)->~root_t(); }

    template<typename T, typename... A>
    const T* new_node(A&&... args)
    {
        auto p = node_alloc.allocate<T>();
        try         { return new (p) T{std::forward<A>(args)...}; }
        catch (...) { node_alloc.rollback(p); throw; }
    }

    friend struct Arg<root_t, const char*>;
    friend struct Arg<root_t, const root_t*>;

    class node_iterator;

    node_iterator node_begin() const;
    node_iterator node_end() const;

    template<typename Op>
    class op_node_iterator;

    PoolAllocator node_alloc;
    PoolAllocator raw_alloc;
    const root_t *root;
};

typedef Ext::widest_uint_t result_t;

template<typename T>
struct ResultCast;

template<>
struct ResultCast<result_t>
{
    typedef result_t target_t;

    static target_t cast(result_t val)
    { return val; } // stev: NOP
};

template<>
struct ResultCast<unsigned>
{
    typedef unsigned target_t;

    static target_t cast(result_t val)
    { return Ext::integer_cast<target_t>(val); }

    static result_t cast(target_t val)
    { return Ext::integer_cast<result_t>(val); }
};

template<typename T>
struct ResultCast<T*>
{
    typedef T* target_t;

    static target_t cast(result_t val)
    { return Ext::int_to_ptr<T>(Ext::integer_cast<uintptr_t>(val)); }

    static result_t cast(target_t val)
    { return Ext::integer_cast<result_t>(Ext::ptr_to_int(val)); }
};

template <typename R>
class BaseAST<R>::node_iterator :
    public std::iterator<std::input_iterator_tag, const R>
{
public:
    node_iterator& operator++()
    { ptr ++; return *this; };
    node_iterator operator++(int)
    { auto r = *this; ptr ++; return r; }

    const R* operator*() const
    { return static_cast<const R*>(*ptr); }
    const R* operator->() const
    { return static_cast<const R*>(*ptr); }

    bool operator==(const node_iterator& v) const
    { return ptr == v.ptr; }
    bool operator!=(const node_iterator& v) const
    { return ptr != v.ptr; }

private:
    node_iterator(const PoolAllocator::iterator& _ptr) : ptr(_ptr) {}
    friend class BaseAST<R>;

    PoolAllocator::iterator ptr;
};

template <typename R>
inline typename BaseAST<R>::node_iterator BaseAST<R>::node_begin() const
{ return node_iterator(node_alloc.begin()); }

template <typename R>
inline typename BaseAST<R>::node_iterator BaseAST<R>::node_end() const
{ return node_iterator(node_alloc.end()); }

template <typename R>
template<typename Op>
class BaseAST<R>::op_node_iterator :
    public std::iterator<std::input_iterator_tag, const R>
{
public:
    op_node_iterator& operator++()
    { ptr = next(++ ptr); return *this; }
    op_node_iterator operator++(int)
    { auto r = *this; operator++(); return r; }

    const R* operator*() const
    { return *ptr; }
    const R* operator->() const
    { return *ptr; }

    bool operator==(const op_node_iterator& v) const
    { return ptr == v.ptr; }
    bool operator!=(const op_node_iterator& v) const
    { return ptr != v.ptr; }

protected:
    op_node_iterator(const node_iterator& _ptr, const node_iterator& _end) :
        ptr(_ptr), end(_end)
    {
        ptr = next(ptr);
    }
    op_node_iterator(const node_iterator& _ptr) :
        ptr(_ptr), end(_ptr)
    {}

    node_iterator next(const node_iterator& _ptr)
    { return std::find_if(_ptr, end, Op()); }

    friend class BaseAST<R>;

    node_iterator ptr;
    node_iterator end;
};

struct hash_t : Ext::unary_func_t<size_t, const char*>
{
    size_t operator()(const char* key) const // hash pjw
    {
        unsigned r = 0, t;
        for (const char* p = key; *p; p ++) {
            r = (r << 4) + *p;
            t = r & 0xf0000000;
            if (t) {
                r ^= t >> 24;
                r ^= t;
            }
        }
        return r;
    }
};

struct eq_t : Ext::binary_func_t<bool, const char*>
{
    bool operator()(const char* key1, const char* key2) const
    {
        return strcmp(key1, key2) == 0;
    }
};

struct cmp_t : Ext::binary_func_t<int, const char*>
{
    bool operator()(const char* key1, const char* key2) const
    {
        return strcmp(key1, key2) < 0;
    }
};

template <typename T>
class map_t :
    protected std::unordered_map<const char*, T, hash_t, eq_t>
{
public:
    typedef const char* key_t;
    typedef T val_t;

    typedef std::pair<val_t, bool> result_t;

    map_t() {}

    bool get(key_t key) const;
    bool get(key_t key, val_t& val) const;

    result_t set(key_t key, val_t val);

private:
    typedef std::unordered_map<const char*, T, hash_t, eq_t> base_t;
};

template <typename T>
bool map_t<T>::get(key_t key) const
{
    auto r = base_t::find(key);
    return r != base_t::end();
}

template <typename T>
bool map_t<T>::get(key_t key, val_t& val) const
{
    auto r = base_t::find(key);
    if (r == base_t::end())
        return false;
    val = r->second;
    return true;
}

template <typename T>
typename map_t<T>::result_t map_t<T>::set(key_t key, val_t val)
{
    auto r = base_t::insert(std::make_pair(key, val));
    return std::make_pair((r.first)->second, r.second);
}

} // namespace AST

#endif/*__AST_BASE_HPP*/

